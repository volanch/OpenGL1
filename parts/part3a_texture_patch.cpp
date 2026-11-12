#include <glad/glad.h>
#include <GL/freeglut.h>
#include <iostream>
#include <memory>
#include "BezierPatch.h"
#include "PatchModel.h"
#include "PhongScene.h"
#include "Texture.h"
#include "Window.h"
#include "VAO.h"
#include "VBO.h"
#include "shaderClass.h"

namespace {
    PhongScene scene;
    std::unique_ptr<Shader> phong;
    std::unique_ptr<VAO> vao;
    std::unique_ptr<VBO> vbo;
    PatchModel patch;
    GLuint texture = 0;

    void updateTitle() {
        glutSetWindowTitle(("Part 3a | " + scene.status() + " | " + std::to_string(patch.triangleCount) + " tris").c_str());
    }

    void display() {
        scene.beginFrame();

        phong->Activate();
        scene.uploadUniforms(*phong);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        phong->SetInt("uTexture", 0);

        vao->Bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(patch.vertices.size()));

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

    void createPatchBuffers() {
        patch = BuildPatchModel(loadControlPoints(), 12);
        vao = std::make_unique<VAO>();
        vbo = std::make_unique<VBO>(reinterpret_cast<GLfloat*>(patch.vertices.data()),
                                    static_cast<GLsizeiptr>(patch.vertices.size() * sizeof(PatchVertex)));
        vao->Bind();
        vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, sizeof(PatchVertex), (void*)offsetof(PatchVertex, position));
        vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, sizeof(PatchVertex), (void*)offsetof(PatchVertex, normal));
        vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, sizeof(PatchVertex), (void*)offsetof(PatchVertex, uv));
        vao->Unbind();
    }
}

int main(int argc, char** argv) {
    initWindow(argc, argv, "Part 3a", 800, 800);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    try {
        phong = std::make_unique<Shader>("shaders/part3a.vert", "shaders/part3a.frag");
        scene.init("shaders");
    } catch (int) {
        std::cerr << "Cannot read the shader files in the 'shaders' folder. Run from the folder that contains it.\n";
        return 1;
    }

    createPatchBuffers();
    const int texSize = 256;
    texture = createTexture2D(makeSwirlImage(texSize), texSize, 0);

    ViewPreset preset;
    preset.cameraAngle = glm::radians(-90.0f);
    preset.cameraRadius = 1.2f;
    preset.cameraHeight = 2.0f;
    preset.lightAngle = glm::radians(-45.0f);
    preset.lightRadius = 2.0f;
    preset.lightHeight = 2.0f;
    scene.setBounds(patch.center, patch.radius, preset);

    PhongScene::PrintControls();
    updateTitle();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}
