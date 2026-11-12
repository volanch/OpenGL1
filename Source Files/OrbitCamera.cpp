#include "OrbitCamera.h"

void OrbitCamera::init(Vec3 t, float d, float y, float p, int button) {
    target_ = t;
    dist_ = dist0_ = d;
    yaw_ = yaw0_ = y;
    pitch_ = pitch0_ = p;
    rotateButton_ = button;
}

void OrbitCamera::reset() { dist_ = dist0_; yaw_ = yaw0_; pitch_ = pitch0_; }
void OrbitCamera::zoom(float f) { dist_ = clampf(dist_ * f, 1.0f, 100.0f); }
void OrbitCamera::rotate(float dy, float dp) { yaw_ += dy; pitch_ = clampf(pitch_ + dp, -1.5f, 1.5f); }

Vec3 OrbitCamera::eye() const {
    return target_ + Vec3{std::cos(pitch_) * std::sin(yaw_), std::sin(pitch_), std::cos(pitch_) * std::cos(yaw_)} * dist_;
}

void OrbitCamera::mouse(int button, int state, int x, int y) {
    if (button == rotateButton_) { dragging_ = (state == GLUT_DOWN); lastX_ = x; lastY_ = y; }
}

bool OrbitCamera::motion(int x, int y) {
    if (!dragging_) return false;
    rotate(-(x - lastX_) * 0.01f, (y - lastY_) * 0.01f);
    lastX_ = x; lastY_ = y;
    return true;
}

bool OrbitCamera::special(int key) {
    const float s = 0.08f;
    switch (key) {
        case GLUT_KEY_LEFT:  rotate(-s, 0); return true;
        case GLUT_KEY_RIGHT: rotate(s, 0);  return true;
        case GLUT_KEY_UP:    rotate(0, s);  return true;
        case GLUT_KEY_DOWN:  rotate(0, -s); return true;
    }
    return false;
}
