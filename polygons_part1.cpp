#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<vector>
#include<cmath>
#include<iostream>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"shapeGen.h"

int main() {
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(500, 500, "Task 2 Part 1 - Polygons", NULL, NULL);
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

	// Dark shade
	const float lightAngle = 5.0f * PI / 4.0f; // 225 deg
	auto circleShade = [&](float angle, float& r, float& g, float& b)
	{
		float t = 0.5f * (1.0f + cosf(angle - lightAngle));
		t = powf(t, 5.0f); // high exponent
		r = 0.05f + 0.95f * t; // Deep shadow (0.05) to bright red (1.0)
		g = 0.0f;
		b = 0.0f;
	};

	float circleRadius = 0.28f;
	float offX = circleRadius * 0.60f * cosf(lightAngle);
	float offY = circleRadius * 0.60f * sinf(lightAngle);

	// circle
	std::vector<Vertex> circle = MakeEllipseFan(
		0.55f, 0.55f, circleRadius, circleRadius, SEGMENTS,
		1.0f, 0.0f, 0.0f, circleShade, offX, offY);

	// ellipse (solid color)
	auto solidRed = [](float /*angle*/, float& r, float& g, float& b)
	{
		r = 0.9f;
		g = 0.0f;
		b = 0.0f;
	};
	std::vector<Vertex> ellipse = MakeEllipseFan(
		-0.55f, 0.55f, 0.28f, 0.28f * 0.6f, SEGMENTS,
		0.9f, 0.0f, 0.0f, solidRed);

	// triangle
	std::vector<Vertex> triangle = MakeTriangle(
		0.0f, 0.53f, 0.30f,
		1.0f, 0.0f, 0.0f,   // top red
		0.0f, 1.0f, 0.0f,   // bottom-left green
		0.0f, 0.0f, 1.0f);  // bottom-right blue

	// squares
	const int RING_COUNT = 6;
	const float OUTER_HALF = 0.45f;
	const float STEP = 0.07f;
	const float squareCenterX = 0.0f, squareCenterY = -0.35f;

	std::vector<Vertex> squares;
	std::vector<int> squareStart, squareCount;
	for (int i = 0; i < RING_COUNT; i++)
	{
		float halfSide = OUTER_HALF - i * STEP;
		float c = (i % 2 == 0) ? 1.0f : 0.0f; // even index white, odd black
		std::vector<Vertex> sq = MakeSquare(squareCenterX, squareCenterY, halfSide, c, c, c);
		squareStart.push_back((int)squares.size());
		squareCount.push_back((int)sq.size());
		squares.insert(squares.end(), sq.begin(), sq.end());
	}

	// one big buffer
	std::vector<Vertex> all;
	int circleStart = 0;
	all.insert(all.end(), circle.begin(), circle.end());
	int ellipseStart = (int)all.size();
	all.insert(all.end(), ellipse.begin(), ellipse.end());
	int triangleStart = (int)all.size();
	all.insert(all.end(), triangle.begin(), triangle.end());
	int squaresBaseStart = (int)all.size();
	all.insert(all.end(), squares.begin(), squares.end());

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

		glDrawArrays(GL_TRIANGLE_FAN, circleStart, (GLsizei)circle.size());
		glDrawArrays(GL_TRIANGLE_FAN, ellipseStart, (GLsizei)ellipse.size());
		glDrawArrays(GL_TRIANGLES, triangleStart, (GLsizei)triangle.size());
		for (int i = 0; i < RING_COUNT; i++)
			glDrawArrays(GL_TRIANGLE_FAN, squaresBaseStart + squareStart[i], squareCount[i]);

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
