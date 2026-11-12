#include "PatchModel.h"
#include "BezierPatch.h"
#include "Mesh.h"
#include <algorithm>

namespace {
    glm::vec3 ToGlm(Vec3 v) { return {v.x, v.y, v.z}; }

    glm::vec3 SafeNormalize(const glm::vec3& v, const glm::vec3& fallback) {
        const float len = glm::length(v);
        return len > 1e-12f ? v / len : fallback;
    }
}

PatchModel BuildPatchModel(const std::vector<Vec3>& ctrl, int n) {
    const std::vector<Vec3> grid = bezierGrid(ctrl, n);          // iterative evaluation
    const int w = n + 1;

    std::vector<glm::vec3> P(w * w), N(w * w);
    std::vector<glm::vec2> UV(w * w);
    for (int i = 0; i <= n; i++)
        for (int j = 0; j <= n; j++) {
            const int i0 = std::max(i - 1, 0), i1 = std::min(i + 1, n);
            const int j0 = std::max(j - 1, 0), j1 = std::min(j + 1, n);
            const glm::vec3 du = ToGlm(grid[i1 * w + j]) - ToGlm(grid[i0 * w + j]);
            const glm::vec3 dv = ToGlm(grid[i * w + j1]) - ToGlm(grid[i * w + j0]);
            P[i * w + j] = ToGlm(grid[i * w + j]);
            N[i * w + j] = SafeNormalize(glm::cross(du, dv), glm::vec3(0, 0, 1));
            UV[i * w + j] = {float(i) / n, float(j) / n};
        }

    const auto tp = quadsToTriangles(P, n, n);
    const auto tn = quadsToTriangles(N, n, n);
    const auto tu = quadsToTriangles(UV, n, n);

    PatchModel out;
    out.triangleCount = tp.size() / 3;
    out.vertices.reserve(tp.size());
    for (size_t k = 0; k < tp.size(); k++) out.vertices.push_back({tp[k], tn[k], tu[k]});

    glm::vec3 lo = P[0], hi = P[0];
    for (const auto& p : P) { lo = glm::min(lo, p); hi = glm::max(hi, p); }
    out.center = 0.5f * (lo + hi);
    float r2 = 0.0f;
    for (const auto& p : P) r2 = std::max(r2, glm::dot(p - out.center, p - out.center));
    out.radius = std::max(std::sqrt(r2), 1e-6f);
    return out;
}
