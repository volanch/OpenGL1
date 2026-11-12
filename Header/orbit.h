#ifndef COMPUTERGRAPHICSASSIGNMENT3_ORBIT_H
#define COMPUTERGRAPHICSASSIGNMENT3_ORBIT_H
#pragma once
#include <cmath>
#include <glm/glm.hpp>

struct Orbit {
    float angle;
    float radius;
    float height;
};


inline glm::vec3 CylindricalToCartesian(const Orbit& o, bool zUp) {
    const float x = o.radius * std::cos(o.angle);
    const float s = o.radius * std::sin(o.angle);
    return zUp ? glm::vec3(x, s, o.height) : glm::vec3(x, o.height, s);
}

#endif //COMPUTERGRAPHICSASSIGNMENT3_ORBIT_H
