//
// Created by vladk on 02.10.2026.
//

#ifndef COMPUTERGRAPHICSASSIGNMENT2_TRANSFORM_H
#define COMPUTERGRAPHICSASSIGNMENT2_TRANSFORM_H
#pragma once
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class Mode { Scale = 1, Rotate = 2, Translate = 3 };

struct TransformState {
    glm::vec3 scale{0.6f}, rotate{135.0f, -35.264f, -30.0f}, translate{0.0f};
    float dScale = 0.10f, dRotate = 5.0f, dTranslate = 0.10f;

    void Reset() { *this = TransformState{}; }
    void Isometric() { rotate = {35.264f, 45.0f, 0.0f}; scale = glm::vec3(0.6f); translate = glm::vec3(0.0f); }

    // Adjust component `axis` (0=x,1=y,2=z) of the active mode; sign = +1 / -1
    void Adjust(Mode m, int axis, float sign) {
        switch (m) {
            case Mode::Scale: scale[axis] = std::max(0.01f, scale[axis] + sign * dScale); break;
            case Mode::Rotate: rotate[axis] += sign * dRotate; break;
            case Mode::Translate: translate[axis] += sign * dTranslate; break;
        }
    }
    void AdjustDelta(Mode m, float sign) {
        switch (m) {
            case Mode::Scale: dScale = std::max(0.01f, dScale + sign * 0.02f); break;
            case Mode::Rotate: dRotate = std::max(0.5f,  dRotate + sign * 1.0f); break;
            case Mode::Translate: dTranslate = std::max(0.01f, dTranslate + sign * 0.02f); break;
        }
    }

    glm::mat4 Matrix() const { // M = T * R * S
        glm::mat4 R(1.0f);
        R = glm::rotate(R, glm::radians(rotate.z), {0,0,1});
        R = glm::rotate(R, glm::radians(rotate.y), {0,1,0});
        R = glm::rotate(R, glm::radians(rotate.x), {1,0,0});
        return glm::translate(glm::mat4(1.0f), translate) * R * glm::scale(glm::mat4(1.0f), scale);
    }
};
#endif //COMPUTERGRAPHICSASSIGNMENT2_TRANSFORM_H
