// Orbit camera that always looks at a target point.
#pragma once
#include <glad/glad.h>
#include <GL/freeglut.h>
#include "MathUtils.h"

class OrbitCamera {
public:
    void init(Vec3 target, float dist, float yaw, float pitch, int rotateButton = GLUT_LEFT_BUTTON);
    void reset();
    void zoom(float factor);

    Vec3 eye() const;
    Vec3 up() const { return {0, 1, 0}; }
    Mat4 view() const { return lookAt(eye(), target_, up()); }

    // Input handlers.
    void mouse(int button, int state, int x, int y);
    bool motion(int x, int y);   // true if the view changed
    bool special(int key);       // true if the view changed

private:
    void rotate(float dyaw, float dpitch);

    Vec3 target_ = {0, 0, 0};
    float dist_ = 8, yaw_ = 0, pitch_ = 0.35f;
    float dist0_ = 8, yaw0_ = 0, pitch0_ = 0.35f;
    int rotateButton_ = GLUT_LEFT_BUTTON;
    int lastX_ = 0, lastY_ = 0;
    bool dragging_ = false;
};
