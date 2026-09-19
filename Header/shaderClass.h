//
// Created by vladk on 14.09.2026.
//

#ifndef COMPUTERGRAPHICSPROJECT_SHADERCLASS_H
#define COMPUTERGRAPHICSPROJECT_SHADERCLASS_H
#include<glad/glad.h>
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
};

#endif //COMPUTERGRAPHICSPROJECT_SHADERCLASS_H
