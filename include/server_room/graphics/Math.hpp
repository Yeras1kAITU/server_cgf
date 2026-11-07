#pragma once

#include <cmath>

namespace server_room {

struct Vec3 {
    float x, y, z;
    Vec3 operator+(Vec3 b) const { return {x + b.x, y + b.y, z + b.z}; }
    Vec3 operator-(Vec3 b) const { return {x - b.x, y - b.y, z - b.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
};

inline float dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
inline Vec3 cross(Vec3 a, Vec3 b) {
    return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}
inline Vec3 normalized(Vec3 v) {
    const float length = std::sqrt(dot(v, v));
    return length > 0.00001f ? v * (1.0f / length) : Vec3{0, 0, 0};
}
inline float radians(float degrees) { return degrees * 0.01745329252f; }

// Column-major matrices, matching GLSL's default layout.
struct Mat4 { float data[16] = {}; };

inline Mat4 modelMatrix(Vec3 center, Vec3 size) {
    Mat4 m;
    m.data[0] = size.x; m.data[5] = size.y; m.data[10] = size.z;
    m.data[12] = center.x; m.data[13] = center.y; m.data[14] = center.z;
    m.data[15] = 1;
    return m;
}
inline Mat4 perspective(float aspect) {
    const float nearPlane = 0.1f, farPlane = 60.0f;
    const float f = 1.0f / std::tan(radians(65.0f) / 2.0f);
    Mat4 m;
    m.data[0] = f / aspect; m.data[5] = f;
    m.data[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    m.data[11] = -1;
    m.data[14] = -2 * farPlane * nearPlane / (farPlane - nearPlane);
    return m;
}
inline Mat4 lookAt(Vec3 eye, Vec3 direction) {
    const Vec3 f = normalized(direction);
    const Vec3 s = normalized(cross(f, {0, 1, 0}));
    const Vec3 u = cross(s, f);
    Mat4 m;
    m.data[0] = s.x; m.data[4] = s.y; m.data[8] = s.z;
    m.data[1] = u.x; m.data[5] = u.y; m.data[9] = u.z;
    m.data[2] = -f.x; m.data[6] = -f.y; m.data[10] = -f.z;
    m.data[12] = -dot(s, eye); m.data[13] = -dot(u, eye);
    m.data[14] = dot(f, eye); m.data[15] = 1;
    return m;
}

}
