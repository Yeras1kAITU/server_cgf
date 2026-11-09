#pragma once

#include <glad/gl.h>
#include "server_room/graphics/Math.hpp"
#include <filesystem>

namespace server_room {

class Shader {
public:
    Shader(const std::filesystem::path& vertex, const std::filesystem::path& fragment);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    void use() const;
    void matrix(const char* name, const Mat4& value) const;
    void vector(const char* name, Vec3 value) const;
    void scalar(const char* name, float value) const;
private:
    GLuint program_ = 0;
};

} // namespace server_room
