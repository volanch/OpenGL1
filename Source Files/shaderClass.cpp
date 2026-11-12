#include"shaderClass.h"
#include<glm/gtc/type_ptr.hpp>

std::string get_file_contents(const char* filename) {
    std::ifstream in(filename, std::ios::binary);
    if (in) {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}

namespace {
    void CheckShader(GLuint shader, const char* label) {
        GLint ok = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (ok != GL_TRUE) {
            char log[2048];
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            std::cerr << "Shader compile error (" << label << "):\n" << log << std::endl;
        }
    }
}

Shader::Shader(const char* vertexFile, const char* fragmentFile) {
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    CheckShader(vertexShader, vertexFile);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    CheckShader(fragmentShader, fragmentFile);

    ID = glCreateProgram();

    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    GLint linked = GL_FALSE;
    glGetProgramiv(ID, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        char log[2048];
        glGetProgramInfoLog(ID, sizeof(log), nullptr, log);
        std::cerr << "Shader link error:\n" << log << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Activate() {
    glUseProgram(ID);
}

void Shader::Delete() {
    glDeleteProgram(ID);
}

void Shader::SetVec3(const char* name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void Shader::SetMat4(const char* name, const glm::mat4& m) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(m));
}
void Shader::SetVec3(const char* name, const glm::vec3& v) {
    glUniform3f(glGetUniformLocation(ID, name), v.x, v.y, v.z);
}

void Shader::SetVec4(const char* name, const glm::vec4& v) {
    glUniform4f(glGetUniformLocation(ID, name), v.x, v.y, v.z, v.w);
}

void Shader::SetMat3(const char* name, const glm::mat3& m) {
    glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::SetFloat(const char* name, float value) {
    glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::SetInt(const char* name, int value) {
    glUniform1i(glGetUniformLocation(ID, name), value);
}
