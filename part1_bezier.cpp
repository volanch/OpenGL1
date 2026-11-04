// Part 1 - Flat-shaded bicubic Bezier patch with interactive control points.
#include "common.h"
#include <cctype>

static const char* VS = R"GLSL(#version 330 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 col;
uniform mat4 mvp;
uniform float pointSize;
out vec3 vcol;
void main() { gl_Position = mvp * vec4(pos, 1.0); gl_PointSize = pointSize; vcol = col; }
)GLSL";
static const char* FS = R"GLSL(#version 330 core
in vec3 vcol;
out vec4 color;
void main() { color = vec4(vcol, 1.0); }
)GLSL";

std::vector<Vec3> ctrl;                 // 16 control points, edited by the user
int res = 10;                           // patch has res x res quads
int sel = 0;                            // selected control point
Vec3 baseColor = {0.9f, 0.55f, 0.2f};   // surface (diffuse) colour kd
OrbitCamera cam;
int winW = 800, winH = 600;
GLuint prog;
VBuf patch, points, axes;
std::vector<Vec3> tri;                  // 3 vertices per triangle
std::vector<Vec3> normals;              // 1 normal per triangle

// Re-evaluate the patch (only when a control point or the resolution changes)
void rebuild() {
    tri = quadsToTriangles(bezierGrid(ctrl, res), res, res);
    normals.resize(tri.size() / 3);
    for (size_t t = 0; t < normals.size(); t++)
        normals[t] = normalize(cross(tri[3 * t + 1] - tri[3 * t], tri[3 * t + 2] - tri[3 * t]));
}

void display() {
    glClearColor(0.1f, 0.1f, 0.13f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(prog);
    setMat4(prog, "mvp", perspective(45, (float)winW / winH, 0.1f, 100) * cam.view());

    // Diffuse term of Phong per triangle: kd * max(N.L, 0). Light sits next to the camera.
    Vec3 eye = cam.eye(), light = eye + cam.up() * 0.5f;
    std::vector<float> data;
    for (size_t t = 0; t < normals.size(); t++) {
        Vec3 centre = (tri[3 * t] + tri[3 * t + 1] + tri[3 * t + 2]) * (1.0f / 3);
        Vec3 N = normals[t], L = normalize(light - centre);
        if (dot(N, eye - centre) < 0) N = N * -1.0f;             // face the viewer
        float d = clampf(dot(N, L), 0, 1);
        Vec3 colour = baseColor * (d + 0.08f);                   // + tiny ambient
        for (int k = 0; k < 3; k++) { push(data, tri[3 * t + k]); push(data, colour); }
    }
    patch.set(data);
    setFloat(prog, "pointSize", 1);
    patch.draw(GL_TRIANGLES);

    // control points (selected one yellow and bigger)
    std::vector<float> pd;
    for (int i = 0; i < 16; i++) { push(pd, ctrl[i]); push(pd, i == sel ? Vec3{1, 1, 0} : Vec3{1, 1, 1}); }
    points.set(pd);
    setFloat(prog, "pointSize", 8);
    points.draw(GL_POINTS);
    setFloat(prog, "pointSize", 16);
    points.draw(GL_POINTS, sel, 1);

    setFloat(prog, "pointSize", 1);
    axes.draw(GL_LINES);                                         // axes at the patch origin
    glutSwapBuffers();
}

void reshape(int w, int h) { winW = w; winH = h > 0 ? h : 1; glViewport(0, 0, w, h); }

void updateTitle() {
    char t[100];
    std::snprintf(t, 100, "Bezier patch | selected point %d | resolution %dx%d", sel, res, res);
    glutSetWindowTitle(t);
}

void keyboard(unsigned char key, int, int) {
    key = (unsigned char)std::tolower(key);
    float step = 0.1f;
    switch (key) {
        case 'a': ctrl[sel].x -= step; rebuild(); break;         // move selected point
        case 'd': ctrl[sel].x += step; rebuild(); break;
        case 's': ctrl[sel].y -= step; rebuild(); break;
        case 'w': ctrl[sel].y += step; rebuild(); break;
        case 'q': ctrl[sel].z -= step; rebuild(); break;
        case 'e': ctrl[sel].z += step; rebuild(); break;
        case 'n': sel = (sel + 1) % 16; break;                   // select next / previous point
        case 'b': sel = (sel + 15) % 16; break;
        case '+': case '=': if (res < 60) res++; rebuild(); break;
        case '-': case '_': if (res > 1) res--; rebuild(); break;
        case 'z': cam.zoom(0.9f); break;
        case 'x': cam.zoom(1.1f); break;
        case 'r': cam.reset(); ctrl = loadControlPoints(); rebuild(); break;   // reset view
        case 27: std::exit(0);
    }
    updateTitle();
    glutPostRedisplay();
}
void special(int key, int, int) { if (cam.special(key)) glutPostRedisplay(); }
void mouse(int b, int s, int x, int y) { cam.mouse(b, s, x, y); }
void motion(int x, int y) { if (cam.motion(x, y)) glutPostRedisplay(); }

int main(int argc, char** argv) {
    initWindow(argc, argv, "Bezier patch", winW, winH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    prog = makeProgram(VS, FS);
    patch.init({3, 3}); points.init({3, 3}); axes.init({3, 3});
    axes.set({0, 0, 0, 1, 0, 0,   1, 0, 0, 1, 0, 0,             // X red
              0, 0, 0, 0, 1, 0,   0, 1, 0, 0, 1, 0,             // Y green
              0, 0, 0, 0, 0, 1,   0, 0, 1, 0, 0, 1});           // Z blue
    ctrl = loadControlPoints();
    Vec3 centre = {0, 0, 0};
    for (Vec3 p : ctrl) centre = centre + p * (1.0f / 16);
    cam.init(centre, 9.0f, 0.0f, 0.4f);
    rebuild();
    updateTitle();
    std::printf("Mouse drag / arrows: rotate | z,x: zoom | r: reset | n,b: select point\n"
                "a,d: -/+ x | s,w: -/+ y | q,e: -/+ z | + / - : resolution | Esc: quit\n");
    glutDisplayFunc(display); glutReshapeFunc(reshape); glutKeyboardFunc(keyboard);
    glutSpecialFunc(special); glutMouseFunc(mouse); glutMotionFunc(motion);
    glutMainLoop();
}
