#include <glad/glad.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <vector>

#include "shaderClass.h"
#include "mesh.h"

namespace {

struct NamedColor { const char* name; glm::vec3 rgb; };
struct KeyColor   { char key; glm::vec3 rgb; };

const NamedColor kSquareColors[] = {
    {"White", {1.00f, 1.00f, 1.00f}}, {"Red", {0.95f, 0.15f, 0.15f}}, {"Green", {0.15f, 0.85f, 0.25f}}};

const NamedColor kSubBgColors[] = {
    {"Blue",   {0.12f, 0.28f, 0.75f}}, {"Green", {0.15f, 0.60f, 0.25f}}, {"Purple", {0.50f, 0.15f, 0.65f}},
    {"Yellow", {0.85f, 0.80f, 0.20f}}, {"Gray",  {0.40f, 0.40f, 0.45f}}, {"Black",  {0.02f, 0.02f, 0.02f}}};

const KeyColor kWin2Keys[] = {
    {'r', {1.0f, 0.0f, 0.0f}}, {'g', {0.0f, 1.0f, 0.0f}}, {'b', {0.0f, 0.0f, 1.0f}},
    {'y', {1.0f, 1.0f, 0.0f}}, {'o', {1.0f, 0.5f, 0.0f}}, {'p', {0.6f, 0.1f, 0.85f}},
    {'w', {1.0f, 1.0f, 1.0f}}};

enum AnimMenu { kStop = 1, kStart = 2 };
constexpr int kSegs = 64, kRings = 6;

// each GLUT window has its own GL context, so each needs its own program + VAOs.
struct MainScene {
    Shader shader{"shaders/flat2d.vert", "shaders/flat2d.frag"};
    std::vector<Mesh> squares;
    Mesh circle{Ellipse(1.0f, 1.0f, kSegs), GL_TRIANGLE_FAN};   // unit circle, scaled when drawn
};
struct SubScene {
    Shader shader{"shaders/flat2d.vert", "shaders/flat2d.frag"};
    Mesh ellipse{Ellipse(0.72f, 0.45f, kSegs), GL_TRIANGLE_FAN};
};
struct Win2Scene {
    Shader shader{"shaders/flat2d.vert", "shaders/flat2d.frag"};
    Mesh circle{Ellipse(0.28f, 0.28f, kSegs), GL_TRIANGLE_FAN};
    Mesh triangle{Triangle(0.32f), GL_TRIANGLES};
};

std::unique_ptr<MainScene> g_main;
std::unique_ptr<SubScene>  g_sub;
std::unique_ptr<Win2Scene> g_win2;

// application state
struct Bubble { float x, y, radius; glm::vec3 color; float phase; };

struct State {
    int mainWin = 0, subWin = 0, win2 = 0;
    int lastMs = 0;
    float time = 0, squareAngle = 0, triAngle = 0;
    glm::vec3 squareColor{1.0f};
    glm::vec3 subBg{0.18f, 0.24f, 0.36f};
    glm::vec3 win2Color{1.0f, 0.0f, 0.0f};
    std::vector<Bubble> bubbles;
} S;

float Rand01() { return rand() / (float)RAND_MAX; }

void Draw(Shader& sh, Mesh& mesh, const glm::mat4& m, const glm::vec3& color) {
    sh.SetMat4("uTransform", m);
    sh.SetVec3("uColor", color);
    mesh.Draw();
}

// animation
void Idle() {
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = (now - S.lastMs) * 0.001f; // frame-rate independent
    S.lastMs = now;

    S.time += dt;
    S.squareAngle = fmodf(S.squareAngle + 36.0f * dt, 360.0f);   // CCW
    S.triAngle = fmodf(S.triAngle    - 60.0f * dt, 360.0f);   // CW

    glutPostWindowRedisplay(S.mainWin);
    glutPostWindowRedisplay(S.subWin);
    glutPostWindowRedisplay(S.win2);
}

// display callbacks
void DisplayMain() {
    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    g_main->shader.Activate();

    glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(S.squareAngle), {0, 0, 1});
    for (int i = 0; i < kRings; ++i)
        Draw(g_main->shader, g_main->squares[i], rot, i % 2 == 0 ? S.squareColor : glm::vec3(0.0f));

    for (const Bubble& b : S.bubbles) {
        float s = b.radius * (1.0f + 0.35f * sinf(S.time * 3.5f + b.phase));
        glm::mat4 m = glm::scale(glm::translate(glm::mat4(1.0f), {b.x, b.y, 0.0f}), {s, s, 1.0f});
        Draw(g_main->shader, g_main->circle, m, b.color);
    }
    glutSwapBuffers();
}

