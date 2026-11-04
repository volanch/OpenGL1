// Part 2 - Smooth-shaded meshes, colour-ID picking through an FBO, and anti-aliasing (multisampled FBO).
#include "common.h"
#include <cctype>
#include <ctime>

static const char* VS = R"GLSL(#version 330 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 nrm;
uniform mat4 model, view, proj;
out vec3 ePos, eNrm;                          // eye (camera) space position and normal
void main() {
    vec4 p = view * model * vec4(pos, 1.0);
    ePos = p.xyz;
    eNrm = mat3(view * model) * nrm;          // models only translate, so this is fine
    gl_Position = proj * p;
}
)GLSL";
static const char* FS = R"GLSL(#version 330 core
in vec3 ePos, eNrm;
uniform vec3 kd;                              // diffuse colour
uniform bool pickMode;                        // true: output a constant id colour, no shading
uniform vec3 pickColor;
out vec4 color;
void main() {
    if (pickMode) { color = vec4(pickColor, 1.0); return; }
    vec3 lightPos = vec3(0.0, 3.0, 2.0);      // static light, defined in camera coordinates
    vec3 N = normalize(eNrm), L = normalize(lightPos - ePos), V = normalize(-ePos);
    vec3 R = reflect(-L, N);
    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(R, V), 0.0), 40.0);
    color = vec4(0.12 * kd + diff * kd + 0.3 * spec, 1.0);
}
)GLSL";

const float PI = 3.14159265f;

// ---- three parametric surfaces: position and (analytic) normal for u, v in [0,1]
Vec3 spherePos(float u, float v) { float t = 2 * PI * u, p = PI * v; return {std::sin(p) * std::cos(t), std::cos(p), std::sin(p) * std::sin(t)}; }
Vec3 torusPos(float u, float v) {
    float R = 0.75f, r = 0.3f, t = 2 * PI * u, p = 2 * PI * v;
    return {(R + r * std::cos(p)) * std::cos(t), r * std::sin(p), (R + r * std::cos(p)) * std::sin(t)};
}
Vec3 torusNrm(float u, float v) { float t = 2 * PI * u, p = 2 * PI * v; return {std::cos(p) * std::cos(t), std::sin(p), std::cos(p) * std::sin(t)}; }
Vec3 conePos(float u, float v) { float r = 0.9f, h = 2.0f, t = 2 * PI * u, rad = (1 - v) * r; return {rad * std::cos(t), v * h - h / 2, rad * std::sin(t)}; }
Vec3 coneNormal(float u, float) { float r = 0.9f, h = 2.0f, t = 2 * PI * u; return normalize({h * std::cos(t), r, h * std::sin(t)}); }

typedef Vec3 (*SurfFn)(float, float);

// sample the surface on an NxN grid, make triangles, interleave position + normal
std::vector<float> makeMesh(SurfFn pos, SurfFn nrm, int N = 48) {
    std::vector<Vec3> P, Nr;
    for (int i = 0; i <= N; i++)
        for (int j = 0; j <= N; j++) { P.push_back(pos((float)i / N, (float)j / N)); Nr.push_back(nrm((float)i / N, (float)j / N)); }
    std::vector<Vec3> tp = quadsToTriangles(P, N, N), tn = quadsToTriangles(Nr, N, N);
    std::vector<float> data;
    for (size_t k = 0; k < tp.size(); k++) { push(data, tp[k]); push(data, tn[k]); }
    return data;
}

const int NOBJ = 3;
VBuf objects[NOBJ];
Vec3 offsets[NOBJ] = {{-2.6f, 0, 0}, {0, 0, 0}, {2.6f, 0, 0}};
Vec3 colors[NOBJ] = {{0.9f, 0.2f, 0.2f}, {0.2f, 0.8f, 0.3f}, {0.2f, 0.4f, 0.9f}};
OrbitCamera cam;
int winW = 800, winH = 600;
bool antialias = true;
GLuint prog;

// ---- off-screen frame buffer objects
struct Fbo { GLuint fbo = 0, rb[2] = {0, 0}; };
Fbo pickFbo, msFbo;

