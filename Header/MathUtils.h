#pragma once
#include <cmath>
#include <vector>

struct Vec2 { float x, y; };
struct Vec3 { float x, y, z; };

inline Vec3 operator+(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline Vec3 operator-(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline Vec3 operator*(Vec3 a, float s) { return {a.x * s, a.y * s, a.z * s}; }
inline float dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline Vec3 cross(Vec3 a, Vec3 b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
inline Vec3 normalize(Vec3 a) { float l = std::sqrt(dot(a, a)) + 1e-12f; return a * (1.0f / l); }
inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
inline void push(std::vector<float>& v, Vec3 a) { v.push_back(a.x); v.push_back(a.y); v.push_back(a.z); }

struct Mat4 { float m[16]; };

Mat4 identity();
Mat4 operator*(Mat4 a, Mat4 b);
Mat4 perspective(float fovyDeg, float aspect, float nearZ, float farZ);
Mat4 lookAt(Vec3 eye, Vec3 target, Vec3 up);
Mat4 translate(Vec3 t);
