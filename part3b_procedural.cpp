// Part 3b - 3D procedural (veined marble) texture evaluated from world coordinates in the fragment shader.
#include "common.h"
#include <sstream>
#include <string>

static const char* VS = R"GLSL(#version 330 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 nrm;
uniform mat4 view, proj;
out vec3 wPos, ePos, eNrm;                 // world position (for the texture), eye-space position/normal
void main() {
    wPos = pos;                            // the model matrix is the identity, so object = world
    vec4 p = view * vec4(pos, 1.0);
    ePos = p.xyz;
    eNrm = mat3(view) * nrm;
    gl_Position = proj * p;
}
)GLSL";
static const char* FS = R"GLSL(#version 330 core
in vec3 wPos, ePos, eNrm;
out vec4 color;

float hash(vec3 p) { return fract(sin(dot(p, vec3(127.1, 311.7, 74.7))) * 43758.5453); }

float noise(vec3 p) {                      // smooth value noise: trilinear blend of random lattice values
    vec3 i = floor(p), f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    float x00 = mix(hash(i),                   hash(i + vec3(1, 0, 0)), f.x);
    float x10 = mix(hash(i + vec3(0, 1, 0)),   hash(i + vec3(1, 1, 0)), f.x);
    float x01 = mix(hash(i + vec3(0, 0, 1)),   hash(i + vec3(1, 0, 1)), f.x);
    float x11 = mix(hash(i + vec3(0, 1, 1)),   hash(i + vec3(1, 1, 1)), f.x);
    return mix(mix(x00, x10, f.y), mix(x01, x11, f.y), f.z);
}

float fbm(vec3 p) {                        // 5 octaves of noise
    float sum = 0.0, amp = 0.5;
    for (int k = 0; k < 5; k++) { sum += amp * noise(p); p *= 2.03; amp *= 0.5; }
    return sum;
}

vec3 marble(vec3 p) {
    float warp  = fbm(p * 2.5);                                   // distorts the veins
    float vein  = abs(sin(4.0 * p.x + 3.0 * p.y + 7.0 * warp));   // 0 on a vein line
    vec3 stone  = mix(vec3(0.10, 0.20, 0.42), vec3(0.92, 0.88, 0.80),
                      smoothstep(0.25, 0.75, fbm(p * 4.0 + 3.0)));
    vec3 gold   = vec3(0.95, 0.65, 0.15);
    return mix(gold, stone, smoothstep(0.0, 0.22, vein));
}

void main() {
    vec3 kd = marble(wPos);                                       // texture colour = diffuse colour
    vec3 lightPos = vec3(2.0, 3.0, 3.0);
    vec3 N = normalize(eNrm), L = normalize(lightPos - ePos), V = normalize(-ePos);
    if (dot(N, V) < 0.0) N = -N;
    vec3 R = reflect(-L, N);
    float diff = max(dot(N, L), 0.0);
    float spec = pow(max(dot(R, V), 0.0), 60.0);
    color = vec4(0.15 * kd + diff * kd + 0.5 * spec, 1.0);
}
)GLSL";

OrbitCamera cam;
int winW = 800, winH = 600;
GLuint prog;
VBuf model;

// Create a simple SMF model so the program runs even without a model file.
void writeTorusSmf(const char* path, int nu = 64, int nv = 32, float R = 1.0f, float r = 0.4f) {
    std::ofstream f(path);
    for (int i = 0; i < nu; i++)
        for (int j = 0; j < nv; j++) {
            float t = 6.2831853f * i / nu, p = 6.2831853f * j / nv;
            f << "v " << (R + r * std::cos(p)) * std::cos(t) << " " << r * std::sin(p) << " " << (R + r * std::cos(p)) * std::sin(t) << "\n";
        }
    for (int i = 0; i < nu; i++)
        for (int j = 0; j < nv; j++) {
            int a = i * nv + j, b = ((i + 1) % nu) * nv + j;
            int c = ((i + 1) % nu) * nv + (j + 1) % nv, d = i * nv + (j + 1) % nv;
            f << "f " << a + 1 << " " << d + 1 << " " << c + 1 << "\nf " << a + 1 << " " << c + 1 << " " << b + 1 << "\n";
        }
}

// Read 'v x y z' and 'f a b c' lines, centre + scale the model, build smooth vertex normals.
std::vector<float> loadSmf(const char* path) {
    std::ifstream in(path);
    if (!in) { std::printf("Cannot open %s\n", path); std::exit(1); }
    std::vector<Vec3> V;
    std::vector<int> F;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string tag; ss >> tag;
        if (tag == "v") { Vec3 v; ss >> v.x >> v.y >> v.z; V.push_back(v); }
        else if (tag == "f") { for (int k = 0; k < 3; k++) { std::string t; ss >> t; F.push_back(std::atoi(t.c_str()) - 1); } }
    }
    Vec3 lo = V[0], hi = V[0];                                    // bounding box
    for (Vec3 v : V) {
        lo = {std::fmin(lo.x, v.x), std::fmin(lo.y, v.y), std::fmin(lo.z, v.z)};
        hi = {std::fmax(hi.x, v.x), std::fmax(hi.y, v.y), std::fmax(hi.z, v.z)};
    }
    float size = std::fmax(hi.x - lo.x, std::fmax(hi.y - lo.y, hi.z - lo.z));
    for (Vec3& v : V) v = (v - (lo + hi) * 0.5f) * (2.5f / size); // largest side = 2.5 units
    std::vector<Vec3> N(V.size(), Vec3{0, 0, 0});
    for (size_t t = 0; t + 2 < F.size(); t += 3) {                // add area-weighted face normal to its 3 vertices
        Vec3 fn = cross(V[F[t + 1]] - V[F[t]], V[F[t + 2]] - V[F[t]]);
        for (int k = 0; k < 3; k++) N[F[t + k]] = N[F[t + k]] + fn;
    }
    std::vector<float> data;
    for (int idx : F) { push(data, V[idx]); push(data, normalize(N[idx])); }
    return data;
}

void display() {
    glClearColor(0.1f, 0.1f, 0.13f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(prog);
    setMat4(prog, "view", cam.view());
    setMat4(prog, "proj", perspective(45, (float)winW / winH, 0.1f, 100));
    model.draw(GL_TRIANGLES);
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
    const char* path = argc > 1 ? argv[1] : "torus.smf";          // ./part3b bunny.smf
    if (!std::ifstream(path)) writeTorusSmf(path);
    initWindow(argc, argv, "3D procedural texture", winW, winH);
    glEnable(GL_DEPTH_TEST);
    prog = makeProgram(VS, FS);
    model.init({3, 3});
    model.set(loadSmf(path));
    cam.init({0, 0, 0}, 6.0f, 0.0f, 0.4f);
    std::printf("Model: %s | mouse drag / arrows: rotate | z,x: zoom | r: reset | Esc: quit\n", path);
    glutDisplayFunc(display); glutReshapeFunc(reshape); glutKeyboardFunc(keyboard);
    glutSpecialFunc(special); glutMouseFunc(mouse); glutMotionFunc(motion);
    glutMainLoop();
}
