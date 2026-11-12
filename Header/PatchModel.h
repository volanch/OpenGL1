#ifndef PATCHMODEL_H
#define PATCHMODEL_H
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "MathUtils.h"

struct PatchVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;        // equal to the (u,v) used to compute the position
};

struct PatchModel {
    std::vector<PatchVertex> vertices;   // triangle list, 3 per triangle
    size_t triangleCount = 0;
    glm::vec3 center{0.0f};
    float radius = 1.0f;
};

// n x n quads -> 2*n*n triangles from a bicubic Bezier patch (16 control points).
PatchModel BuildPatchModel(const std::vector<Vec3>& controlPoints, int n);

#endif
