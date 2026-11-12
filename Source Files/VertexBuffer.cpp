#include "VertexBuffer.h"
#include <cstddef>

void VertexBuffer::init(const std::vector<int>& sizes) {
    stride_ = 0;
    for (int s : sizes) stride_ += s;
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    int offset = 0;
    for (int i = 0; i < (int)sizes.size(); i++) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, sizes[i], GL_FLOAT, GL_FALSE, stride_ * sizeof(float),
                              (void*)(size_t)(offset * sizeof(float)));
        offset += sizes[i];
    }
}

void VertexBuffer::set(const std::vector<float>& d) {
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, d.size() * sizeof(float), d.data(), GL_DYNAMIC_DRAW);
    count_ = (int)d.size() / stride_;
}

void VertexBuffer::draw(GLenum mode, int first, int n) const {
    glBindVertexArray(vao_);
    glDrawArrays(mode, first, n < 0 ? count_ : n);
}
