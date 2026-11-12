#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "Framebuffer.h"
#include "OrbitCamera.h"
#include "ShaderProgram.h"
#include "Surfaces.h"
#include "VertexBuffer.h"
#include "Window.h"

namespace {
    const int NUM_OBJECTS = 3;
    VertexBuffer objects[NUM_OBJECTS];
    Vec3 offsets[NUM_OBJECTS] = {{-2.6f, 0, 0}, {0, 0, 0}, {2.6f, 0, 0}};
    Vec3 colors[NUM_OBJECTS] = {{0.9f, 0.2f, 0.2f}, {0.2f, 0.8f, 0.3f}, {0.2f, 0.4f, 0.9f}};

    OrbitCamera cam;
    ShaderProgram prog;
    Framebuffer pickFbo, msFbo;
    int winW = 800, winH = 600;
    bool antialias = true;
    bool showPick = true;

    float frand() { return std::rand() / (float)RAND_MAX; }

    void drawScene(bool pick) {
        if (pick) glClearColor(0, 0, 0, 1); else glClearColor(0.12f, 0.12f, 0.15f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        prog.use();
        prog.setMat4("view", cam.view());
        prog.setMat4("proj", perspective(45, (float)winW / winH, 0.1f, 100));
        prog.setInt("pickMode", pick ? 1 : 0);
        for (int i = 0; i < NUM_OBJECTS; i++) {
            prog.setMat4("model", translate(offsets[i]));
            prog.setVec3("kd", colors[i]);
            if (showPick) {
                Vec3 visColors[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
                prog.setVec3("pickColor", visColors[i]);
            } else {
                float id = (i + 1) / 255.0f;
                prog.setVec3("pickColor", {id, id, id});
            }
            objects[i].draw(GL_TRIANGLES);
        }
    }

    int pickObject(int x, int y) {
        pickFbo.bind();
        glViewport(0, 0, winW, winH);
        drawScene(true);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        unsigned char px[3] = {0, 0, 0};
        glReadPixels(x, winH - 1 - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, px);
        Framebuffer::bindDefault();
        return (int)px[0] - 1;
    }

    void display() {
        if (showPick) {
            Framebuffer::bindDefault();
            drawScene(true);
        } else if (antialias) {
            msFbo.bind();
            drawScene(false);
            msFbo.bindForRead();
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, winW, winH, 0, 0, winW, winH, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        } else {
            Framebuffer::bindDefault();
            drawScene(false);
        }
        Framebuffer::bindDefault();
        glutSwapBuffers();
    }

    void reshape(int w, int h) {
        winW = w; winH = h > 0 ? h : 1;
        glViewport(0, 0, winW, winH);
        pickFbo.destroy(); msFbo.destroy();
        pickFbo.create(winW, winH, 0);
        msFbo.create(winW, winH, 4);
    }

    void mouse(int button, int state, int x, int y) {
        cam.mouse(button, state, x, y);
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            int i = pickObject(x, y);
            if (i >= 0) { colors[i] = {frand(), frand(), frand()}; std::printf("clicked object %d\n", i); }
            glutPostRedisplay();
        }
    }
    void motion(int x, int y) { if (cam.motion(x, y)) glutPostRedisplay(); }
    void special(int key, int, int) { if (cam.special(key)) glutPostRedisplay(); }

    void keyboard(unsigned char key, int, int) {
        key = (unsigned char)std::tolower(key);
        if (key == 'a') {
            antialias = !antialias;
            glutSetWindowTitle(antialias ? "Picking + anti-aliasing: ON" : "Picking + anti-aliasing: OFF");
        } else if (key == 'p') showPick = !showPick;
        else if (key == 'r') cam.reset();
        else if (key == 'z') cam.zoom(0.9f);
        else if (key == 'x') cam.zoom(1.1f);
        else if (key == 27) std::exit(0);
        glutPostRedisplay();
    }

    void initGeometry() {
        const SurfaceFn pos[NUM_OBJECTS] = {spherePos, torusPos, conePos};
        const SurfaceFn nrm[NUM_OBJECTS] = {spherePos, torusNormal, coneNormal};   // sphere normal = position
        for (int i = 0; i < NUM_OBJECTS; i++) {
            objects[i].init({3, 3});
            objects[i].set(makeSurfaceMesh(pos[i], nrm[i]));
        }
    }
}

int main(int argc, char** argv) {
    std::srand((unsigned)std::time(nullptr));
    initWindow(argc, argv, "Picking + anti-aliasing: ON", winW, winH);
    glEnable(GL_DEPTH_TEST);
    prog = ShaderProgram("shaders/part2.vert", "shaders/part2.frag");
    initGeometry();
    cam.init({0, 0, 0}, 9.0f, 0.0f, 0.25f, GLUT_RIGHT_BUTTON);
    std::printf("Left click: random colour | right-drag / arrows: rotate | z,x: zoom | r: reset | a: anti-aliasing | Esc: quit\n");
    glutDisplayFunc(display); glutReshapeFunc(reshape); glutMouseFunc(mouse);
    glutMotionFunc(motion); glutKeyboardFunc(keyboard); glutSpecialFunc(special);
    glutMainLoop();
}
