#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <cmath>

#include"shaderClass.h"
#include"VBO.h"
#include"EBO.h"
#include"VAO.h"

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
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f,
	};

	GLuint indices[] =
	{
		0, 1, 2,  // bottom left, bottom right, top right
		0, 2, 3,  // bottom left, top right, top left
	};

	GLFWwindow* window = glfwCreateWindow(800, 800, "Task 1 - Blue square", NULL, NULL);

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
	EBO EBO1(indices, sizeof(indices));

	VAO1.LinkVBO(VBO1, 0, 2);
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.5f, 0.0f, 0.6f);
		// clean the back buffer with my color
		glClear(GL_COLOR_BUFFER_BIT);
		shaderProgram.Activate();
		shaderProgram.SetVec3("uColor", 0.0f, 0.0f, 1.0f); //blue
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		// all GLFW events
		glfwPollEvents();
	}

	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
