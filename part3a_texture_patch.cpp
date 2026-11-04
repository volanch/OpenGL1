// Part 3a - Procedural 2D texture mapped over a 12x12 Bezier patch, Phong shading in the fragment shader.
#include "common.h"

static const char* VS = R"GLSL(#version 330 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 nrm;
layout(location=2) in vec2 uv;
uniform mat4 view, proj;
out vec3 ePos, eNrm;
out vec2 vUV;
void main() {
    vec4 p = view * vec4(pos, 1.0);
    ePos = p.xyz;
    eNrm = mat3(view) * nrm;
    vUV = uv;
    gl_Position = proj * p;
}
)GLSL";
static const char* FS = R"GLSL(#version 330 core
in vec3 ePos, eNrm;
in vec2 vUV;
uniform sampler2D tex;
out vec4 color;
void main() {
    vec3 kd = texture(tex, vUV).rgb;               // texture replaces the constant diffuse colour
    vec3 lightPos = vec3(2.0, 3.0, 3.0);           // light in camera coordinates
    vec3 N = normalize(eNrm), L = normalize(lightPos - ePos), V = normalize(-ePos);
    if (dot(N, V) < 0.0) N = -N;                   // two-sided surface
    vec3 R = reflect(-L, N);
    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(R, V), 0.0), 60.0);
    color = vec4(0.15 * kd + diff * kd + 0.5 * spec, 1.0);
}
)GLSL";

OrbitCamera cam;
int winW = 800, winH = 600;
GLuint prog;
VBuf patch;

// Own procedural image: swirling interference pattern mapped to a colour palette.
std::vector<unsigned char> makeTexture(int size) {
    std::vector<unsigned char> img(size * size * 3);
    for (int row = 0; row < size; row++)
        for (int col = 0; col < size; col++) {
            float x = (float)col / (size - 1), y = (float)row / (size - 1);
            float swirl = std::sin(10 * x + 3 * std::sin(8 * y)) + std::sin(10 * y + 3 * std::cos(7 * x));
            float ring = std::sin(25 * std::hypot(x - 0.5f, y - 0.5f));
            float t = 0.5f + 0.25f * swirl + 0.25f * ring;
            float shift[3] = {0.0f, 0.33f, 0.66f};
            for (int k = 0; k < 3; k++)
                img[(row * size + col) * 3 + k] = (unsigned char)(255 * (0.5f + 0.5f * std::sin(6.28f * (t + shift[k]))));
        }
    return img;
}

// 12x12 quads -> 288 triangles. Texture coordinate of a vertex = its (u, v).
std::vector<float> buildPatch(int n) {
    std::vector<Vec3> V = bezierGrid(defaultControlPoints(), n), N((n + 1) * (n + 1));
    std::vector<Vec2> UV((n + 1) * (n + 1));
    for (int i = 0; i <= n; i++)
        for (int j = 0; j <= n; j++) {
            int i0 = i > 0 ? i - 1 : 0, i1 = i < n ? i + 1 : n, j0 = j > 0 ? j - 1 : 0, j1 = j < n ? j + 1 : n;
            Vec3 du = V[i1 * (n + 1) + j] - V[i0 * (n + 1) + j];     // tangent along u
            Vec3 dv = V[i * (n + 1) + j1] - V[i * (n + 1) + j0];     // tangent along v
            N[i * (n + 1) + j] = normalize(cross(du, dv));
            UV[i * (n + 1) + j] = {(float)i / n, (float)j / n};
        }
    std::vector<Vec3> tp = quadsToTriangles(V, n, n), tn = quadsToTriangles(N, n, n);
    std::vector<Vec2> tu = quadsToTriangles(UV, n, n);
    std::vector<float> data;
    for (size_t k = 0; k < tp.size(); k++) { push(data, tp[k]); push(data, tn[k]); data.push_back(tu[k].x); data.push_back(tu[k].y); }
    return data;
}

void display() {
    glClearColor(0.1f, 0.1f, 0.13f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(prog);
    setMat4(prog, "view", cam.view());
    setMat4(prog, "proj", perspective(45, (float)winW / winH, 0.1f, 100));
    patch.draw(GL_TRIANGLES);
    glutSwapBuffers();
}
void reshape(int w, int h) { winW = w; winH = h > 0 ? h : 1; glViewport(0, 0, w, h); }
void keyboard(unsigned char key, int, int) {
    if (key == 'r') cam.reset();
    else if (key == 'z') cam.zoom(0.9f);
    else if (key == 'x') cam.zoom(1.1f);
    else if (key == 27) std::exit(0);
    glutPostRedisplay();
}
void special(int key, int, int) { if (cam.special(key)) glutPostRedisplay(); }
void mouse(int b, int s, int x, int y) { cam.mouse(b, s, x, y); }
void motion(int x, int y) { if (cam.motion(x, y)) glutPostRedisplay(); }

int main(int argc, char** argv) {
    initWindow(argc, argv, "Textured Bezier patch", winW, winH);
    glEnable(GL_DEPTH_TEST);
    prog = makeProgram(VS, FS);
    patch.init({3, 3, 2});
    patch.set(buildPatch(12));

    std::vector<unsigned char> img = makeTexture(256);
    GLuint tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glUseProgram(prog);
    setInt(prog, "tex", 0);

    cam.init({1.5f, 1.5f, 1.0f}, 7.0f, 0.3f, 0.5f);
    std::printf("Mouse drag / arrows: rotate | z,x: zoom | r: reset | Esc: quit\n");
    glutDisplayFunc(display); glutReshapeFunc(reshape); glutKeyboardFunc(keyboard);
    glutSpecialFunc(special); glutMouseFunc(mouse); glutMotionFunc(motion);
    glutMainLoop();
}
