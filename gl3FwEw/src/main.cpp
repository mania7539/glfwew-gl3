#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSources {
	std::string vertexSource;
	std::string fragmentSource;
};

static ShaderProgramSources parseShader(const std::string& filepath) {

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


static unsigned int compileShader(unsigned int type, const std::string& source) {
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

static int createShader(const std::string& vertexShader, const std::string& fragmentShader) {
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


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "Error!" << std::endl;
	};

	std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;
	float positions[6] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	}; // This is vertex positions of our triangle
	   // But a "vetex" doesn't only contain position, it can contain more of that
	   // : a position is an attribute, a color is an attribute, a texture coordinate is an attribute
	   // : a normal is an attribute
	   // NOTE: VertexAttribPointer - Attribute is a part which you can deal with your self or managed by opengl by default
	   // : stride - Specifies the byte offset between consecutive generic vertex attributes.
	   //				(: it's the amount of bytes between each vertex, or the size of each vertex)
	   //				you'll get weird artifacts in your rendering or just black screen while you just get only 1 byte out of place

	unsigned int buffer;
	glGenBuffers(1, &buffer);				// Generate/Create a GL Buffer, we should provide an Integer as a memory which we can write into 
	glBindBuffer(GL_ARRAY_BUFFER, buffer);	// How do I want to use the GL Buffer? Define it to a specific buffer:
											//	target - GL_ARRAY_BUFFER: it's just an array
											//	buffer - an integer comes from memory
											// if we use glBindBuffer(GL_ARRAY_BUFFER, 0); then GPU won't draw the triangle out since we bind something else

	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);
	// above: Set data which we want to use to the specific GPU buffer
	// : STATIC, DYNAMIC: we should let GPU knows that if the buffer can be modified more than ONCE.
	// : DRAW: we want to draw things with the buffer, so use it

	glEnableVertexAttribArray(0);	// index: it's the index we want to enable attribute.
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	// we need to tell opengl how we layout the data with - glVertexAttribPointer
	// example: glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (const void*) 8);
	// : normal: GL_FALSE - if we want them to be normalized
	// : pass 4*2 = 8 as offset (in byte) to the texture coordinate - converts to a pointer with (const void*)
	// IMPORTANT: we need to enable attribute feature with - glEnableVertexAttribArray

	// vertex shader:
	// gl_Position is actually a "vec4" (definition I guess), 
	//		and it's value should be as the same as in "index" argument of: glVertexAttribPointer
	//		so when we specify layout(location = 0), it should also be vec4, 
	//		or we need to cast it to vec4 while we assign it to gl_Position with vec4(position.xy)
	// https://www.youtube.com/watch?v=71BLZwRGUJE&index=7&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2
	// don't need to use + to concatenate string, since it's in c++ style
	/*std::string vertexShader =
	"\\\\comment"
	"#version 330 core\n"
	"\n"
	"layout(location = 0) in vec4 position;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = position;\n"
	"}\n";*/

	// fragment shader: assign "color" with RGBA value => Red, the layout of RGBA sequence actually depends on your framebuffer format
	/*std::string fragmentShader =
	"#version 330 core\n"
	"\n"
	"layout(location = 0) out vec4 color;\n"
	"void main()\n"
	"{\n"
	"	color = vec4(1.0, 0.0, 0.0, 1.0);\n"
	"}\n";*/
	//unsigned int shader = createShader(vertexShader, fragmentShader);
	//glUseProgram(shader);
	ShaderProgramSources source = parseShader("res/shaders/basic.shader");
	std::cout << "VERTEX: " << std::endl;
	std::cout << source.vertexSource << std::endl;
	std::cout << "FRAGMENT: " << std::endl;
	std::cout << source.fragmentSource << std::endl;
	unsigned int shader = createShader(source.vertexSource, source.fragmentSource);
	glUseProgram(shader);

	/*
	* Loop until the user closes the window:
	* : This is like onDraw, so it will draw on each frame when updating,
	* : but we don't have to put the buffer code here, but draw command only
	*/
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		// TODO: modern gl codes begin:
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// above: glDrawArrays ¡X render primitives from array data
		// : first - 0 means the index of the array we want to start
		// : count - the number of axis (x, y) of the array we want to draw
		// NOTE: similar API - glDrawElements(GL_TRIANGLES, 3, UNSIGNED_INT, ...) used with index buffer

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	//glDeleteShader(shader);	// delete shader after the app ends

	glfwTerminate();
	return 0;
}