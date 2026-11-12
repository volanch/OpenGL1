#pragma once
#include <glad/glad.h>

class Framebuffer {
public:
    void create(int width, int height, int samples);   // samples = 0 -> single-sampled
    void destroy();
    void bind() const { glBindFramebuffer(GL_FRAMEBUFFER, fbo_); }
    void bindForRead() const { glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_); }
    static void bindDefault() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

private:
    GLuint fbo_ = 0, rb_[2] = {0, 0};
};
