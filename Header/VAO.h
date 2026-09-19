//
// Created by vladk on 14.09.2026.
//

#ifndef COMPUTERGRAPHICSPROJECT_VAO_H
#define COMPUTERGRAPHICSPROJECT_VAO_H
#include<glad/glad.h>
#include"VBO.h"

class VAO {
public:
    GLuint ID;
    VAO();

    void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, void* offset);
    void LinkVBO(VBO& VBO, GLuint layout, GLuint numComponents = 3);
    void Bind();
    void Unbind();
    void Delete();
};
#endif //COMPUTERGRAPHICSPROJECT_VAO_H
