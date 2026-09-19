#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<cmath>

#include"VAO.h"
#include"VBO.h"
#include"shaderClass.h"

int main()
{
	glfwInit();
	// version of OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// only CORE profile (modern functions)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLfloat vertices[] =
	{
		-0.5f, -0.5f, // * float(sqrt(3)) / 3,
		0.5f, -0.5f, // * float(sqrt(3)) / 3,
		0.0f, 0.5f, // * float(sqrt(3)) * 2 / 3, 0.0f,
	};

	GLFWwindow* window = glfwCreateWindow(800, 800, "Task 1 - Red triangle", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// window into current context
	glfwMakeContextCurrent(window);

	// load GLAD to configure OpenGL
	gladLoadGL();

	// render
	glViewport(0, 0, 800, 800);

	Shader shaderProgram("solid.vert", "solid.frag");

	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));
	VAO1.LinkVBO(VBO1, 0, 2);

	VAO1.Unbind();
	VBO1.Unbind();

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.5f, 0.0f, 0.6f);
		// clean the back buffer with my color
		glClear(GL_COLOR_BUFFER_BIT);
		shaderProgram.Activate();
		shaderProgram.SetVec3("uColor", 1.0f, 0.0f, 0.0f); //red
		VAO1.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		// all GLFW events
		glfwPollEvents();
	}

	VAO1.Delete();
	VBO1.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}