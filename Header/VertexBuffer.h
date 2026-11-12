// VAO + VBO holding interleaved float attributes.
#pragma once
#include <glad/glad.h>
#include <vector>

class VertexBuffer {
public:
    // attributeSizes e.g. {3, 3} = position + colour, {3, 3, 2} = pos + normal + uv
    void init(const std::vector<int>& attributeSizes);
    void set(const std::vector<float>& data);                 // upload (dynamic)
    void draw(GLenum mode, int first = 0, int count = -1) const;

private:
    GLuint vao_ = 0, vbo_ = 0;
    int stride_ = 0, count_ = 0;
};
