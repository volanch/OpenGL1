#include <glad/glad.h>
#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "cube.h"
#include "transform.h"

namespace {
    Shader* g_shader = nullptr;
    VAO* g_vao = nullptr;
    int g_count = 0;
    Mode g_mode = Mode::Rotate;
    TransformState g_t;
}

void display() {
    glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    g_shader->Activate();
    glUniformMatrix4fv(glGetUniformLocation(g_shader->ID, "uTransform"),
                       1, GL_FALSE, glm::value_ptr(g_t.Matrix()));
    g_vao->Bind();
    glDrawArrays(GL_TRIANGLES, 0, g_count);
    glutSwapBuffers();
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 'x': g_t.Adjust(g_mode, 0, +1); break;
        case 'X': g_t.Adjust(g_mode, 0, -1); break;
        case 'y': g_t.Adjust(g_mode, 1, +1); break;
        case 'Y': g_t.Adjust(g_mode, 1, -1); break;
        case 'z': g_t.Adjust(g_mode, 2, +1); break;
        case 'Z': g_t.Adjust(g_mode, 2, -1); break;
        case '+': case '=': g_t.AdjustDelta(g_mode, +1); break;
        case '-': case '_': g_t.AdjustDelta(g_mode, -1); break;
        case 'r': case 'R': g_t.Reset(); break;
        case 'i': case 'I': g_t.Isometric(); break;
        case 27: case 'q': case 'Q': glutLeaveMainLoop(); return;
        default: return;
    }
    glutPostRedisplay();
}

void menuHandler(int option) { g_mode = static_cast<Mode>(option); }

void PrintInstructions() {
    std::cout << " x / X : Increase / Decrease X component of current transformation\n"
              << " y / Y : Increase / Decrease Y component of current transformation\n"
              << " z / Z : Increase / Decrease Z component of current transformation\n"
              << " + / - : Increase / Decrease delta step for current transformation\n"
              << " r / R : Reset all transformations and deltas\n"
              << " i / I : Set to Isometric View (hexagon silhouette)\n"
              << " q / ESC : Quit program\n" << std::endl;}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Task 2 Part 2 - 3D Colour Cube");
    gladLoadGL();
    glEnable(GL_DEPTH_TEST);

    Shader shader("shaders/cube.vert", "shaders/cube.frag");
    g_shader = &shader;

    auto cube = colorcube();
    g_count = (int)cube.size();
    VBO vbo((GLfloat*)cube.data(), cube.size() * sizeof(Vertex3D));
    VAO vao;
    vao.Bind();
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex3D), (void*)0);
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex3D), (void*)(3 * sizeof(float)));
    vao.Unbind();
    g_vao = &vao;

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutCreateMenu(menuHandler);
    glutAddMenuEntry("SCALE", (int)Mode::Scale);
    glutAddMenuEntry("ROTATE", (int)Mode::Rotate);
    glutAddMenuEntry("TRANSLATE", (int)Mode::Translate);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    PrintInstructions();
    glutMainLoop();
    return 0;
}