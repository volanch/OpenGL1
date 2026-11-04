// Small helpers shared by all programs: math, shaders, vertex buffer, orbit camera, Bezier patch.
#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

// ------------------------------------------------------------------ vectors
struct Vec2 { float x, y; };
struct Vec3 { float x, y, z; };
inline Vec3 operator+(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline Vec3 operator-(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline Vec3 operator*(Vec3 a, float s) { return {a.x * s, a.y * s, a.z * s}; }
inline float dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 cross(Vec3 a, Vec3 b) { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
inline Vec3 normalize(Vec3 a) { float l = std::sqrt(dot(a, a)) + 1e-12f; return a * (1.0f / l); }
inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline void push(std::vector<float>& v, Vec3 a) { v.push_back(a.x); v.push_back(a.y); v.push_back(a.z); }

// ------------------------------------------------------------------ 4x4 matrices (column-major, as OpenGL wants)
struct Mat4 { float m[16]; };
inline Mat4 identity() { Mat4 r = {}; r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1; return r; }
inline Mat4 operator*(Mat4 a, Mat4 b) {
    Mat4 r = {};
    for (int c = 0; c < 4; c++)
        for (int row = 0; row < 4; row++)
            for (int k = 0; k < 4; k++) r.m[c * 4 + row] += a.m[k * 4 + row] * b.m[c * 4 + k];
    return r;
}
inline Mat4 perspective(float fovyDeg, float aspect, float n, float f) {
    float t = 1.0f / std::tan(fovyDeg * 3.14159265f / 360.0f);
    Mat4 r = {};
    r.m[0] = t / aspect; r.m[5] = t; r.m[10] = (f + n) / (n - f); r.m[11] = -1; r.m[14] = 2 * f * n / (n - f);
    return r;
}
inline Mat4 lookAt(Vec3 eye, Vec3 target, Vec3 up) {
    Vec3 f = normalize(target - eye), s = normalize(cross(f, up)), u = cross(s, f);
    Mat4 r = identity();
    r.m[0] = s.x; r.m[4] = s.y; r.m[8] = s.z;
    r.m[1] = u.x; r.m[5] = u.y; r.m[9] = u.z;
    r.m[2] = -f.x; r.m[6] = -f.y; r.m[10] = -f.z;
    r.m[12] = -dot(s, eye); r.m[13] = -dot(u, eye); r.m[14] = dot(f, eye);
    return r;
}
inline Mat4 translate(Vec3 t) { Mat4 r = identity(); r.m[12] = t.x; r.m[13] = t.y; r.m[14] = t.z; return r; }

// ------------------------------------------------------------------ window + shaders
inline void initWindow(int argc, char** argv, const char* title, int w, int h) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitWindowSize(w, h);
    glutCreateWindow(title);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::printf("glewInit failed\n"); std::exit(1); }
    glGetError();                                   // glewInit can leave a harmless error in core profile
}

inline GLuint makeProgram(const char* vsSrc, const char* fsSrc) {
    GLuint prog = glCreateProgram();
    GLenum kinds[2] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
    const char* srcs[2] = {vsSrc, fsSrc};
    for (int i = 0; i < 2; i++) {
        GLuint s = glCreateShader(kinds[i]);
        glShaderSource(s, 1, &srcs[i], nullptr);
        glCompileShader(s);
        GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) { char log[2048]; glGetShaderInfoLog(s, 2048, nullptr, log); std::printf("Shader error:\n%s\n", log); std::exit(1); }
        glAttachShader(prog, s);
    }
    glLinkProgram(prog);
    GLint ok; glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) { char log[2048]; glGetProgramInfoLog(prog, 2048, nullptr, log); std::printf("Link error:\n%s\n", log); std::exit(1); }
    return prog;
}
inline void setMat4(GLuint p, const char* n, Mat4 m) { glUniformMatrix4fv(glGetUniformLocation(p, n), 1, GL_FALSE, m.m); }
inline void setVec3(GLuint p, const char* n, Vec3 v) { glUniform3f(glGetUniformLocation(p, n), v.x, v.y, v.z); }
inline void setFloat(GLuint p, const char* n, float x) { glUniform1f(glGetUniformLocation(p, n), x); }
inline void setInt(GLuint p, const char* n, int x) { glUniform1i(glGetUniformLocation(p, n), x); }

// ------------------------------------------------------------------ vertex buffer: VAO + VBO with interleaved floats
struct VBuf {
    GLuint vao = 0, vbo = 0;
    int stride = 0, count = 0;
    void init(std::vector<int> sizes) {             // e.g. {3, 3} = position + colour
        for (int s : sizes) stride += s;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        int offset = 0;
        for (int i = 0; i < (int)sizes.size(); i++) {
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, sizes[i], GL_FLOAT, GL_FALSE, stride * 4, (void*)(size_t)(offset * 4));
            offset += sizes[i];
        }
    }
    void set(const std::vector<float>& d) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, d.size() * sizeof(float), d.data(), GL_DYNAMIC_DRAW);
        count = (int)d.size() / stride;
    }
    void draw(GLenum mode, int first = 0, int n = -1) {
        glBindVertexArray(vao);
        glDrawArrays(mode, first, n < 0 ? count : n);
    }
};

