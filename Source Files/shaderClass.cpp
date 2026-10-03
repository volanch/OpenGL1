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

Shader::Shader(const char* vertexFile, const char* fragmentFile) {
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    ID = glCreateProgram();

    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

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