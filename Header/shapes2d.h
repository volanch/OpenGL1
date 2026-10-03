//
// Created by vladk on 02.10.2026.
//

#ifndef COMPUTERGRAPHICSASSIGNMENT2_SHAPES2D_H
#define COMPUTERGRAPHICSASSIGNMENT2_SHAPES2D_H
#pragma once
#include <vector>
#include <cmath>

struct Vertex2D { float x, y; };
constexpr float kPi = 3.14159265358979f;

inline std::vector<Vertex2D> Ellipse(float rx, float ry, int segs = 64) {
    std::vector<Vertex2D> v{{0.0f, 0.0f}};                       // fan centre
    for (int i = 0; i <= segs; ++i) {
        float t = 2.0f * kPi * i / segs;
        v.push_back({rx * cosf(t), ry * sinf(t)});
    }
    return v;
}

inline std::vector<Vertex2D> Square(float h) {
    return {{-h, -h}, {h, -h}, {h, h}, {-h, h}};
}

inline std::vector<Vertex2D> Triangle(float r) {
    std::vector<Vertex2D> v;
    for (int k = 0; k < 3; ++k) {
        float a = kPi / 2.0f + k * 2.0f * kPi / 3.0f;
        v.push_back({r * cosf(a), r * sinf(a)});
    }
    return v;
}
#endif //COMPUTERGRAPHICSASSIGNMENT2_SHAPES2D_H
