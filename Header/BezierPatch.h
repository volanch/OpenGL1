#pragma once
#include <vector>
#include "MathUtils.h"

std::vector<Vec3> defaultControlPoints();
std::vector<Vec3> loadControlPoints(const char* path = "control_points.txt");

void bernstein(float t, float b[4]);
std::vector<Vec3> bezierGrid(const std::vector<Vec3>& ctrl, int n);   // (n+1)^2 points, index i*(n+1)+j
