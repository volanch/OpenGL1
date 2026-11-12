#pragma once
#include <vector>

template <class T>
std::vector<T> quadsToTriangles(const std::vector<T>& G, int nu, int nv) {
    std::vector<T> out;
    out.reserve(nu * nv * 6);
    for (int i = 0; i < nu; i++)
        for (int j = 0; j < nv; j++) {
            T a = G[i * (nv + 1) + j], b = G[(i + 1) * (nv + 1) + j];
            T c = G[(i + 1) * (nv + 1) + j + 1], d = G[i * (nv + 1) + j + 1];
            out.push_back(a); out.push_back(b); out.push_back(c);
            out.push_back(a); out.push_back(c); out.push_back(d);
        }
    return out;
}
