#include "modelData.h"
#include <algorithm>
#include <cmath>

namespace {
    glm::vec3 SafeNormalize(const glm::vec3& v, const glm::vec3& fallback) {
        const float len = glm::length(v);
        return len > 1e-12f ? v / len : fallback;
    }
}

ModelData BuildModelData(const SmfModel& smf) {
    ModelData out;
    const auto& V = smf.vertices;
    const auto& T = smf.triangles;
    out.triangleCount = T.size();

    std::vector<glm::vec3> faceNormal(T.size());
    for (size_t t = 0; t < T.size(); ++t) {
        const glm::vec3& a = V[T[t].x];
        const glm::vec3& b = V[T[t].y];
        const glm::vec3& c = V[T[t].z];
        faceNormal[t] = SafeNormalize(glm::cross(b - a, c - a), glm::vec3(0.0f));
    }

    std::vector<glm::vec3> vertexNormal(V.size(), glm::vec3(0.0f));
    for (size_t t = 0; t < T.size(); ++t) {
        vertexNormal[T[t].x] += faceNormal[t];
        vertexNormal[T[t].y] += faceNormal[t];
        vertexNormal[T[t].z] += faceNormal[t];
    }
    for (auto& n : vertexNormal) n = SafeNormalize(n, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec3 bmin = V[0], bmax = V[0];
    for (const auto& p : V) { bmin = glm::min(bmin, p); bmax = glm::max(bmax, p); }
    out.center = 0.5f * (bmin + bmax);
    float r2 = 0.0f;
    for (const auto& p : V) r2 = std::max(r2, glm::dot(p - out.center, p - out.center));
    out.radius = std::max(std::sqrt(r2), 1e-6f);

    out.vertices.reserve(T.size() * 3);
    for (size_t t = 0; t < T.size(); ++t) {
        for (int k = 0; k < 3; ++k) {
            const unsigned i = T[t][k];
            out.vertices.push_back({V[i], faceNormal[t], vertexNormal[i]});
        }
    }
    return out;
}
