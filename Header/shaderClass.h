//
// Created by vladk on 14.09.2026.
//

#ifndef COMPUTERGRAPHICSPROJECT_SHADERCLASS_H
#define COMPUTERGRAPHICSPROJECT_SHADERCLASS_H
#include<glad/glad.h>
#include<glm/glm.hpp>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader {
public:
    GLuint ID;
    Shader(const char* vertexFile, const char* fragmentFile);

    void Activate();
    void Delete();
    void SetVec3(const char* name, float x, float y, float z);
    void SetMat4(const char* name, const glm::mat4& m);
    void SetVec3(const char* name, const glm::vec3& v);
    void SetVec4(const char* name, const glm::vec4& v);
    void SetMat3(const char* name, const glm::mat3& m);
    void SetFloat(const char* name, float value);
    void SetInt(const char* name, int value);
};

#endif //COMPUTERGRAPHICSPROJECT_SHADERCLASS_H
