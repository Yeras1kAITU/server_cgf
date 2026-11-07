#pragma once

#include <glad/gl.h>

namespace server_room {
class CubeMesh {
public:
    CubeMesh();
    ~CubeMesh();
    CubeMesh(const CubeMesh&) = delete;
    CubeMesh& operator=(const CubeMesh&) = delete;
    void draw() const;
private:
    GLuint vao_ = 0, vbo_ = 0, ebo_ = 0;
};
}
