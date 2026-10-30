#include <glad/glad.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <iomanip>
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "smfLoader.h"
#include "modelData.h"
#include "materials.h"
#include "orbit.h"

namespace fs = std::filesystem;

namespace {

    enum class Shading { Flat, Gouraud, Phong };

    constexpr float kFovY = glm::radians(45.0f);
    constexpr float kAngleStep = glm::radians(3.0f);   // per key press
    constexpr float kHeightStep = 0.05f; // x model radius
    constexpr float kRadiusStep = 0.10f; // x model radius
    constexpr float kMinRadius = 0.10f;  // x model radius

    // application state
    ModelData g_model;
    std::string g_modelName;
    size_t g_vertexCount = 0;

    std::unique_ptr<Shader> g_flatShader, g_gouraudShader, g_phongShader, g_markerShader;
    std::unique_ptr<VAO> g_vao, g_markerVao;
    std::unique_ptr<VBO> g_vbo;

    Orbit g_camera{}; // camera on a cylinder around the model centre
    Orbit g_light{}; // light 1: on a cylinder, defined in OBJECT coordinates
    glm::vec3 g_cameraLightEye{};  // light 2: defined in CAMERA coordinates (moves with the camera)

    Shading g_shading = Shading::Phong;
    bool g_perspective = true;
    bool g_zUp = true;
    bool g_flipNormals = false;
    bool g_lightOn[2] = {true, true};
    int g_material = 0;
    int g_winW = 800, g_winH = 800;

    // helpers
    std::string ShaderPath(const char* name) { return (fs::path("shaders") / name).string(); }
    std::string ResolveModelPath(const std::string& arg) {
        const fs::path p(arg);
        const fs::path candidates[] = { p, fs::path("models") / p };
        for (const auto& c : candidates) {
            std::error_code ec;
            if (fs::is_regular_file(c, ec)) return c.string();
        }
        return "";
    }

    void ResetView() {
        const float r = g_model.radius;
        g_camera= {0.0f, 3.0f * r, 0.5f * r};
        g_light= {glm::radians(60.0f), 2.0f * r, 1.5f * r};
        g_cameraLightEye = glm::vec3(0.5f * r, 0.5f * r, 0.0f);   // a little right/above the eye
    }

    const char* ShadingName() {
        switch (g_shading) {
            case Shading::Flat: return "Flat (normal colours)";
            case Shading::Gouraud: return "Gouraud";
            default: return "Phong";
        }
    }

    void UpdateTitle() {
        std::ostringstream t;
        t << std::fixed << std::setprecision(2)
          << "A3 | " << ShadingName() << " | " << (g_perspective ? "Perspective" : "Parallel")
          << " | " << kMaterials[g_material].name
          << " | cam a=" << std::setprecision(0) << std::fmod(glm::degrees(g_camera.angle), 360.0f)
          << " R=" << std::setprecision(2) << g_camera.radius << " H=" << g_camera.height
          << " | light a=" << std::setprecision(0) << std::fmod(glm::degrees(g_light.angle), 360.0f)
          << " R=" << std::setprecision(2) << g_light.radius << " H=" << g_light.height
          << " | " << g_modelName << " (" << g_model.triangleCount << " tris)";
        glutSetWindowTitle(t.str().c_str());
    }

    void PrintInstructions() {
        std::cout <<
            "\nAssignment 3 controls\n"
            " CAMERA (cylinder around the model, always looks at the model centre)\n"
            "   Left / Right arrow : cylinder angle  -/+\n"
            "   Up / Down arrow    : camera height   +/-\n"
            "   + / -  (PgUp/PgDn) : orbit radius    larger / smaller\n"
            " LIGHT 1 (cylinder around the model, object coordinates)\n"
            "   a / d : angle -/+      w / s : height +/-      e / q : radius larger / smaller\n"
            " LIGHT 2 (camera coordinates, near the eye - moves with the camera)\n"
            " DISPLAY\n"
            "   1 : flat shading, colour = |normal|  (Part 1)\n"
            "   2 : Gouraud shading                  (Part 2)\n"
            "   3 : Phong shading                    (Part 2)\n"
            "   m / M : next / previous material\n"
            "   p : parallel <-> perspective projection\n"
            "   z / x : switch light 1 / light 2 on and off\n"
            "   u : cylinder axis Z-up <-> Y-up (use Y-up for models such as the bunny)\n"
            "   n : flip normals (if a model is wound clockwise and looks dark)\n"
            "   r : reset view and lights\n"
            "   ESC : quit\n" << std::endl;
    }