Fbo makeFbo(int w, int h, int samples) {
    Fbo f;
    glGenFramebuffers(1, &f.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, f.fbo);
    glGenRenderbuffers(2, f.rb);
    GLenum fmt[2] = {GL_RGBA8, GL_DEPTH_COMPONENT24};
    GLenum attach[2] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
    for (int i = 0; i < 2; i++) {
        glBindRenderbuffer(GL_RENDERBUFFER, f.rb[i]);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, fmt[i], w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attach[i], GL_RENDERBUFFER, f.rb[i]);
    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) std::printf("FBO incomplete\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return f;
}
void deleteFbo(Fbo& f) {
    if (f.fbo) { glDeleteFramebuffers(1, &f.fbo); glDeleteRenderbuffers(2, f.rb); }
    f = Fbo();
}

void drawScene(bool pick) {
    if (pick) glClearColor(0, 0, 0, 1); else glClearColor(0.12f, 0.12f, 0.15f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(prog);
    setMat4(prog, "view", cam.view());
    setMat4(prog, "proj", perspective(45, (float)winW / winH, 0.1f, 100));
    setInt(prog, "pickMode", pick ? 1 : 0);
    for (int i = 0; i < NOBJ; i++) {
        setMat4(prog, "model", translate(offsets[i]));
        setVec3(prog, "kd", colors[i]);
        float id = (i + 1) / 255.0f;
        setVec3(prog, "pickColor", {id, id, id});            // unique constant colour per object
        objects[i].draw(GL_TRIANGLES);
    }
}

// Render ids into the FBO, read the pixel under the mouse. Returns object index, -1 = background.
int pickObject(int x, int y) {
    glBindFramebuffer(GL_FRAMEBUFFER, pickFbo.fbo);          // single-sample: id colours never blend
    glViewport(0, 0, winW, winH);
    drawScene(true);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    unsigned char px[3] = {0, 0, 0};
    glReadPixels(x, winH - 1 - y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, px);   // window y is flipped
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return (int)px[0] - 1;
}

void display() {
    if (antialias) {      // draw into the 4x multisampled FBO, then resolve (blit) to the screen
        glBindFramebuffer(GL_FRAMEBUFFER, msFbo.fbo);
        drawScene(false);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, msFbo.fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, winW, winH, 0, 0, winW, winH, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    } else {              // draw directly into the GLUT frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        drawScene(false);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glutSwapBuffers();
}

void reshape(int w, int h) {
    winW = w; winH = h > 0 ? h : 1;
    glViewport(0, 0, winW, winH);
    deleteFbo(pickFbo); deleteFbo(msFbo);
    pickFbo = makeFbo(winW, winH, 0);
    msFbo = makeFbo(winW, winH, 4);
}

float frand() { return rand() / (float)RAND_MAX; }

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
    } else if (key == 'r') cam.reset();
    else if (key == 'z') cam.zoom(0.9f);
    else if (key == 'x') cam.zoom(1.1f);
    else if (key == 27) std::exit(0);
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    srand((unsigned)time(nullptr));
    initWindow(argc, argv, "Picking + anti-aliasing: ON", winW, winH);
    glEnable(GL_DEPTH_TEST);
    prog = makeProgram(VS, FS);
    SurfFn pos[NOBJ] = {spherePos, torusPos, conePos};
    SurfFn nrm[NOBJ] = {spherePos, torusNrm, coneNormal};    // sphere normal = position
    for (int i = 0; i < NOBJ; i++) { objects[i].init({3, 3}); objects[i].set(makeMesh(pos[i], nrm[i])); }
    cam.init({0, 0, 0}, 9.0f, 0.0f, 0.25f, GLUT_RIGHT_BUTTON);
    std::printf("Left click: random colour | right-drag / arrows: rotate | z,x: zoom | r: reset | a: anti-aliasing | Esc: quit\n");
    glutDisplayFunc(display); glutReshapeFunc(reshape); glutMouseFunc(mouse);
    glutMotionFunc(motion); glutKeyboardFunc(keyboard); glutSpecialFunc(special);
    glutMainLoop();
}
