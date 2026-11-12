#pragma once
#include <vector>
#include "MathUtils.h"

typedef Vec3 (*SurfaceFn)(float u, float v);

Vec3 spherePos(float u, float v);
Vec3 torusPos(float u, float v);
Vec3 torusNormal(float u, float v);
Vec3 conePos(float u, float v);
Vec3 coneNormal(float u, float v);

// Interleaved triangle list: position(3) + normal(3).
std::vector<float> makeSurfaceMesh(SurfaceFn pos, SurfaceFn normal, int n = 48);
