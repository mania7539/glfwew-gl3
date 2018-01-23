#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"

Shader::Shader(const std::string & filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSources source = parseShader(filepath);
	std::cout << "VERTEX: " << std::endl;
	std::cout << source.vertexSource << std::endl;
	std::cout << "FRAGMENT: " << std::endl;
	std::cout << source.fragmentSource << std::endl;
	m_RendererID = createShader(source.vertexSource, source.fragmentSource);

}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));

}

void Shader::bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::setUniform4f(const std::string & name, float f0, float f1, float f2, float f3)
{
	GLCall(glUniform4f(getUniformLocation(name), f0, f1, f2, f3));
}

int Shader::getUniformLocation(const std::string & name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	
	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	
	m_UniformLocationCache[name] = location;

	return location;
}

ShaderProgramSources Shader::parseShader(const std::string& filepath) {

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	std::cout << filepath << std::endl;
	std::ifstream stream(filepath);
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line)) {
		// npos is basically an invalid string position
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				// set mode to vertex
				type = ShaderType::VERTEX;

			}
			else if (line.find("fragment") != std::string::npos) {
				// set mode to fragment
				type = ShaderType::FRAGMENT;
			}

		}
		else {
			ss[(int)type] << line << '\n';
		}
	}

	return {
		ss[0].str(), ss[1].str()
	};
}

unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);	// after the shaders are linked, we can delete the intermediates.
	glDeleteShader(fs);

	return program;
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// TODO: Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);	// iv: i = integer, v = vector
	if (!result) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}
