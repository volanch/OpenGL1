//
// Created by vladk on 14.09.2026.
//

#ifndef COMPUTERGRAPHICSPROJECT_EBO_H
#define COMPUTERGRAPHICSPROJECT_EBO_H
#include<glad/glad.h>

class EBO {
public:
    GLuint ID;
    EBO(GLuint* indices, GLsizeiptr size);

    void Bind();
    void Unbind();
    void Delete();
};

#endif //COMPUTERGRAPHICSPROJECT_EBO_H
