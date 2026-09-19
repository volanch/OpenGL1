//
// Created by vladk on 19.09.2026.
//

#ifndef COMPUTERGRAPHICSPROJECT_SHAPEGEN_H
#define COMPUTERGRAPHICSPROJECT_SHAPEGEN_H
#include<vector>
#include<cmath>
#include<functional>

struct Vertex {
    float x, y;
    float r, g, b;
};

const float PI = 3.14159265358979323846f;

inline std::vector<Vertex> MakeEllipseFan(
    float cx, float cy, float rx, float ry, int segments,
    float centerR, float centerG, float centerB,
    const std::function<void(float, float&, float&, float&)>& colorAtAngle,
    float centerOffsetX = 0.0f, float centerOffsetY = 0.0f) {
    std::vector<Vertex> verts;
    verts.reserve(segments + 2);

    verts.push_back({cx + centerOffsetX, cy + centerOffsetY, centerR, centerG, centerB});

    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * PI * float(i) / float(segments);
        float x = cx + rx * cosf(angle);
        float y = cy + ry * sinf(angle);
        float r, g, b;
        colorAtAngle(angle, r, g, b);
        verts.push_back({x, y, r, g, b});
    }
    return verts;
}

inline std::vector<Vertex> MakeTriangle(
    float cx, float cy, float r,
    float r0, float g0, float b0,
    float r1, float g1, float b1,
    float r2, float g2, float b2)
{
    // Points at angle = pi/2 (top), pi/2 + 2pi/3 (bottom-left), pi/2 + 4pi/3 (bottom-right)
    float a0 = PI / 2.0f;
    float a1 = a0 + 2.0f * PI / 3.0f;
    float a2 = a0 + 4.0f * PI / 3.0f;

    return {
        {cx + r * cosf(a0), cy + r * sinf(a0), r0, g0, b0},
        {cx + r * cosf(a1), cy + r * sinf(a1), r1, g1, b1},
        {cx + r * cosf(a2), cy + r * sinf(a2), r2, g2, b2},
    };
}

inline std::vector<Vertex> MakeSquare(float cx, float cy, float halfSide, float r, float g, float b)
{
    float radius = halfSide * sqrtf(2.0f);
    std::vector<Vertex> verts;
    verts.reserve(4);
    for (int k = 0; k < 4; k++)
    {
        float angle = PI / 4.0f + float(k) * (PI / 2.0f);
        float x = cx + radius * cosf(angle);
        float y = cy + radius * sinf(angle);
        verts.push_back({x, y, r, g, b});
    }
    return verts;
}

inline std::vector<Vertex> MakeGradientQuad(
    float x0, float y0, float x1, float y1,
    float topR, float topG, float topB,
    float botR, float botG, float botB)
{
    return {
        {x0, y0, botR, botG, botB}, // bottom left
        {x1, y0, botR, botG, botB}, // bottom right
        {x1, y1, topR, topG, topB}, // top right
        {x0, y1, topR, topG, topB}, // top left
    };
}

inline std::vector<Vertex> MakeStarFan(
    float cx, float cy, float outerR, float innerR, int points,
    float centerR, float centerG, float centerB,
    const std::function<void(float, float&, float&, float&)>& colorAtAngle)
{
    std::vector<Vertex> verts;
    int n = points * 2;
    verts.reserve(n + 2);
    verts.push_back({cx, cy, centerR, centerG, centerB});

    for (int i = 0; i <= n; i++)
    {
        float angle = PI / 2.0f + 2.0f * PI * float(i) / float(n);
        float radius = (i % 2 == 0) ? outerR : innerR;
        float x = cx + radius * cosf(angle);
        float y = cy + radius * sinf(angle);
        float r, g, b;
        colorAtAngle(angle, r, g, b);
        verts.push_back({x, y, r, g, b});
    }
    return verts;
}

#endif //COMPUTERGRAPHICSPROJECT_SHAPEGEN_H
