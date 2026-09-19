#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<vector>
#include<cmath>
#include<iostream>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"ShapeGen.h"

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(500, 500, "Task 2 Part 2 - Sunset", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, 500, 500);

	Shader shaderProgram("default.vert", "default.frag");

	const int SEGMENTS = 64;

	// sky
	std::vector<Vertex> sky = MakeGradientQuad(
		-1.0f, -1.0f, 1.0f, 1.0f,
		0.10f, 0.05f, 0.25f, //just gradient sky
		0.95f, 0.55f, 0.25f);

	// sun
	auto sunShade = [](float angle, float& r, float& g, float& b)
	{
		(void)angle;
		r = 1.0f; g = 0.85f; b = 0.35f;
	};
	std::vector<Vertex> sun = MakeEllipseFan(
		0.0f, 0.05f, 0.30f, 0.30f, SEGMENTS,
		1.0f, 1.0f, 0.75f, sunShade);

	// mountains
	auto makeMountain = [](float cx, float baseY, float peakY, float halfWidth,
	                        float peakR, float peakG, float peakB,
	                        float baseR, float baseG, float baseB) -> std::vector<Vertex>
	{
		return {
			{cx - halfWidth, baseY, baseR, baseG, baseB},
			{cx + halfWidth, baseY, baseR, baseG, baseB},
			{cx,              peakY, peakR, peakG, peakB},
		};
	};
	std::vector<Vertex> mountainBack  = makeMountain(-0.35f, -0.55f, 0.05f, 0.55f, 0.30f, 0.20f, 0.35f, 0.05f, 0.05f, 0.15f);
	std::vector<Vertex> mountainFront = makeMountain( 0.30f, -0.60f, -0.05f, 0.60f, 0.20f, 0.10f, 0.20f, 0.02f, 0.02f, 0.08f);

	// gradient star in the sky
	auto starShade = [](float angle, float& r, float& g, float& b)
	{
		(void)angle;
		r = 1.0f; g = 0.9f; b = 0.6f;
	};
	std::vector<Vertex> star = MakeStarFan(
		-0.55f, 0.65f, 0.10f, 0.045f, 5,
		1.0f, 1.0f, 1.0f, starShade);

	// all shapes into one interleaved VBO
	std::vector<Vertex> all;
	int skyStart = 0;              all.insert(all.end(), sky.begin(), sky.end());
	int sunStart = (int)all.size(); all.insert(all.end(), sun.begin(), sun.end());
	int mBackStart = (int)all.size(); all.insert(all.end(), mountainBack.begin(), mountainBack.end());
	int mFrontStart = (int)all.size(); all.insert(all.end(), mountainFront.begin(), mountainFront.end());
	int starStart = (int)all.size(); all.insert(all.end(), star.begin(), star.end());

	VAO VAO1;
	VAO1.Bind();

	VBO VBO1((GLfloat*)all.data(), all.size() * sizeof(Vertex));
	VAO1.LinkAttrib(VBO1, 0, 2, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(2 * sizeof(float)));

	VAO1.Unbind();
	VBO1.Unbind();

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.Activate();
		VAO1.Bind();

		glDrawArrays(GL_TRIANGLE_FAN, skyStart, (GLsizei)sky.size());
		glDrawArrays(GL_TRIANGLE_FAN, sunStart, (GLsizei)sun.size());
		glDrawArrays(GL_TRIANGLES, mBackStart, (GLsizei)mountainBack.size());
		glDrawArrays(GL_TRIANGLES, mFrontStart, (GLsizei)mountainFront.size());
		glDrawArrays(GL_TRIANGLE_FAN, starStart, (GLsizei)star.size());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	VAO1.Delete();
	VBO1.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
