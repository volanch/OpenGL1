//
// Created by vladk on 28.09.2026.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"

static int g_windowWidth = 800;
static int g_windowHeight = 800;

// robot position
static float g_robotX = 0.0f;
static float g_robotY = -0.40f;

// target position
static float g_targetX = 0.45f;
static float g_targetY = 0.20f;
static bool  g_targetPlaced = true;

// animation and input state
static bool  g_animating = true;
static float g_animTime = 0.0f;

static const float PROXIMITY_THRESHOLD = 0.32f;

struct Vertex {
    float x, y;
    float r, g, b;
};

// add filled rectangle built from two triangles
void AddRect(std::vector<Vertex>& verts, float cx, float cy, float w, float h,
             float r, float g, float b)
{
    float x0 = cx - w * 0.5f;
    float x1 = cx + w * 0.5f;
    float y0 = cy - h * 0.5f;
    float y1 = cy + h * 0.5f;

    verts.push_back({x0, y0, r, g, b});
    verts.push_back({x1, y0, r, g, b});
    verts.push_back({x1, y1, r, g, b});

    verts.push_back({x0, y0, r, g, b});
    verts.push_back({x1, y1, r, g, b});
    verts.push_back({x0, y1, r, g, b});
}

// add filled gradient rectangle (per-vertex color interpolation)
void AddGradientRect(std::vector<Vertex>& verts, float cx, float cy, float w, float h,
                     float rTop, float gTop, float bTop,
                     float rBot, float gBot, float bBot)
{
    float x0 = cx - w * 0.5f;
    float x1 = cx + w * 0.5f;
    float y0 = cy - h * 0.5f;
    float y1 = cy + h * 0.5f;

    verts.push_back({x0, y0, rBot, gBot, bBot});
    verts.push_back({x1, y0, rBot, gBot, bBot});
    verts.push_back({x1, y1, rTop, gTop, bTop});

    verts.push_back({x0, y0, rBot, gBot, bBot});
    verts.push_back({x1, y1, rTop, gTop, bTop});
    verts.push_back({x0, y1, rTop, gTop, bTop});
}

// add triangle fan
void AddCircleFan(std::vector<Vertex>& verts, float cx, float cy, float radius, int segments,
                  float rCenter, float gCenter, float bCenter,
                  float rEdge, float gEdge, float bEdge)
{
    verts.push_back({cx, cy, rCenter, gCenter, bCenter});
    const float PI = 3.14159265358979323846f;
    for (int i = 0; i <= segments; ++i)
    {
        float angle = 2.0f * PI * (float)i / (float)segments;
        verts.push_back({cx + radius * cosf(angle), cy + radius * sinf(angle), rEdge, gEdge, bEdge});
    }
}

// add line segment
void AddLine(std::vector<Vertex>& verts, float x0, float y0, float x1, float y1,
             float r, float g, float b)
{
    verts.push_back({x0, y0, r, g, b});
    verts.push_back({x1, y1, r, g, b});
}

// window resizing callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    if (width > 0 && height > 0)
    {
        g_windowWidth = width;
        g_windowHeight = height;
        glViewport(0, 0, width, height);
    }
}

// keyboard input callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)scancode; (void)mods;
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(window, true);
        }
        else if (key == GLFW_KEY_R)
        {
            // reset robot position
            g_robotX = 0.0f;
            g_robotY = -0.40f;
        }
        else if (key == GLFW_KEY_SPACE)
        {
            // toggle animation
            g_animating = !g_animating;
        }
    }
}