    // GLUT callbacks
    void display() {
        const bool lit = g_shading != Shading::Flat;
        if (lit) glClearColor(0.18f, 0.18f, 0.20f, 1.0f);
        else glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // white background like the sample images
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // camera: LookAt on a cylinder, target = model centre (constant)
        const glm::vec3 center = g_model.center;
        const glm::vec3 up = g_zUp ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
        const glm::vec3 eye = center + CylindricalToCartesian(g_camera, g_zUp);
        const glm::mat4 view = glm::lookAt(eye, center, up); // model matrix = identity

        // projection: parallel or perspective, same apparent size at the model centre
        const float aspect = static_cast<float>(g_winW) / static_cast<float>(std::max(g_winH, 1));
        const float dist = glm::length(eye - center);
        const float zNear = std::max(0.02f * g_model.radius, 1e-4f);
        const float zFar = dist + 3.0f * g_model.radius;
        glm::mat4 proj;
        if (g_perspective) {
            proj = glm::perspective(kFovY, aspect, zNear, zFar);
        } else {
            const float halfH = dist * std::tan(kFovY * 0.5f);
            proj = glm::ortho(-halfH * aspect, halfH * aspect, -halfH, halfH, zNear, zFar);
        }

        // draw the model
        Shader* shader = g_flatShader.get();
        if (g_shading == Shading::Gouraud) shader = g_gouraudShader.get();
        if (g_shading == Shading::Phong) shader = g_phongShader.get();

        shader->Activate();
        shader->SetMat4("uModelView", view);
        shader->SetMat4("uProjection", proj);

        if (lit) {
            shader->SetMat3("uNormalMatrix", glm::inverseTranspose(glm::mat3(view)));
            shader->SetFloat("uNormalSign", g_flipNormals ? -1.0f : 1.0f);

            // light 1
            const glm::vec3 light1Obj = center + CylindricalToCartesian(g_light, g_zUp);
            const glm::vec3 light1Eye = glm::vec3(view * glm::vec4(light1Obj, 1.0f));
            // light 2
            const glm::vec3 lightEye[2] = {light1Eye, g_cameraLightEye};

            for (int i = 0; i < 2; ++i) {
                const std::string base = "uLight[" + std::to_string(i) + "].";
                shader->SetVec3((base + "position").c_str(), lightEye[i]);
                shader->SetVec4((base + "ambient").c_str(), kLightColors.ambient);
                shader->SetVec4((base + "diffuse").c_str(), kLightColors.diffuse);
                shader->SetVec4((base + "specular").c_str(), kLightColors.specular);
                shader->SetFloat((base + "enabled").c_str(), g_lightOn[i] ? 1.0f : 0.0f);
            }
            const Material& m = kMaterials[g_material];
            shader->SetVec4("uMaterial.ambient", m.ambient);
            shader->SetVec4("uMaterial.diffuse", m.diffuse);
            shader->SetVec4("uMaterial.specular", m.specular);
            shader->SetFloat("uMaterial.shininess", m.shininess);
        }

        g_vao->Bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(g_vertexCount));

        // small yellow dot showing where light 1 is
        if (lit && g_lightOn[0]) {
            g_markerShader->Activate();
            g_markerShader->SetMat4("uModelView", view);
            g_markerShader->SetMat4("uProjection", proj);
            g_markerShader->SetVec3("uPos", center + CylindricalToCartesian(g_light, g_zUp));
            g_markerVao->Bind();
            glDrawArrays(GL_POINTS, 0, 1);
        }