// ------------------------------------------------------------------ orbit camera (always looks at the target)
struct OrbitCamera {
    Vec3 target = {0, 0, 0};
    float dist = 8, yaw = 0, pitch = 0.35f;
    float dist0 = 8, yaw0 = 0, pitch0 = 0.35f;
    int rotButton = GLUT_LEFT_BUTTON;
    int lastX = 0, lastY = 0;
    bool dragging = false;

    void init(Vec3 t, float d, float y, float p, int button = GLUT_LEFT_BUTTON) {
        target = t; dist = dist0 = d; yaw = yaw0 = y; pitch = pitch0 = p; rotButton = button;
    }
    void reset() { dist = dist0; yaw = yaw0; pitch = pitch0; }
    void rotate(float dyaw, float dpitch) { yaw += dyaw; pitch = clampf(pitch + dpitch, -1.5f, 1.5f); }
    void zoom(float factor) { dist = clampf(dist * factor, 1.0f, 100.0f); }
    Vec3 eye() const { return target + Vec3{std::cos(pitch) * std::sin(yaw), std::sin(pitch), std::cos(pitch) * std::cos(yaw)} * dist; }
    Vec3 up() const { return {0, 1, 0}; }
    Mat4 view() const { return lookAt(eye(), target, up()); }

    void mouse(int button, int state, int x, int y) {
        if (button == rotButton) { dragging = (state == GLUT_DOWN); lastX = x; lastY = y; }
    }
    bool motion(int x, int y) {                     // returns true if the view changed
        if (!dragging) return false;
        rotate(-(x - lastX) * 0.01f, (y - lastY) * 0.01f);
        lastX = x; lastY = y;
        return true;
    }
    bool special(int key) {                         // arrow keys
        float s = 0.08f;
        if (key == GLUT_KEY_LEFT) rotate(-s, 0);
        else if (key == GLUT_KEY_RIGHT) rotate(s, 0);
        else if (key == GLUT_KEY_UP) rotate(0, s);
        else if (key == GLUT_KEY_DOWN) rotate(0, -s);
        else return false;
        return true;
    }
};

// ------------------------------------------------------------------ Bezier patch
// 16 control points, order [0,0],[1,0],[2,0],[3,0],[0,1],... (x index runs fastest)
inline std::vector<Vec3> defaultControlPoints() {
    return {{0, 0, 0}, {1, 0, 1}, {2, 0, 1}, {3, 0, 0},
            {0, 1, 1}, {1, 1, 2}, {2, 1, 2}, {3, 1, 1},
            {0, 2, 1}, {1, 2, 2}, {2, 2, 2}, {3, 2, 1},
            {0, 3, 0}, {1, 3, 1}, {2, 3, 1}, {3, 3, 0}};
}
// Use the course's control_points.txt (16 lines of 3 floats) if it is next to the program.
inline std::vector<Vec3> loadControlPoints(const char* path = "control_points.txt") {
    std::ifstream f(path);
    std::vector<Vec3> p;
    Vec3 v;
    while (f >> v.x >> v.y >> v.z) p.push_back(v);
    if (p.size() == 16) { std::printf("Loaded control points from %s\n", path); return p; }
    return defaultControlPoints();
}

// the four cubic Bernstein polynomials written out (no recursion)
inline void bernstein(float t, float b[4]) {
    float s = 1 - t;
    b[0] = s * s * s; b[1] = 3 * t * s * s; b[2] = 3 * t * t * s; b[3] = t * t * t;
}

// Iterative evaluation on an (n+1)x(n+1) grid: V[i*(n+1)+j] = P(u=i/n, v=j/n)
inline std::vector<Vec3> bezierGrid(const std::vector<Vec3>& ctrl, int n) {
    std::vector<Vec3> V((n + 1) * (n + 1));
    for (int i = 0; i <= n; i++) {
        float bu[4]; bernstein((float)i / n, bu);
        for (int j = 0; j <= n; j++) {
            float bv[4]; bernstein((float)j / n, bv);
            Vec3 p = {0, 0, 0};
            for (int r = 0; r < 4; r++)             // sum over the 4x4 control points
                for (int c = 0; c < 4; c++)
                    p = p + ctrl[r * 4 + c] * (bv[r] * bu[c]);
            V[i * (n + 1) + j] = p;
        }
    }
    return V;
}

// Grid of (n+1)x(nv+1) samples -> triangle list (2 triangles per quad, 6 entries per quad)
template <class T>
std::vector<T> quadsToTriangles(const std::vector<T>& G, int nu, int nv) {
    std::vector<T> out;
    for (int i = 0; i < nu; i++)
        for (int j = 0; j < nv; j++) {
            T a = G[i * (nv + 1) + j], b = G[(i + 1) * (nv + 1) + j];
            T c = G[(i + 1) * (nv + 1) + j + 1], d = G[i * (nv + 1) + j + 1];
            out.push_back(a); out.push_back(b); out.push_back(c);
            out.push_back(a); out.push_back(c); out.push_back(d);
        }
    return out;
}