// mouse input callback
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    (void)mods;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // convert window pixel coordinates (top-left origin) to OpenGL NDC
        float ndcX = (float)((2.0 * xpos / (double)g_windowWidth) - 1.0);
        float ndcY = (float)(1.0 - (2.0 * ypos / (double)g_windowHeight));

        g_targetX = ndcX;
        g_targetY = ndcY;
        g_targetPlaced = true;
    }
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(g_windowWidth, g_windowHeight, "Interactive Robot Controller", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, g_windowWidth, g_windowHeight);

    // register event callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // enable double buffering and vsync
    glfwSwapInterval(1);

    Shader shaderProgram("robot.vert", "robot.frag");

    // build geometry
    std::vector<Vertex> allVertices;

    // ground platform (gradient rect with visible interpolation)
    int groundStart = (int)allVertices.size();
    AddGradientRect(allVertices, 0.0f, -0.80f, 2.0f, 0.40f,
                    0.20f, 0.28f, 0.40f,
                    0.06f, 0.08f, 0.14f);
    int groundCount = (int)allVertices.size() - groundStart;

    // ground accent stripe
    int groundStripeStart = (int)allVertices.size();
    AddRect(allVertices, 0.0f, -0.60f, 2.0f, 0.015f, 0.35f, 0.65f, 0.85f);
    int groundStripeCount = (int)allVertices.size() - groundStripeStart;

    // some kind off charging station (obstacle pillar)
    int obstacleStart = (int)allVertices.size();
    // pillar body
    AddGradientRect(allVertices, -0.72f, -0.25f, 0.20f, 0.70f,
                    0.45f, 0.48f, 0.55f,  // Top metallic
                    0.18f, 0.20f, 0.25f); // Base
    // hazard stripe accents
    AddRect(allVertices, -0.72f, -0.05f, 0.16f, 0.04f, 0.95f, 0.75f, 0.15f);
    AddRect(allVertices, -0.72f, -0.20f, 0.16f, 0.04f, 0.95f, 0.75f, 0.15f);
    AddRect(allVertices, -0.72f, -0.35f, 0.16f, 0.04f, 0.95f, 0.75f, 0.15f);
    int obstacleCount = (int)allVertices.size() - obstacleStart;

    // target marker
    int targetCoreStart = (int)allVertices.size();
    AddCircleFan(allVertices, 0.0f, 0.0f, 0.045f, 16,
                1.0f, 1.0f, 1.0f,
                 0.2f, 0.8f, 1.0f);
    int targetCoreCount = (int)allVertices.size() - targetCoreStart;

    // fan
    int targetRingStart = (int)allVertices.size();
    AddCircleFan(allVertices, 0.0f, 0.0f, 0.085f, 20,
                 0.1f, 0.5f, 0.8f,
                 0.0f, 0.3f, 0.7f);
    int targetRingCount = (int)allVertices.size() - targetRingStart;

    // target crosshair lines (GL_LINES)
    int targetLinesStart = (int)allVertices.size();
    AddLine(allVertices, -0.12f, 0.0f, 0.12f, 0.0f, 0.3f, 0.9f, 1.0f);
    AddLine(allVertices, 0.0f, -0.12f, 0.0f, 0.12f, 0.3f, 0.9f, 1.0f);
    int targetLinesCount = (int)allVertices.size() - targetLinesStart;

    // robot
    // body (gradient rect visible per-vertex color interpolation!)
    int torsoStart = (int)allVertices.size();
    AddGradientRect(allVertices, 0.0f, 0.04f, 0.22f, 0.28f,
                    0.25f, 0.70f, 0.92f,
                    0.10f, 0.22f, 0.45f);
    int torsoCount = (int)allVertices.size() - torsoStart;

    // head
    int headStart = (int)allVertices.size();
    AddRect(allVertices, 0.0f, 0.25f, 0.16f, 0.12f, 0.55f, 0.62f, 0.72f);
    int headCount = (int)allVertices.size() - headStart;

    // eyes (rectangle)
    int visorStart = (int)allVertices.size();
    AddRect(allVertices, 0.0f, 0.26f, 0.12f, 0.04f, 0.05f, 0.95f, 0.90f);
    int visorCount = (int)allVertices.size() - visorStart;

    // arms (filled rectangular parts from triangles)
    int armsStart = (int)allVertices.size();
    AddRect(allVertices, -0.15f, 0.04f, 0.055f, 0.22f, 0.38f, 0.45f, 0.55f); // Left arm
    AddRect(allVertices,  0.15f, 0.04f, 0.055f, 0.22f, 0.38f, 0.45f, 0.55f); // Right arm
    // arm joint details
    AddRect(allVertices, -0.15f, 0.13f, 0.065f, 0.035f, 0.75f, 0.80f, 0.88f);
    AddRect(allVertices,  0.15f, 0.13f, 0.065f, 0.035f, 0.75f, 0.80f, 0.88f);
    int armsCount = (int)allVertices.size() - armsStart;

    // legs (filled rectangular parts from triangles)
    int legsStart = (int)allVertices.size();
    AddRect(allVertices, -0.065f, -0.18f, 0.06f, 0.18f, 0.22f, 0.28f, 0.36f); // Left leg
    AddRect(allVertices,  0.065f, -0.18f, 0.06f, 0.18f, 0.22f, 0.28f, 0.36f); // Right leg
    // foot pads
    AddRect(allVertices, -0.075f, -0.27f, 0.08f, 0.03f, 0.45f, 0.50f, 0.58f);
    AddRect(allVertices,  0.075f, -0.27f, 0.08f, 0.03f, 0.45f, 0.50f, 0.58f);
    int legsCount = (int)allVertices.size() - legsStart;

    // antenna stem (GL_LINES) and antenna tip (triangle fan)
    int antennaLineStart = (int)allVertices.size();
    AddLine(allVertices, 0.0f, 0.31f, 0.0f, 0.40f, 0.80f, 0.85f, 0.90f);
    int antennaLineCount = (int)allVertices.size() - antennaLineStart;

    int antennaTipStart = (int)allVertices.size();
    AddCircleFan(allVertices, 0.0f, 0.41f, 0.022f, 10,
                 1.0f, 0.9f, 0.2f,
                 1.0f, 0.5f, 0.0f);
    int antennaTipCount = (int)allVertices.size() - antennaTipStart;

    // chest indicator (animated fan)
    int chestCoreStart = (int)allVertices.size();
    AddCircleFan(allVertices, 0.0f, 0.06f, 0.035f, 12,
                 1.0f, 1.0f, 1.0f,
                 0.95f, 0.35f, 0.20f);
    int chestCoreCount = (int)allVertices.size() - chestCoreStart;

    // upload geometry to GPU via VAO and VBO
    VAO VAO1;
    VAO1.Bind();

    VBO VBO1((GLfloat*)allVertices.data(), allVertices.size() * sizeof(Vertex));
    // location 0: 2D Position (vec2)
    VAO1.LinkAttrib(VBO1, 0, 2, GL_FLOAT, sizeof(Vertex), (void*)0);
    // location 1: RGB Color (vec3)
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(2 * sizeof(float)));

    VAO1.Unbind();
    VBO1.Unbind();

    // query uniform locations
    shaderProgram.Activate();
    GLint uOffsetLoc = glGetUniformLocation(shaderProgram.ID, "uOffset");
    GLint uScaleLoc = glGetUniformLocation(shaderProgram.ID, "uScale");
    GLint uTimeLoc = glGetUniformLocation(shaderProgram.ID, "uTime");
    GLint uIsCloseLoc = glGetUniformLocation(shaderProgram.ID, "uIsClose");
    GLint uColorOverrideLoc = glGetUniformLocation(shaderProgram.ID, "uColorOverride");
    GLint uOverrideMixLoc = glGetUniformLocation(shaderProgram.ID, "uOverrideMix");

    double lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double currentFrameTime = glfwGetTime();
        float dt = (float)(currentFrameTime - lastFrameTime);
        lastFrameTime = currentFrameTime;

        // animation clock if running
        if (g_animating)
        {
            g_animTime += dt;
        }

        // keyboard movement
        float moveSpeed = 1.1f * dt;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) g_robotY += moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) g_robotY -= moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) g_robotX -= moveSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) g_robotX += moveSpeed;

        // robot only within visible NDC region
        g_robotX = std::max(-0.82f, std::min(0.82f, g_robotX));
        g_robotY = std::max(-0.45f, std::min(0.70f, g_robotY));

        // proximity detection
        float robotCenterY = g_robotY + 0.05f;
        float dx = g_robotX - g_targetX;
        float dy = robotCenterY - g_targetY;
        float distance = std::sqrt(dx * dx + dy * dy);
        bool isClose = (distance < PROXIMITY_THRESHOLD);

        glClearColor(0.08f, 0.09f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.Activate();
        VAO1.Bind();

        // pass continuous animation time
        glUniform1f(uTimeLoc, g_animTime);

        // static environment (offset = 0, scale = 1)
        glUniform2f(uOffsetLoc, 0.0f, 0.0f);
        glUniform1f(uScaleLoc, 1.0f);
        glUniform1i(uIsCloseLoc, 0);
        glUniform1f(uOverrideMixLoc, 0.0f);

        // ground platform and stripe (GL_TRIANGLES)
        glDrawArrays(GL_TRIANGLES, groundStart, groundCount);
        glDrawArrays(GL_TRIANGLES, groundStripeStart, groundStripeCount);

        // obstacle pillar (GL_TRIANGLES)
        glDrawArrays(GL_TRIANGLES, obstacleStart, obstacleCount);

        // target marker
        if (g_targetPlaced)
        {
            // continuous pulsating scale
            float targetPulse = 1.0f + 0.18f * sinf(g_animTime * 4.5f);
            glUniform2f(uOffsetLoc, g_targetX, g_targetY);
            glUniform1f(uScaleLoc, targetPulse);

            if (isClose)
            {
                // when robot is close to target changing target color
                glUniform1i(uIsCloseLoc, 1);
                glUniform1f(uOverrideMixLoc, 0.85f);
                glUniform3f(uColorOverrideLoc, 0.25f, 1.00f, 0.35f);
            }
            else
            {
                glUniform1i(uIsCloseLoc, 0);
                glUniform1f(uOverrideMixLoc, 0.0f);
            }

            // outer ring and core (GL_TRIANGLE_FAN)
            glDrawArrays(GL_TRIANGLE_FAN, targetRingStart, targetRingCount);
            glDrawArrays(GL_TRIANGLE_FAN, targetCoreStart, targetCoreCount);

            // crosshair lines (GL_LINES)
            glLineWidth(2.5f);
            glDrawArrays(GL_LINES, targetLinesStart, targetLinesCount);
        }

        // robot
        glUniform2f(uOffsetLoc, g_robotX, g_robotY);
        glUniform1f(uScaleLoc, 1.0f);
        glUniform1i(uIsCloseLoc, isClose ? 1 : 0);
        glUniform1f(uOverrideMixLoc, 0.0f);

        // legs, arms, head, torso, visor (GL_TRIANGLES)
        glDrawArrays(GL_TRIANGLES, legsStart, legsCount);
        glDrawArrays(GL_TRIANGLES, armsStart, armsCount);
        glDrawArrays(GL_TRIANGLES, headStart, headCount);
        glDrawArrays(GL_TRIANGLES, torsoStart, torsoCount);
        glDrawArrays(GL_TRIANGLES, visorStart, visorCount);

        // antenna stem (GL_LINES)
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, antennaLineStart, antennaLineCount);

        // antenna tip (GL_TRIANGLE_FAN)
        glDrawArrays(GL_TRIANGLE_FAN, antennaTipStart, antennaTipCount);

        // chest indicator (pulsing based on animation time)
        if (isClose)
        {
            float rapidPulse = 0.5f + 0.5f * sinf(g_animTime * 12.0f);
            glUniform1f(uOverrideMixLoc, rapidPulse * 0.9f);
            glUniform3f(uColorOverrideLoc, 0.2f, 1.0f, 0.3f);
        }
        else
        {
            float slowPulse = 0.4f + 0.4f * sinf(g_animTime * 3.5f);
            glUniform1f(uOverrideMixLoc, slowPulse);
            glUniform3f(uColorOverrideLoc, 1.0f, 0.85f, 0.2f);
        }
        glDrawArrays(GL_TRIANGLE_FAN, chestCoreStart, chestCoreCount);

        // reset state
        glUniform1f(uOverrideMixLoc, 0.0f);
        glUniform1i(uIsCloseLoc, 0);

        // double buffering swaping front and back buffers
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