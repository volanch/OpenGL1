#include "Surfaces.h"
#include "Mesh.h"

namespace { const float PI = 3.14159265f; }

Vec3 spherePos(float u, float v) {
    float t = 2 * PI * u, p = PI * v;
    return {std::sin(p) * std::cos(t), std::cos(p), std::sin(p) * std::sin(t)};
}

Vec3 torusPos(float u, float v) {
    float R = 0.75f, r = 0.3f, t = 2 * PI * u, p = 2 * PI * v;
    return {(R + r * std::cos(p)) * std::cos(t), r * std::sin(p), (R + r * std::cos(p)) * std::sin(t)};
}

Vec3 torusNormal(float u, float v) {
    float t = 2 * PI * u, p = 2 * PI * v;
    return {std::cos(p) * std::cos(t), std::sin(p), std::cos(p) * std::sin(t)};
}

Vec3 conePos(float u, float v) {
    float r = 0.9f, h = 2.0f, t = 2 * PI * u, rad = (1 - v) * r;
    return {rad * std::cos(t), v * h - h / 2, rad * std::sin(t)};
}

Vec3 coneNormal(float u, float) {
    float r = 0.9f, h = 2.0f, t = 2 * PI * u;
    return normalize({h * std::cos(t), r, h * std::sin(t)});
}

std::vector<float> makeSurfaceMesh(SurfaceFn pos, SurfaceFn nrm, int N) {
    std::vector<Vec3> P, Nr;
    for (int i = 0; i <= N; i++)
        for (int j = 0; j <= N; j++) {
            P.push_back(pos((float)i / N, (float)j / N));
            Nr.push_back(nrm((float)i / N, (float)j / N));
        }
    auto tp = quadsToTriangles(P, N, N), tn = quadsToTriangles(Nr, N, N);
    std::vector<float> data;
    for (size_t k = 0; k < tp.size(); k++) { push(data, tp[k]); push(data, tn[k]); }
    return data;
}
