#pragma once
#include <glad/gl.h>
#include "server_room/graphics/Instance.hpp"
#include <vector>
namespace server_room {
class CubeMesh {
public:
    CubeMesh();
    ~CubeMesh();
    CubeMesh(const CubeMesh&)=delete;
    CubeMesh& operator=(const CubeMesh&)=delete;
    void draw(GLuint instances, GLsizei count, bool frontQuads=false) const;
private:
    GLuint vao_=0,vbo_=0,ebo_=0;
};
class InstanceBatch {
public:
    explicit InstanceBatch(const std::vector<Instance>& instances, bool dynamic=false);
    ~InstanceBatch();
    InstanceBatch(const InstanceBatch&)=delete;
    InstanceBatch& operator=(const InstanceBatch&)=delete;
    void update(const std::vector<Instance>& instances);
    void draw(const CubeMesh& mesh, bool quads=false) const { mesh.draw(buffer_,count_,quads); }
private:
    GLuint buffer_=0;
    GLsizei count_=0;
};
}
