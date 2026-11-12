#include "Window.h"
#include <glad/glad.h>
#include <GL/freeglut.h>
#include <cstdio>
#include <cstdlib>

void initWindow(int argc, char** argv, const char* title, int w, int h) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(w, h);
    glutCreateWindow(title);
    if (!gladLoadGL()) { std::printf("gladLoadGL failed\n"); std::exit(1); }
}
