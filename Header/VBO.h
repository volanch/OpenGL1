//
// Created by vladk on 14.09.2026.
//

#ifndef COMPUTERGRAPHICSPROJECT_VBO_H
#define COMPUTERGRAPHICSPROJECT_VBO_H
#include<glad/glad.h>

class VBO {
public:
    GLuint ID;
    VBO(GLfloat* vertices, GLsizeiptr size);

    void Bind();
    void Unbind();
    void Delete();
};

#endif //COMPUTERGRAPHICSPROJECT_VBO_H
