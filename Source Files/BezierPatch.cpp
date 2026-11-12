#include "BezierPatch.h"
#include <cstdio>
#include <fstream>

std::vector<Vec3> defaultControlPoints() {
    return {{0, 0, 0}, {1, 0, 1}, {2, 0, 1}, {3, 0, 0},
            {0, 1, 1}, {1, 1, 2}, {2, 1, 2}, {3, 1, 1},
            {0, 2, 1}, {1, 2, 2}, {2, 2, 2}, {3, 2, 1},
            {0, 3, 0}, {1, 3, 1}, {2, 3, 1}, {3, 3, 0}};
}

std::vector<Vec3> loadControlPoints(const char* path) {
    std::ifstream f(path);
    std::vector<Vec3> p;
    Vec3 v;
    while (f >> v.x >> v.y >> v.z) p.push_back(v);
    if (p.size() == 16) { std::printf("Loaded control points from %s\n", path); return p; }
    return defaultControlPoints();
}

void bernstein(float t, float b[4]) {
    float s = 1 - t;
    b[0] = s * s * s; b[1] = 3 * t * s * s; b[2] = 3 * t * t * s; b[3] = t * t * t;
}

std::vector<Vec3> bezierGrid(const std::vector<Vec3>& ctrl, int n) {
    std::vector<Vec3> V((n + 1) * (n + 1));
    for (int i = 0; i <= n; i++) {
        float bu[4]; bernstein((float)i / n, bu);
        for (int j = 0; j <= n; j++) {
            float bv[4]; bernstein((float)j / n, bv);
            Vec3 p = {0, 0, 0};
            for (int r = 0; r < 4; r++)
                for (int c = 0; c < 4; c++) p = p + ctrl[r * 4 + c] * (bv[r] * bu[c]);
            V[i * (n + 1) + j] = p;
        }
    }
    return V;
}
