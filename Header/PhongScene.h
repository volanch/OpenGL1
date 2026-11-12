#ifndef PHONGSCENE_H
#define PHONGSCENE_H
#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "orbit.h"
#include "shaderClass.h"
#include "VAO.h"

// Initial camera / light position, in units of the model radius.
struct ViewPreset {
    float cameraAngle = 0.0f, cameraRadius = 3.0f, cameraHeight = 0.5f;
    float lightAngle = glm::radians(60.0f), lightRadius = 2.0f, lightHeight = 1.5f;
};

// Assignment 3 viewing + lighting set-up, Phong only, one material (glossy, with specular highlight):
//  - camera on a cylinder around the model, always looking at the centre
//  - light 1 on a cylinder (object coordinates), light 2 in camera coordinates
//  - parallel / perspective projection
class PhongScene {
public:
    void init(const std::string& shaderDir = "shaders");           // creates the light marker
    void setBounds(const glm::vec3& center, float radius, const ViewPreset& preset = ViewPreset());
    void setViewport(int w, int h);

    void beginFrame() const;                                       // clear colour + depth
    void uploadUniforms(Shader& shader) const;                     // matrices, lights, material
    void drawLightMarker() const;                                  // yellow dot at light 1

    bool onKey(unsigned char key);                                 // true = redraw
    bool onSpecial(int key);                                       // true = redraw
    std::string status() const;
    static void PrintControls();

    glm::vec3 center() const { return center_; }
    float radius() const { return radius_; }

private:
    void computeMatrices(glm::mat4& view, glm::mat4& proj) const;
    void reset();

    glm::vec3 center_{0.0f};
    float radius_ = 1.0f;
    ViewPreset preset_;
    Orbit camera_{}, light_{};
    glm::vec3 cameraLightEye_{};
    bool perspective_ = true, zUp_ = true, flipNormals_ = false;
    bool lightOn_[2] = {true, true};
    int winW_ = 800, winH_ = 800;
    std::unique_ptr<Shader> marker_;
    std::unique_ptr<VAO> markerVao_;
};

#endif
