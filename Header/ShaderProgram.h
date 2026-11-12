// Compiles and links a GLSL program from two files and sets uniforms.
#pragma once
#include <glad/glad.h>
#include <string>
#include "MathUtils.h"

class ShaderProgram {
public:
    ShaderProgram() = default;
    ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);

    void use() const { glUseProgram(id_); }
    GLuint id() const { return id_; }

    void setMat4(const char* name, const Mat4& m) const;
    void setVec3(const char* name, Vec3 v) const;
    void setFloat(const char* name, float x) const;
    void setInt(const char* name, int x) const;

private:
    GLuint id_ = 0;
    GLint loc(const char* name) const { return glGetUniformLocation(id_, name); }
};
