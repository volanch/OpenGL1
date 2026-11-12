#ifndef COMPUTERGRAPHICSASSIGNMENT3_SMFLOADER_H
#define COMPUTERGRAPHICSASSIGNMENT3_SMFLOADER_H
#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

// Raw triangle mesh as stored in an .smf file:
struct SmfModel {
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec3> triangles;
};

bool LoadSMF(const std::string& path, SmfModel& model, std::string& error);

#endif //COMPUTERGRAPHICSASSIGNMENT3_SMFLOADER_H