void DisplaySub() {
    glClearColor(S.subBg.r, S.subBg.g, S.subBg.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    g_sub->shader.Activate();
    Draw(g_sub->shader, g_sub->ellipse, glm::mat4(1.0f), {0.95f, 0.15f, 0.15f});
    glutSwapBuffers();
}

void DisplayWin2() {
    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    g_win2->shader.Activate();

    float s = 1.0f + 0.35f * sinf(S.time * 2.8f); // breathing
    glm::mat4 c = glm::scale(glm::translate(glm::mat4(1.0f), {-0.45f, 0.0f, 0.0f}), {s, s, 1.0f});
    Draw(g_win2->shader, g_win2->circle, c, S.win2Color);

    glm::mat4 t = glm::rotate(glm::translate(glm::mat4(1.0f), {0.45f, 0.0f, 0.0f}),
                              glm::radians(S.triAngle), {0, 0, 1});
    Draw(g_win2->shader, g_win2->triangle, t, S.win2Color);
    glutSwapBuffers();
}

// input callbacks
void MouseMain(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
    float nx = 2.0f * x / glutGet(GLUT_WINDOW_WIDTH) - 1.0f;
    float ny = 1.0f - 2.0f * y / glutGet(GLUT_WINDOW_HEIGHT);
    glm::vec3 color(0.25f + 0.75f * Rand01(), 0.25f + 0.75f * Rand01(), 0.25f + 0.75f * Rand01());
    S.bubbles.push_back({nx, ny, 0.08f, color, Rand01() * 2.0f * kPi});
    glutPostRedisplay();
}

void KeyMain(unsigned char key, int, int) {
    if (key == 27 || key == 'q' || key == 'Q') glutLeaveMainLoop();
}

void KeyWin2(unsigned char key, int, int) {
    if (key == 27) { glutLeaveMainLoop(); return; }
    for (const KeyColor& k : kWin2Keys)
        if (k.key == std::tolower(key)) { S.win2Color = k.rgb; glutPostRedisplay(); return; }
}

// menu callbacks
void OnSquareColor(int i) { S.squareColor = kSquareColors[i].rgb; glutPostWindowRedisplay(S.mainWin); }
void OnSubBg(int i) { S.subBg = kSubBgColors[i].rgb;  glutPostWindowRedisplay(S.subWin); }

void OnMainMenu(int option) {
    if (option == kStop) {
        glutIdleFunc(nullptr);
    } else if (option == kStart) {
        S.lastMs = glutGet(GLUT_ELAPSED_TIME); // avoid a time jump after a pause
        glutIdleFunc(Idle);
    }
}

template <size_t N>
int MakeColorMenu(void (*cb)(int), const NamedColor (&colors)[N]) {
    int id = glutCreateMenu(cb);
    for (size_t i = 0; i < N; ++i) glutAddMenuEntry(colors[i].name, (int)i);
    return id;
}

void PrintInstructions() {
    std::cout << " Main window: right-click = stop/start, square colors; left-click = spawn breathing circle\n"
              << " Sub-window: right-click = change background color\n"
              << " window 2: r g b y o p w = change shape color\n"
              << " ESC: quit\n";
}

}

int main(int argc, char** argv) {
    srand((unsigned)time(nullptr));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    // the newly created window is current, so GL objects below belong to its context.

    // main window
    glutInitWindowSize(550, 550);
    glutInitWindowPosition(100, 100);
    S.mainWin = glutCreateWindow("Task 2 Part 1 - Main Window");
    if (!gladLoadGL()) { std::cerr << "Failed to initialize GLAD\n"; return -1; }
    glutDisplayFunc(DisplayMain);
    glutMouseFunc(MouseMain);
    glutKeyboardFunc(KeyMain);

    g_main = std::make_unique<MainScene>();
    for (int i = 0; i < kRings; ++i)
        g_main->squares.emplace_back(Square(0.5f - i * 0.075f), GL_TRIANGLE_FAN);

    int colorSub = MakeColorMenu(OnSquareColor, kSquareColors);
    glutCreateMenu(OnMainMenu);
    glutAddMenuEntry("Stop Animation", kStop);
    glutAddMenuEntry("Start Animation", kStart);
    glutAddSubMenu("Square Colors ->", colorSub);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // sub window
    S.subWin = glutCreateSubWindow(S.mainWin, 15, 15, 170, 130);
    glutDisplayFunc(DisplaySub);
    g_sub = std::make_unique<SubScene>();
    MakeColorMenu(OnSubBg, kSubBgColors);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // window 2
    glutInitWindowSize(550, 550);
    glutInitWindowPosition(680, 100);
    S.win2 = glutCreateWindow("window 2");
    glutDisplayFunc(DisplayWin2);
    glutKeyboardFunc(KeyWin2);
    g_win2 = std::make_unique<Win2Scene>();

    S.lastMs = glutGet(GLUT_ELAPSED_TIME);
    glutIdleFunc(Idle);
    PrintInstructions();
    glutMainLoop();
    return 0;
}