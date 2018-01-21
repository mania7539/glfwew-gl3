#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);
	// above code:
	// synchronize better with slower rate
	// : interval - The minimum number of screen updates to wait for until the buffers are swapped by glfwSwapBuffers.

	if (glewInit() != GLEW_OK) {
		std::cout << "Error!" << std::endl;
	};

	std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;
	float positions[] = {
		-0.5f, -0.5f,	// 0
		 0.5f, -0.5f,	// 1
		 0.5f,  0.5f,	// 2
		-0.5f,  0.5f,	// 3
	}; // This is vertex positions of our triangle
	   // But a "vetex" doesn't only contain position, it can contain more of that
	   // : a position is an attribute, a color is an attribute, a texture coordinate is an attribute
	   // : a normal is an attribute
	   // NOTE: VertexAttribPointer - Attribute is a part which you can deal with your self or managed by opengl by default
	   // : stride - Specifies the byte offset between consecutive generic vertex attributes.
	   //				(: it's the amount of bytes between each vertex, or the size of each vertex)
	   //				you'll get weird artifacts in your rendering or just black screen while you just get only 1 byte out of place

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	}; // it has to be unsigned int than signed


	// below code: used to fix the OpenGL Core profile with a actual Vertex Array Object
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// above code ends

	// below code starts: add a scope to destory vertex and index buffer before the window is terminated which causes an GL_ERROR
	{
		VertexBuffer vertexBuffer(positions, 4 * 2 * sizeof(float));

		// below code for vertex array object concept: keep still at the first time, later can remove
		glEnableVertexAttribArray(0);			// index: it's the index we want to enable attribute.
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
		// above code ends
		// We need to tell opengl how we layout the data with - glVertexAttribPointer
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

		IndexBuffer indexBuffer(indices, 6);

		ShaderProgramSources source = parseShader("res/shaders/basic.shader");
		std::cout << "VERTEX: " << std::endl;
		std::cout << source.vertexSource << std::endl;
		std::cout << "FRAGMENT: " << std::endl;
		std::cout << source.fragmentSource << std::endl;
		unsigned int shader = createShader(source.vertexSource, source.fragmentSource);
		glUseProgram(shader);

		int location = glGetUniformLocation(shader, "u_Color");
		glUniform4f(location, 0.8f, 0.3f, 0.8f, 1.0f);
		// above code:
		// we want to pass vec4 and in float type so the name should contain "4" and "f"
		// put the code below glUseProgram()
		// NOTE: location == -1 then it means the glGetUniformLocation can't find the uniform

		// below code: unset the buffers
		/*glBindVertexArray(0);
		glUseProgram(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
		// above code ends


		float r = 0.0f;
		float increment = 0.05f;
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

			// glDrawArrays(GL_TRIANGLES, 0, 6);
			// above: glDrawArrays ¡X render primitives from array data
			// : first - 0 means the index of the array we want to start
			// : count - the number of axis (x, y) of the array we want to draw
			// NOTE: similar API - glDrawElements(GL_TRIANGLES, 3, UNSIGNED_INT, ...) used with index buffer
			glUseProgram(shader);
			glUniform4f(location, r, 0.3f, 0.8f, 1.0f);
			//glBindVertexArray(vao);
			vertexBuffer.bind();

			// can remove the below "vertex buffer" code and still work: don't need to bind the vertex buffer at all
			/*
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
			*/
			// above code ends

			//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			indexBuffer.bind();

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
			// above code:
			// : count - the number of indices we are drawing not vertexes
			// : indices - since glBindBuffer already binded "ibo", we can pass "nullptr" (we don't have to put anything else in it)
			// IMPORTANT: call glDrawElements(GL_TRIANGLES, 6, GL_INT, nullptr); (with GL_INT than GL_UNSIGNED_INT) will cause black screen and exception.

			if (r > 1.0f) increment = -0.05f;
			else if (r < 0.0f) increment = 0.05f;
			r += increment;

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

		glDeleteProgram(shader);	// delete shader after the app ends
	}
	// above code ends: add a scope

	// TODO: delete vertex buffer and index buffer here
	glfwTerminate();

	return 0;
}