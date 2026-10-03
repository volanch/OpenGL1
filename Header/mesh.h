//
// Created by vladk on 02.10.2026.
//

#ifndef COMPUTERGRAPHICSASSIGNMENT2_MESH_H
#define COMPUTERGRAPHICSASSIGNMENT2_MESH_H
#pragma once
#include <vector>
#include "VAO.h"
#include "VBO.h"
#include "shapes2d.h"

struct Mesh {
    VAO vao;
    VBO vbo;
    GLenum mode;
    GLsizei count;

    Mesh(std::vector<Vertex2D> v, GLenum drawMode)
        : vbo(reinterpret_cast<GLfloat*>(v.data()), v.size() * sizeof(Vertex2D)),
          mode(drawMode), count((GLsizei)v.size()) {
        vao.Bind();
        vao.LinkVBO(vbo, 0, 2);
        vao.Unbind();
    }
    void Draw() { vao.Bind(); glDrawArrays(mode, 0, count); }
};
#endif //COMPUTERGRAPHICSASSIGNMENT2_MESH_H
