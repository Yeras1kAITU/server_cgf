#include "server_room/graphics/CubeMesh.hpp"
#include <array>

namespace server_room {
CubeMesh::CubeMesh() {
    const float vertices[] = {
        -.5f,-.5f,.5f, 0,0,1,  .5f,-.5f,.5f, 0,0,1,
         .5f,.5f,.5f, 0,0,1,  -.5f,.5f,.5f, 0,0,1,
         .5f,-.5f,-.5f, 0,0,-1,  -.5f,-.5f,-.5f, 0,0,-1,
        -.5f,.5f,-.5f, 0,0,-1,  .5f,.5f,-.5f, 0,0,-1,
        -.5f,-.5f,-.5f, -1,0,0,  -.5f,-.5f,.5f, -1,0,0,
        -.5f,.5f,.5f, -1,0,0,  -.5f,.5f,-.5f, -1,0,0,
         .5f,-.5f,.5f, 1,0,0,  .5f,-.5f,-.5f, 1,0,0,
         .5f,.5f,-.5f, 1,0,0,  .5f,.5f,.5f, 1,0,0,
        -.5f,.5f,.5f, 0,1,0,  .5f,.5f,.5f, 0,1,0,
         .5f,.5f,-.5f, 0,1,0,  -.5f,.5f,-.5f, 0,1,0,
        -.5f,-.5f,-.5f, 0,-1,0,  .5f,-.5f,-.5f, 0,-1,0,
         .5f,-.5f,.5f, 0,-1,0,  -.5f,-.5f,.5f, 0,-1,0
    };
    std::array<GLuint, 36> indices{};
    for (GLuint face = 0; face < 6; ++face) {
        const GLuint pattern[] = {0, 1, 2, 2, 3, 0};
        for (GLuint i = 0; i < 6; ++i) indices[face*6+i] = face*4 + pattern[i];
    }
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), reinterpret_cast<void*>(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}
CubeMesh::~CubeMesh() {
    glDeleteBuffers(1, &ebo_);
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
}
void CubeMesh::draw() const {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}
}
