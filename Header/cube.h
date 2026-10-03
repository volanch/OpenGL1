//
// Created by vladk on 02.10.2026.
//

#ifndef COMPUTERGRAPHICSASSIGNMENT2_CUBE_H
#define COMPUTERGRAPHICSASSIGNMENT2_CUBE_H

#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Vertex3D { float x, y, z, r, g, b; };

// Vertices of a unit cube centered at origin, sides aligned with axes
inline const glm::vec3 vertices[8] = {
    {-0.5f, -0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f},
    {-0.5f, -0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}
};

// RGB colors
inline const glm::vec3 vertex_colors[8] = {
    {0.0f, 0.0f, 0.0f},  // black
    {1.0f, 0.0f, 0.0f},  // red
    {1.0f, 1.0f, 0.0f},  // yellow
    {0.0f, 1.0f, 0.0f},  // green
    {0.0f, 0.0f, 1.0f},  // blue
    {1.0f, 0.0f, 1.0f},  // magenta
    {1.0f, 1.0f, 1.0f},  // white
    {0.0f, 1.0f, 1.0f}   // cyan
};

// quad generates two triangles for each face and assigns colors to the vertices
inline void quad(std::vector<Vertex3D>& out, int a, int b, int c, int d) {
    auto add = [&](int i) {
        out.push_back({vertices[i].x, vertices[i].y, vertices[i].z,
                       vertex_colors[i].r, vertex_colors[i].g, vertex_colors[i].b});
    };
    add(a); add(b); add(c);
    add(a); add(c); add(d);
}

inline std::vector<Vertex3D> colorcube() {
    std::vector<Vertex3D> v;
    v.reserve(36);
    quad(v, 1, 0, 3, 2);
    quad(v, 2, 3, 7, 6);
    quad(v, 3, 0, 4, 7);
    quad(v, 6, 5, 1, 2);
    quad(v, 4, 5, 6, 7);
    quad(v, 5, 4, 0, 1);
    return v;
}
#endif //COMPUTERGRAPHICSASSIGNMENT2_CUBE_H
