#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<vector>
#include<cmath>
#include<iostream>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"shapeGen.h"

static bool g_nightTarget = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window; (void)scancode; (void)mods;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        g_nightTarget = !g_nightTarget;
        std::cout << (g_nightTarget ? "Night mode\n" : "Day mode\n");
    }
} //space = switch day/night

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Digital Postcard", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, 800, 800);
    glfwSetKeyCallback(window, key_callback);

    Shader shaderProgram("default.vert", "default.frag");

    const int SEGMENTS = 64;

    // sky is gradient quad. Vertex colors differ top to bottom, which is the per-vertex color-interpolation
    std::vector<Vertex> sky = MakeGradientQuad(
        -1.0f, -1.0f, 1.0f, 1.0f,
        0.10f, 0.05f, 0.25f,
        0.95f, 0.55f, 0.25f);

    // Sun and Moon one ellipse fan is for both
    auto sunShade = [](float angle, float& r, float& g, float& b)
    {
        (void)angle;
        r = 1.0f; g = 0.85f; b = 0.35f;
    };
    std::vector<Vertex> sunMoon = MakeEllipseFan(
        0.0f, 0.05f, 0.28f, 0.28f, SEGMENTS,
        1.0f, 1.0f, 0.75f, sunShade);

    // Mountains: two GL_TRIANGLES silhouettes
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

    // star in night mode
    auto starShade = [](float angle, float& r, float& g, float& b)
    {
        (void)angle;
        r = 1.0f; g = 0.9f; b = 0.6f;
    };
    std::vector<Vertex> eveningStar = MakeStarFan(
        -0.60f, 0.68f, 0.10f, 0.045f, 5,
        1.0f, 1.0f, 1.0f, starShade);

    // lots of little stars GL_POINTS scattered across the upper sky in night mode
    std::vector<Vertex> starField = MakeStarField(
        60, /*seed=*/1337,
        -0.95f, 0.95f, 0.0f, 0.95f,
        0.95f, 0.95f, 1.0f);

    // birds GL_LINES, in the day mode
    std::vector<Vertex> birds;
    {
        auto b1 = MakeBird(-0.55f, 0.55f, 0.14f, 0.05f, 0.05f, 0.05f);
        auto b2 = MakeBird(-0.40f, 0.62f, 0.10f, 0.05f, 0.05f, 0.05f);
        auto b3 = MakeBird(-0.68f, 0.44f, 0.09f, 0.05f, 0.05f, 0.05f);
        birds.insert(birds.end(), b1.begin(), b1.end());
        birds.insert(birds.end(), b2.begin(), b2.end());
        birds.insert(birds.end(), b3.begin(), b3.end());
    }

    // every shape into one interleaved VBO
    std::vector<Vertex> all;
    int skyStart      = 0;                 all.insert(all.end(), sky.begin(), sky.end());
    int sunMoonStart   = (int)all.size();   all.insert(all.end(), sunMoon.begin(), sunMoon.end());
    int mBackStart     = (int)all.size();   all.insert(all.end(), mountainBack.begin(), mountainBack.end());
    int mFrontStart    = (int)all.size();   all.insert(all.end(), mountainFront.begin(), mountainFront.end());
    int starStart       = (int)all.size();  all.insert(all.end(), eveningStar.begin(), eveningStar.end());
    int starFieldStart = (int)all.size();   all.insert(all.end(), starField.begin(), starField.end());
    int birdsStart      = (int)all.size();  all.insert(all.end(), birds.begin(), birds.end());

    VAO VAO1;
    VAO1.Bind();

    VBO VBO1((GLfloat*)all.data(), all.size() * sizeof(Vertex));
    VAO1.LinkAttrib(VBO1, 0, 2, GL_FLOAT, sizeof(Vertex), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(2 * sizeof(float)));

    VAO1.Unbind();
    VBO1.Unbind();

    // uniform locations
    shaderProgram.Activate();
    GLint offsetLoc = glGetUniformLocation(shaderProgram.ID, "uOffset");
    GLint mixLoc    = glGetUniformLocation(shaderProgram.ID, "uNightMix");
    GLint tintLoc   = glGetUniformLocation(shaderProgram.ID, "uNightTint");

    float nightMix = 0.0f; // 0 - day, 1 - night
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        float dt = (float)(now - lastTime);
        lastTime = now;

        // ease uNightMix towards the target set by the Space key.
        float target = g_nightTarget ? 1.0f : 0.0f;
        float speed = 1.2f; // speed for transition
        if (nightMix < target) nightMix = std::min(target, nightMix + speed * dt);
        else if (nightMix > target) nightMix = std::max(target, nightMix - speed * dt);
        bool isNight = nightMix > 0.5f;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.Activate();
        VAO1.Bind();

        // uniform number one blend every vertex color towards a cool tint as nightMix goes 0 -> 1
        glUniform1f(mixLoc, nightMix);
        glUniform3f(tintLoc, 0.35f, 0.42f, 0.85f);
        glUniform2f(offsetLoc, 0.0f, 0.0f);

        // sky
        glDrawArrays(GL_TRIANGLE_FAN, skyStart, (GLsizei)sky.size());

        // uniform number 2 move the sun into moon position as night falls
        glUniform2f(offsetLoc, -0.05f * nightMix, 0.30f * nightMix);
        glDrawArrays(GL_TRIANGLE_FAN, sunMoonStart, (GLsizei)sunMoon.size());
        glUniform2f(offsetLoc, 0.0f, 0.0f);

        // mountains literal GL_TRIANGLES primitive
        glDrawArrays(GL_TRIANGLES, mBackStart, (GLsizei)mountainBack.size());
        glDrawArrays(GL_TRIANGLES, mFrontStart, (GLsizei)mountainFront.size());

        if (isNight)
        {
            // evening star
            glDrawArrays(GL_TRIANGLE_FAN, starStart, (GLsizei)eveningStar.size());

            // little stars GL_POINTS primitive
            glPointSize(3.0f);
            glDrawArrays(GL_POINTS, starFieldStart, (GLsizei)starField.size());
        }
        else
        {
            // birds GL_LINES primitive
            glLineWidth(2.0f);
            glDrawArrays(GL_LINES, birdsStart, (GLsizei)birds.size());
        }

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