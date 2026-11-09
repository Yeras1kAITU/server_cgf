#include "server_room/graphics/Shader.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace server_room {
namespace {
GLuint compile(GLenum kind, const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Cannot open shader: " + path.string());
    std::ostringstream buffer;
    buffer << file.rdbuf();
    const std::string source = buffer.str();
    const char* text = source.c_str();
    const GLuint shader = glCreateShader(kind);
    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);
    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[4096] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        glDeleteShader(shader);
        throw std::runtime_error(path.string() + ": " + log);
    }
    return shader;
}
}

Shader::Shader(const std::filesystem::path& vertex, const std::filesystem::path& fragment) {
    const GLuint vs = compile(GL_VERTEX_SHADER, vertex);
    GLuint fs = 0;
    try { fs = compile(GL_FRAGMENT_SHADER, fragment); }
    catch (...) { glDeleteShader(vs); throw; }
    program_ = glCreateProgram();
    glAttachShader(program_, vs);
    glAttachShader(program_, fs);
    glLinkProgram(program_);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint success = GL_FALSE;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success) {
        char log[4096] = {};
        glGetProgramInfoLog(program_, sizeof(log), nullptr, log);
        glDeleteProgram(program_);
        program_ = 0;
        throw std::runtime_error(std::string("Shader link failed: ") + log);
    }
}
Shader::~Shader() { if (program_) glDeleteProgram(program_); }
void Shader::use() const { glUseProgram(program_); }
void Shader::matrix(const char* name, const Mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(program_, name), 1, GL_FALSE, value.data);
}
void Shader::vector(const char* name, Vec3 v) const {
    glUniform3f(glGetUniformLocation(program_, name), v.x, v.y, v.z);
}
void Shader::scalar(const char* name, float v) const {
    glUniform1f(glGetUniformLocation(program_, name), v);
}
} // namespace server_room
