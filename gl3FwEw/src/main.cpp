#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

	unsigned int a;
	glGenBuffers(1, &a);

	// TODO: GLEW code
	// Documentation: We need to create a valid OpenGL rendering context
	// where do we do this? glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	std::cout << "GLVERSION: " << glGetString(GL_VERSION) << std::endl;
	// output: GLVERSION: 2.1 INTEL-10.30.12
	//                    ^GL ^Driver provider software version (on My Mac)
	// output: GLVERSION: 4.2.0 Build 10.18.10.3643
	//                    ^GL   ^Driver provider software version(on My Windows8.1)

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		// START: legacy opengl
		glBegin(GL_TRIANGLES);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f(0.0f, 0.5f);
		glVertex2f(0.5f, -0.5f);
		glEnd();
		// END: legacy opengl

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}