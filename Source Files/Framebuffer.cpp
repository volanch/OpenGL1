#include "Framebuffer.h"
#include <cstdio>

void Framebuffer::create(int w, int h, int samples) {
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glGenRenderbuffers(2, rb_);
    const GLenum fmt[2] = {GL_RGBA8, GL_DEPTH_COMPONENT24};
    const GLenum attach[2] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
    for (int i = 0; i < 2; i++) {
        glBindRenderbuffer(GL_RENDERBUFFER, rb_[i]);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, fmt[i], w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attach[i], GL_RENDERBUFFER, rb_[i]);
    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) std::printf("FBO incomplete\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::destroy() {
    if (fbo_) { glDeleteFramebuffers(1, &fbo_); glDeleteRenderbuffers(2, rb_); }
    fbo_ = 0; rb_[0] = rb_[1] = 0;
}
