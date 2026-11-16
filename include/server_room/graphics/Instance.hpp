#pragma once
#include "server_room/graphics/Math.hpp"
namespace server_room {
enum Material { Metal=0, Floor=1, Led=2, CeilingLight=3, Monitor=4 };
struct Instance {
    Mat4 model;
    float colorKind[4];
    float parameters[4]; // stable blink seed, UV repeat X/Y, reserved
};
inline Instance instance(const Mat4& parent, Vec3 p, Vec3 size, Vec3 color,
                         Material kind=Metal, float seed=0, float u=1, float v=1) {
    return {parent * modelMatrix(p,size), {color.x,color.y,color.z,float(kind)}, {seed,u,v,0}};
}
}
