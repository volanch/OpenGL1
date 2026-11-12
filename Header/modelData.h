#ifndef COMPUTERGRAPHICSASSIGNMENT3_MODELDATA_H
#define COMPUTERGRAPHICSASSIGNMENT3_MODELDATA_H
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "smfLoader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 faceNormal;
    glm::vec3 smoothNormal;
};

struct ModelData {
    std::vector<Vertex> vertices;
    size_t triangleCount = 0;
    glm::vec3 center{0.0f};
    float radius = 1.0f;
};

ModelData BuildModelData(const SmfModel& smf);

#endif //COMPUTERGRAPHICSASSIGNMENT3_MODELDATA_H
