#include "server_room/graphics/CubeMesh.hpp"
#include <array>
#include <cstddef>
namespace server_room {
CubeMesh::CubeMesh() {
    // One indexed cube; hard face normals, explicit non-stretched face UVs.
    const Vec3 normals[]={{0,0,1},{0,0,-1},{-1,0,0},{1,0,0},{0,1,0},{0,-1,0}};
    const Vec3 points[][4]={
        {{-.5f,-.5f,.5f},{.5f,-.5f,.5f},{.5f,.5f,.5f},{-.5f,.5f,.5f}},
        {{.5f,-.5f,-.5f},{-.5f,-.5f,-.5f},{-.5f,.5f,-.5f},{.5f,.5f,-.5f}},
        {{-.5f,-.5f,-.5f},{-.5f,-.5f,.5f},{-.5f,.5f,.5f},{-.5f,.5f,-.5f}},
        {{.5f,-.5f,.5f},{.5f,-.5f,-.5f},{.5f,.5f,-.5f},{.5f,.5f,.5f}},
        {{-.5f,.5f,.5f},{.5f,.5f,.5f},{.5f,.5f,-.5f},{-.5f,.5f,-.5f}},
        {{-.5f,-.5f,-.5f},{.5f,-.5f,-.5f},{.5f,-.5f,.5f},{-.5f,-.5f,.5f}}
    };
    const float uv[][2]={{0,0},{1,0},{1,1},{0,1}};
    std::array<float,24*8> vertices{};
    std::array<GLuint,36> indices{};
    for(int f=0;f<6;++f) {
        for(int j=0;j<4;++j) {
            const int k=(f*4+j)*8;
            vertices[k]=points[f][j].x;vertices[k+1]=points[f][j].y;vertices[k+2]=points[f][j].z;
            vertices[k+3]=normals[f].x;vertices[k+4]=normals[f].y;vertices[k+5]=normals[f].z;
            vertices[k+6]=uv[j][0];vertices[k+7]=uv[j][1];
        }
        const GLuint pattern[]={0,1,2,2,3,0};
        for(int j=0;j<6;++j) indices[f*6+j]=GLuint(f*4)+pattern[j];
    }
    glGenVertexArrays(1,&vao_);glGenBuffers(1,&vbo_);glGenBuffers(1,&ebo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices.data(),GL_STATIC_DRAW);
    for(GLuint a=0;a<3;++a) {
        const size_t offset=a==0?0:(a==1?3:6);
        glVertexAttribPointer(a,a==2?2:3,GL_FLOAT,GL_FALSE,8*sizeof(float),reinterpret_cast<void*>(offset*sizeof(float)));
        glEnableVertexAttribArray(a);
    }
    glBindVertexArray(0);
}
CubeMesh::~CubeMesh() { glDeleteBuffers(1,&ebo_);glDeleteBuffers(1,&vbo_);glDeleteVertexArrays(1,&vao_); }
void CubeMesh::draw(GLuint instances,GLsizei count,bool quads) const {
    if(!count) return;
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER,instances);
    for(GLuint a=0;a<6;++a) {
        const GLuint attribute=a+3;
        glVertexAttribPointer(attribute,4,GL_FLOAT,GL_FALSE,sizeof(Instance),reinterpret_cast<void*>(size_t(a)*4*sizeof(float)));
        glEnableVertexAttribArray(attribute);
        glVertexAttribDivisor(attribute,1);
    }
    glDrawElementsInstanced(GL_TRIANGLES,quads?6:36,GL_UNSIGNED_INT,nullptr,count);
}
InstanceBatch::InstanceBatch(const std::vector<Instance>& instances,bool dynamic):count_(GLsizei(instances.size())) {
    glGenBuffers(1,&buffer_);glBindBuffer(GL_ARRAY_BUFFER,buffer_);
    glBufferData(GL_ARRAY_BUFFER,GLsizeiptr(instances.size()*sizeof(Instance)),instances.data(),dynamic?GL_DYNAMIC_DRAW:GL_STATIC_DRAW);
}
InstanceBatch::~InstanceBatch() { glDeleteBuffers(1,&buffer_); }
void InstanceBatch::update(const std::vector<Instance>& instances) {
    glBindBuffer(GL_ARRAY_BUFFER,buffer_);
    if(count_!=GLsizei(instances.size())) {
        glBufferData(GL_ARRAY_BUFFER,GLsizeiptr(instances.size()*sizeof(Instance)),instances.data(),GL_DYNAMIC_DRAW);
        count_=GLsizei(instances.size());
    } else glBufferSubData(GL_ARRAY_BUFFER,0,GLsizeiptr(instances.size()*sizeof(Instance)),instances.data());
}
}
