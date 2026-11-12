#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "BezierPatch.h"
#include "Mesh.h"
#include "OrbitCamera.h"
#include "ShaderProgram.h"
#include "VertexBuffer.h"
#include "Window.h"

namespace {
    struct State {
        std::vector<Vec3> ctrl;
        int res = 10;
        int sel = 0;
        Vec3 baseColor = {0.9f, 0.55f, 0.2f};
        int winW = 800, winH = 600;
        std::vector<Vec3> tri;
        std::vector<Vec3> normals;
    } S;

    OrbitCamera cam;
    ShaderProgram prog;
    VertexBuffer patchVB, pointsVB, axesVB;

    void rebuild() {
        S.tri = quadsToTriangles(bezierGrid(S.ctrl, S.res), S.res, S.res);
        S.normals.resize(S.tri.size() / 3);
        for (size_t t = 0; t < S.normals.size(); t++)
            S.normals[t] = normalize(cross(S.tri[3 * t + 1] - S.tri[3 * t], S.tri[3 * t + 2] - S.tri[3 * t]));
    }

    void updateTitle() {
        char t[100];
        std::snprintf(t, 100, "Bezier patch | selected point %d | resolution %dx%d", S.sel, S.res, S.res);
        glutSetWindowTitle(t);
    }

    void drawPatch() {
        Vec3 eye = cam.eye(), light = eye + cam.up() * 0.5f;
        std::vector<float> data;
        for (size_t t = 0; t < S.normals.size(); t++) {
            Vec3 centre = (S.tri[3 * t] + S.tri[3 * t + 1] + S.tri[3 * t + 2]) * (1.0f / 3);
            Vec3 N = S.normals[t], L = normalize(light - centre);
            if (dot(N, eye - centre) < 0) N = N * -1.0f;       // face the viewer
            Vec3 colour = S.baseColor * (clampf(dot(N, L), 0, 1) + 0.08f);   // + tiny ambient
            for (int k = 0; k < 3; k++) { push(data, S.tri[3 * t + k]); push(data, colour); }
        }
        patchVB.set(data);
        prog.setFloat("pointSize", 1);
        patchVB.draw(GL_TRIANGLES);
    }

    void drawControlPoints() {
        std::vector<float> pd;
        for (int i = 0; i < 16; i++) { push(pd, S.ctrl[i]); push(pd, i == S.sel ? Vec3{1, 1, 0} : Vec3{1, 1, 1}); }
        pointsVB.set(pd);
        prog.setFloat("pointSize", 8);
        pointsVB.draw(GL_POINTS);
        prog.setFloat("pointSize", 16);
        pointsVB.draw(GL_POINTS, S.sel, 1);
    }

    void display() {
        glClearColor(0.1f, 0.1f, 0.13f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        prog.use();
        prog.setMat4("mvp", perspective(45, (float)S.winW / S.winH, 0.1f, 100) * cam.view());
        drawPatch();
        drawControlPoints();
        prog.setFloat("pointSize", 1);
        axesVB.draw(GL_LINES);
        glutSwapBuffers();
    }

    void reshape(int w, int h) {
        S.winW = w; S.winH = h > 0 ? h : 1;
        glViewport(0, 0, w, h);
    }

    void keyboard(unsigned char key, int, int) {
        key = (unsigned char)std::tolower(key);
        const float step = 0.1f;
        Vec3& p = S.ctrl[S.sel];
        switch (key) {
            case 'a': p.x -= step; rebuild(); break;
            case 'd': p.x += step; rebuild(); break;
            case 's': p.y -= step; rebuild(); break;
            case 'w': p.y += step; rebuild(); break;
            case 'q': p.z -= step; rebuild(); break;
            case 'e': p.z += step; rebuild(); break;
            case 'n': S.sel = (S.sel + 1) % 16; break;
            case 'b': S.sel = (S.sel + 15) % 16; break;
            case '+': case '=': if (S.res < 60) S.res++; rebuild(); break;
            case '-': case '_': if (S.res > 1) S.res--; rebuild(); break;
            case 'z': cam.zoom(0.9f); break;
            case 'x': cam.zoom(1.1f); break;
            case 'r': cam.reset(); break;                                   // reset view only
            case 'c': S.ctrl = loadControlPoints(); rebuild(); break;       // reset control points
            case 27: std::exit(0);
        }
        updateTitle();
        glutPostRedisplay();
    }

    void special(int key, int, int) { if (cam.special(key)) glutPostRedisplay(); }
    void mouse(int b, int s, int x, int y) { cam.mouse(b, s, x, y); }
    void motion(int x, int y) { if (cam.motion(x, y)) glutPostRedisplay(); }

    void initGeometry() {
        patchVB.init({3, 3});
        pointsVB.init({3, 3});
        axesVB.init({3, 3});
        axesVB.set({0, 0, 0, 1, 0, 0,   1, 0, 0, 1, 0, 0,    // X red
                    0, 0, 0, 0, 1, 0,   0, 1, 0, 0, 1, 0,    // Y green
                    0, 0, 0, 0, 0, 1,   0, 0, 1, 0, 0, 1});  // Z blue
        S.ctrl = loadControlPoints();
        Vec3 centre = {0, 0, 0};
        for (Vec3 p : S.ctrl) centre = centre + p * (1.0f / 16);
        cam.init(centre, 9.0f, 0.0f, 0.4f);
        rebuild();
    }
}

int main(int argc, char** argv) {
    initWindow(argc, argv, "Bezier patch", S.winW, S.winH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    prog = ShaderProgram("shaders/part1.vert", "shaders/part1.frag");
    initGeometry();
    updateTitle();
    std::printf("Mouse drag / arrows: rotate | z,x: zoom | r: reset view | c: reset points | n,b: select point\n"
                "a,d: -/+ x | s,w: -/+ y | q,e: -/+ z | + / - : resolution | Esc: quit\n");
    glutDisplayFunc(display); glutReshapeFunc(reshape); glutKeyboardFunc(keyboard);
    glutSpecialFunc(special); glutMouseFunc(mouse); glutMotionFunc(motion);
    glutMainLoop();
}
