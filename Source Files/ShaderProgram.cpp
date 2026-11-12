#include "ShaderProgram.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace {
std::string readFile(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) { std::printf("Cannot open shader file %s\n", path.c_str()); std::exit(1); }
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

GLuint compile(GLenum kind, const std::string& src, const std::string& name) {
    GLuint s = glCreateShader(kind);
    const char* p = src.c_str();
    glShaderSource(s, 1, &p, nullptr);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(s, 2048, nullptr, log);
        std::printf("Shader error in %s:\n%s\n", name.c_str(), log);
        std::exit(1);
    }
    return s;
}
}  // namespace

ShaderProgram::ShaderProgram(const std::string& vsPath, const std::string& fsPath) {
    GLuint vs = compile(GL_VERTEX_SHADER, readFile(vsPath), vsPath);
    GLuint fs = compile(GL_FRAGMENT_SHADER, readFile(fsPath), fsPath);
    id_ = glCreateProgram();
    glAttachShader(id_, vs);
    glAttachShader(id_, fs);
    glLinkProgram(id_);
    GLint ok;
    glGetProgramiv(id_, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetProgramInfoLog(id_, 2048, nullptr, log);
        std::printf("Link error:\n%s\n", log);
        std::exit(1);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void ShaderProgram::setMat4(const char* n, const Mat4& m) const { glUniformMatrix4fv(loc(n), 1, GL_FALSE, m.m); }
void ShaderProgram::setVec3(const char* n, Vec3 v) const { glUniform3f(loc(n), v.x, v.y, v.z); }
void ShaderProgram::setFloat(const char* n, float x) const { glUniform1f(loc(n), x); }
void ShaderProgram::setInt(const char* n, int x) const { glUniform1i(loc(n), x); }
