#include "PhongScene.h"
#include <glad/glad.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "materials.h"

namespace {
    constexpr float kFovY = glm::radians(45.0f);
    constexpr float kAngleStep = glm::radians(3.0f);
    constexpr float kHeightStep = 0.05f;   // x model radius
    constexpr float kRadiusStep = 0.10f;   // x model radius
    constexpr float kMinRadius = 0.10f;    // x model radius

    const Material& kMaterial = kMaterials[0];
}

void PhongScene::init(const std::string& shaderDir) {
    namespace fs = std::filesystem;
    marker_ = std::make_unique<Shader>((fs::path(shaderDir) / "marker.vert").string().c_str(),
                                       (fs::path(shaderDir) / "marker.frag").string().c_str());
    markerVao_ = std::make_unique<VAO>();   // attribute-less VAO, position is a uniform
}

void PhongScene::setBounds(const glm::vec3& center, float radius, const ViewPreset& preset) {
    center_ = center;
    radius_ = radius;
    preset_ = preset;
    reset();
}

void PhongScene::reset() {
    const float r = radius_;
    camera_ = {preset_.cameraAngle, preset_.cameraRadius * r, preset_.cameraHeight * r};
    light_ = {preset_.lightAngle, preset_.lightRadius * r, preset_.lightHeight * r};
    cameraLightEye_ = glm::vec3(0.5f * r, 0.5f * r, 0.0f);   // a little right/above the eye
}

void PhongScene::setViewport(int w, int h) {
    winW_ = w;
    winH_ = std::max(h, 1);
    glViewport(0, 0, winW_, winH_);
}

void PhongScene::computeMatrices(glm::mat4& view, glm::mat4& proj) const {
    const glm::vec3 up = zUp_ ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
    const glm::vec3 eye = center_ + CylindricalToCartesian(camera_, zUp_);
    view = glm::lookAt(eye, center_, up);                    // model matrix = identity

    const float aspect = static_cast<float>(winW_) / static_cast<float>(winH_);
    const float dist = glm::length(eye - center_);
    const float zNear = std::max(0.02f * radius_, 1e-4f);
    const float zFar = dist + 3.0f * radius_;
    if (perspective_) {
        proj = glm::perspective(kFovY, aspect, zNear, zFar);
    } else {
        const float halfH = dist * std::tan(kFovY * 0.5f);
        proj = glm::ortho(-halfH * aspect, halfH * aspect, -halfH, halfH, zNear, zFar);
    }
}

void PhongScene::beginFrame() const {
    glClearColor(0.18f, 0.18f, 0.20f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PhongScene::uploadUniforms(Shader& shader) const {
    glm::mat4 view, proj;
    computeMatrices(view, proj);
    shader.SetMat4("uModelView", view);
    shader.SetMat4("uProjection", proj);
    shader.SetMat3("uNormalMatrix", glm::inverseTranspose(glm::mat3(view)));
    shader.SetFloat("uNormalSign", flipNormals_ ? -1.0f : 1.0f);

    const glm::vec3 light1Obj = center_ + CylindricalToCartesian(light_, zUp_);
    const glm::vec3 lightEye[2] = {glm::vec3(view * glm::vec4(light1Obj, 1.0f)), cameraLightEye_};
    for (int i = 0; i < 2; ++i) {
        const std::string base = "uLight[" + std::to_string(i) + "].";
        shader.SetVec3((base + "position").c_str(), lightEye[i]);
        shader.SetVec4((base + "ambient").c_str(), kLightColors.ambient);
        shader.SetVec4((base + "diffuse").c_str(), kLightColors.diffuse);
        shader.SetVec4((base + "specular").c_str(), kLightColors.specular);
        shader.SetFloat((base + "enabled").c_str(), lightOn_[i] ? 1.0f : 0.0f);
    }
    shader.SetVec4("uMaterial.specular", kMaterial.specular);
    shader.SetFloat("uMaterial.shininess", kMaterial.shininess);
}

void PhongScene::drawLightMarker() const {
    if (!lightOn_[0]) return;
    glm::mat4 view, proj;
    computeMatrices(view, proj);
    marker_->Activate();
    marker_->SetMat4("uModelView", view);
    marker_->SetMat4("uProjection", proj);
    marker_->SetVec3("uPos", center_ + CylindricalToCartesian(light_, zUp_));
    markerVao_->Bind();
    glDrawArrays(GL_POINTS, 0, 1);
}

bool PhongScene::onKey(unsigned char key) {
    const float r = radius_;
    switch (key) {
        case '+': case '=': camera_.radius += kRadiusStep * r; break;
        case '-': case '_': camera_.radius = std::max(camera_.radius - kRadiusStep * r, kMinRadius * r); break;
        case 'a': case 'A': light_.angle -= kAngleStep; break;
        case 'd': case 'D': light_.angle += kAngleStep; break;
        case 'w': case 'W': light_.height += kHeightStep * r; break;
        case 's': case 'S': light_.height -= kHeightStep * r; break;
        case 'e': case 'E': light_.radius += kRadiusStep * r; break;
        case 'q': case 'Q': light_.radius = std::max(light_.radius - kRadiusStep * r, kMinRadius * r); break;
        case 'p': case 'P': perspective_ = !perspective_; break;
        case 'z': case 'Z': lightOn_[0] = !lightOn_[0]; break;
        case 'x': case 'X': lightOn_[1] = !lightOn_[1]; break;
        case 'u': case 'U': zUp_ = !zUp_; break;
        case 'n': case 'N': flipNormals_ = !flipNormals_; break;
        case 'r': case 'R': reset(); break;
        case 27: glutLeaveMainLoop(); return false;
        default: return false;
    }
    return true;
}

bool PhongScene::onSpecial(int key) {
    const float r = radius_;
    switch (key) {
        case GLUT_KEY_LEFT: camera_.angle -= kAngleStep; break;
        case GLUT_KEY_RIGHT: camera_.angle += kAngleStep; break;
        case GLUT_KEY_UP: camera_.height += kHeightStep * r; break;
        case GLUT_KEY_DOWN: camera_.height -= kHeightStep * r; break;
        case GLUT_KEY_PAGE_UP: camera_.radius += kRadiusStep * r; break;
        case GLUT_KEY_PAGE_DOWN: camera_.radius = std::max(camera_.radius - kRadiusStep * r, kMinRadius * r); break;
        default: return false;
    }
    return true;
}

std::string PhongScene::status() const {
    std::ostringstream t;
    t << std::fixed << std::setprecision(0)
      << "Phong | " << (perspective_ ? "Perspective" : "Parallel")
      << " | cam a=" << std::fmod(glm::degrees(camera_.angle), 360.0f)
      << " R=" << std::setprecision(2) << camera_.radius << " H=" << camera_.height
      << " | light a=" << std::setprecision(0) << std::fmod(glm::degrees(light_.angle), 360.0f)
      << " R=" << std::setprecision(2) << light_.radius << " H=" << light_.height;
    return t.str();
}

void PhongScene::PrintControls() {
    std::cout <<
        "\nControls\n"
        " CAMERA (cylinder around the model)   arrows: angle / height     + - (PgUp/PgDn): radius\n"
        " LIGHT 1 (cylinder, object coords)    a d: angle   w s: height   e q: radius\n"
        " LIGHT 2 (camera coords, near the eye)\n"
        "   p: parallel / perspective   z x: light 1 / light 2 on-off\n"
        "   u: cylinder axis Z-up / Y-up   n: flip normals   r: reset   Esc: quit\n" << std::endl;
}
