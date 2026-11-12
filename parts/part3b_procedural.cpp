#include <glad/glad.h>
#include <GL/freeglut.h>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include "PhongScene.h"
#include "Window.h"
#include "VAO.h"
#include "VBO.h"
#include "modelData.h"
#include "shaderClass.h"
#include "smfLoader.h"

namespace fs = std::filesystem;

namespace {
    PhongScene scene;
    std::unique_ptr<Shader> marble;
    std::unique_ptr<VAO> vao;
    std::unique_ptr<VBO> vbo;
    ModelData model;
    std::string modelName;

    std::string resolveModelPath(const std::string& arg) {
        if (fs::exists(arg)) return arg;
        if (fs::exists("models/" + arg)) return "models/" + arg;
        return "";
    }

    void updateTitle() {
        glutSetWindowTitle(("Part 3b | " + scene.status() + " | " + modelName + " (" +
                            std::to_string(model.triangleCount) + " tris)").c_str());
    }

    void display() {
        scene.beginFrame();

        marble->Activate();
        scene.uploadUniforms(*marble);
        marble->SetVec3("uCenter", model.center);
        marble->SetFloat("uRadius", model.radius);

        vao->Bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(model.vertices.size()));

        scene.drawLightMarker();
        glutSwapBuffers();
    }

    void reshape(int w, int h) { scene.setViewport(w, h); }

    void keyboard(unsigned char key, int, int) {
        if (!scene.onKey(key)) return;
        updateTitle();
        glutPostRedisplay();
    }

    void special(int key, int, int) {
        if (!scene.onSpecial(key)) return;
        updateTitle();
        glutPostRedisplay();
    }

    bool loadModel(const std::string& arg) {
        const std::string path = resolveModelPath(arg);
        if (path.empty()) {
            std::cerr << "Cannot find model '" << arg << "'. Pass a path or put the .smf file into the 'models' folder.\n";
            return false;
        }
        SmfModel smf;
        std::string error;
        if (!LoadSMF(path, smf, error)) {
            std::cerr << "Failed to load " << path << ": " << error << "\n";
            return false;
        }
        model = BuildModelData(smf);
        modelName = fs::path(path).filename().string();
        std::cout << "Loaded " << modelName << ": " << smf.vertices.size() << " vertices, "
                  << model.triangleCount << " triangles\n";
        return true;
    }

    void createModelBuffers() {
        vao = std::make_unique<VAO>();
        vbo = std::make_unique<VBO>(reinterpret_cast<GLfloat*>(model.vertices.data()),
                                    static_cast<GLsizeiptr>(model.vertices.size() * sizeof(Vertex)));
        vao->Bind();
        vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
        vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, faceNormal));
        vao->LinkAttrib(*vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, smoothNormal));
        vao->Unbind();
    }
}

int main(int argc, char** argv) {
    const std::string modelArg = argc >= 2 ? argv[1] : "bound-lo-sphere.smf";
    if (argc < 2) std::cout << "No model given - using " << modelArg << "\nUsage: part3b_procedural <model.smf>\n";
    if (!loadModel(modelArg)) return 1;

    initWindow(argc, argv, "Part 3b", 800, 800);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    try {
        marble = std::make_unique<Shader>("shaders/part3b.vert", "shaders/part3b.frag");
        scene.init("shaders");
    } catch (int) {
        std::cerr << "Cannot read the shader files in the 'shaders' folder. Run from the folder that contains it.\n";
        return 1;
    }

    createModelBuffers();
    scene.setBounds(model.center, model.radius);

    PhongScene::PrintControls();
    updateTitle();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}
