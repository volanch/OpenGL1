#ifndef COMPUTERGRAPHICSASSIGNMENT3_MATERIALS_H
#define COMPUTERGRAPHICSASSIGNMENT3_MATERIALS_H
#pragma once
#include <array>
#include <glm/glm.hpp>

struct Material {
    const char* name;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float shininess;
};

inline const std::array<Material, 4> kMaterials = {{
    {"Glossy red plastic (assignment)",
        {0.6f, 0.2f, 0.2f, 1.0f}, {0.9f, 0.1f, 0.1f, 1.0f}, {0.8f, 0.8f, 0.8f, 1.0f}, 80.0f},
    {"Matte clay",
        {0.45f, 0.33f, 0.24f, 1.0f}, {0.65f, 0.48f, 0.32f, 1.0f}, {0.04f, 0.04f, 0.04f, 1.0f}, 4.0f},
    {"Gold",
        {0.24725f, 0.1995f, 0.0745f, 1.0f}, {0.75164f, 0.60648f, 0.22648f, 1.0f},
        {0.628281f, 0.555802f, 0.366065f, 1.0f}, 51.2f},
    {"Cyan plastic",
        {0.0f, 0.1f, 0.06f, 1.0f}, {0.0f, 0.51f, 0.51f, 1.0f}, {0.5f, 0.5f, 0.5f, 1.0f}, 32.0f},
}};

struct LightColors {
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
};
inline const LightColors kLightColors = {
    {0.2f, 0.2f, 0.2f, 1.0f},
    {0.6f, 0.6f, 0.6f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f},
};

#endif //COMPUTERGRAPHICSASSIGNMENT3_MATERIALS_H