        glutSwapBuffers();
    }

    void reshape(int w, int h) {
        g_winW = w;
        g_winH = std::max(h, 1);
        glViewport(0, 0, w, g_winH);
    }

    void keyboard(unsigned char key, int, int) {
        const float r = g_model.radius;
        switch (key) {
            // camera radius
            case '+': case '=': g_camera.radius += kRadiusStep * r; break;
            case '-': case '_': g_camera.radius = std::max(g_camera.radius - kRadiusStep * r, kMinRadius * r); break;
            // light 1 (cylinder, object coordinates)
            case 'a': case 'A': g_light.angle -= kAngleStep; break;
            case 'd': case 'D': g_light.angle += kAngleStep; break;
            case 'w': case 'W': g_light.height += kHeightStep * r; break;
            case 's': case 'S': g_light.height -= kHeightStep * r; break;
            case 'e': case 'E': g_light.radius += kRadiusStep * r; break;
            case 'q': case 'Q': g_light.radius = std::max(g_light.radius - kRadiusStep * r, kMinRadius * r); break;
            // display options
            case '1': g_shading = Shading::Flat; break;
            case '2': g_shading = Shading::Gouraud; break;
            case '3': g_shading = Shading::Phong; break;
            case 'm': g_material = (g_material + 1) % static_cast<int>(kMaterials.size()); break;
            case 'M': g_material = (g_material + static_cast<int>(kMaterials.size()) - 1) % static_cast<int>(kMaterials.size()); break;
            case 'p': case 'P': g_perspective = !g_perspective; break;
            case 'z': case 'Z': g_lightOn[0] = !g_lightOn[0]; break;
            case 'x': case 'X': g_lightOn[1] = !g_lightOn[1]; break;
            case 'u': case 'U': g_zUp = !g_zUp; break;
            case 'n': case 'N': g_flipNormals = !g_flipNormals; break;
            case 'r': case 'R': ResetView(); break;
            case 27: glutLeaveMainLoop(); return;
            default: return;
        }
        UpdateTitle();
        glutPostRedisplay();
    }

    void special(int key, int, int) {
        const float r = g_model.radius;
        switch (key) {
            case GLUT_KEY_LEFT: g_camera.angle -= kAngleStep; break;
            case GLUT_KEY_RIGHT: g_camera.angle += kAngleStep; break;
            case GLUT_KEY_UP: g_camera.height += kHeightStep * r; break;
            case GLUT_KEY_DOWN: g_camera.height -= kHeightStep * r; break;
            case GLUT_KEY_PAGE_UP: g_camera.radius += kRadiusStep * r; break;
            case GLUT_KEY_PAGE_DOWN: g_camera.radius = std::max(g_camera.radius - kRadiusStep * r, kMinRadius * r); break;
            default: return;
        }
        UpdateTitle();
        glutPostRedisplay();
}

}  // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    // model file name comes from the command line
    std::string modelArg;
    if (argc >= 2) {
        modelArg = argv[1];
    } else {
        modelArg = "bound-lo-sphere.smf";
        std::cout << "No model given - using the bundled " << modelArg << "\n"
                  << "Usage: assignment3 <model.smf>\n";
    }
    const std::string modelPath = ResolveModelPath(modelArg);
    if (modelPath.empty()) {
        std::cerr << "Cannot find model '" << modelArg << "'.\n"
                  << "Pass a path, or copy the .smf file into the 'models' folder next to the executable.\n";
        return 1;
    }

    SmfModel smf;
    std::string error;
    if (!LoadSMF(modelPath, smf, error)) {
        std::cerr << "Failed to load " << modelPath << ": " << error << std::endl;
        return 1;
    }
    g_model = BuildModelData(smf);
    g_modelName = fs::path(modelPath).filename().string();
    g_vertexCount = g_model.vertices.size();
    std::cout << "Loaded " << g_modelName << ": " << smf.vertices.size() << " vertices, "
              << g_model.triangleCount << " triangles\n";
    if (g_model.triangleCount < 100)
        std::cout << "Warning: the assignment asks for a model with at least 100 triangles.\n";

    // window + OpenGL 3.3 core context
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(g_winW, g_winH);
    glutCreateWindow("Assignment 3");
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialise GLAD (OpenGL 3.3 required)." << std::endl;
        return 1;
    }
    std::cout << "OpenGL " << glGetString(GL_VERSION) << " | " << glGetString(GL_RENDERER) << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // shaders and vertex buffers
    try {
        g_flatShader = std::make_unique<Shader>(ShaderPath("flat.vert").c_str(),ShaderPath("flat.frag").c_str());
        g_gouraudShader = std::make_unique<Shader>(ShaderPath("gouraud.vert").c_str(),ShaderPath("gouraud.frag").c_str());
        g_phongShader = std::make_unique<Shader>(ShaderPath("phong.vert").c_str(),ShaderPath("phong.frag").c_str());
        g_markerShader = std::make_unique<Shader>(ShaderPath("marker.vert").c_str(),ShaderPath("marker.frag").c_str());
    } catch (int) {
        std::cerr << "Cannot read the shader files in the 'shaders' folder. "
                  << "Run the program from the folder that contains 'shaders'." << std::endl;
        return 1;
    }

    g_vao = std::make_unique<VAO>();
    g_vbo = std::make_unique<VBO>(reinterpret_cast<GLfloat*>(g_model.vertices.data()),
                                  static_cast<GLsizeiptr>(g_model.vertices.size() * sizeof(Vertex)));
    g_vao->Bind();
    g_vao->LinkAttrib(*g_vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
    g_vao->LinkAttrib(*g_vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, faceNormal));
    g_vao->LinkAttrib(*g_vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, smoothNormal));
    g_vao->Unbind();
    g_markerVao = std::make_unique<VAO>();   // attribute-less VAO, the marker position is a uniform

    ResetView();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);

    PrintInstructions();
    UpdateTitle();
    glutMainLoop();
    return 0;
}

#if defined(_WIN32) && defined(__MINGW32__)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) { return main(__argc, __argv); }
#endif