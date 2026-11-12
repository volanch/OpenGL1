#pragma once
#include <vector>

void writeTorusSmf(const char* path, int nu = 64, int nv = 32, float R = 1.0f, float r = 0.4f);

std::vector<float> loadSmf(const char* path);
