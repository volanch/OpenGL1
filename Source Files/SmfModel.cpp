#include "SmfModel.h"
#include "MathUtils.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

void writeTorusSmf(const char* path, int nu, int nv, float R, float r) {
    std::ofstream f(path);
    for (int i = 0; i < nu; i++)
        for (int j = 0; j < nv; j++) {
            float t = 6.2831853f * i / nu, p = 6.2831853f * j / nv;
            f << "v " << (R + r * std::cos(p)) * std::cos(t) << " " << r * std::sin(p) << " "
              << (R + r * std::cos(p)) * std::sin(t) << "\n";
        }
    for (int i = 0; i < nu; i++)
        for (int j = 0; j < nv; j++) {
            int a = i * nv + j, b = ((i + 1) % nu) * nv + j;
            int c = ((i + 1) % nu) * nv + (j + 1) % nv, d = i * nv + (j + 1) % nv;
            f << "f " << a + 1 << " " << d + 1 << " " << c + 1 << "\nf " << a + 1 << " " << c + 1 << " " << b + 1 << "\n";
        }
}

std::vector<float> loadSmf(const char* path) {
    std::ifstream in(path);
    if (!in) { std::printf("Cannot open %s\n", path); std::exit(1); }
    std::vector<Vec3> V;
    std::vector<int> F;
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream ss(line);
        std::string tag; ss >> tag;
        if (tag == "v") { Vec3 v; ss >> v.x >> v.y >> v.z; V.push_back(v); }
        else if (tag == "f") for (int k = 0; k < 3; k++) { std::string t; ss >> t; F.push_back(std::atoi(t.c_str()) - 1); }
    }
    Vec3 lo = V[0], hi = V[0];
    for (Vec3 v : V) {
        lo = {std::fmin(lo.x, v.x), std::fmin(lo.y, v.y), std::fmin(lo.z, v.z)};
        hi = {std::fmax(hi.x, v.x), std::fmax(hi.y, v.y), std::fmax(hi.z, v.z)};
    }
    float size = std::fmax(hi.x - lo.x, std::fmax(hi.y - lo.y, hi.z - lo.z));
    for (Vec3& v : V) v = (v - (lo + hi) * 0.5f) * (2.5f / size);
    std::vector<Vec3> N(V.size(), Vec3{0, 0, 0});
    for (size_t t = 0; t + 2 < F.size(); t += 3) {
        Vec3 fn = cross(V[F[t + 1]] - V[F[t]], V[F[t + 2]] - V[F[t]]);   // area-weighted
        for (int k = 0; k < 3; k++) N[F[t + k]] = N[F[t + k]] + fn;
    }
    std::vector<float> data;
    for (int idx : F) { push(data, V[idx]); push(data, normalize(N[idx])); }
    return data;
}
