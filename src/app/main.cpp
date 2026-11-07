#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "server_room/graphics/CubeMesh.hpp"
#include "server_room/graphics/Shader.hpp"
#include "server_room/input/Camera.hpp"
#include "server_room/scene/Room.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using namespace server_room;
struct Input {
    Camera camera;
    bool captured = true, firstMouse = true;
    double lastX = 0, lastY = 0;
};
void capture(GLFWwindow* window, bool enabled) {
    auto& input = *static_cast<Input*>(glfwGetWindowUserPointer(window));
    input.captured = enabled;
    input.firstMouse = true;
    glfwSetInputMode(window, GLFW_CURSOR, enabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
void cursor(GLFWwindow* window, double x, double y) {
    auto& input = *static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (!input.captured) return;
    if (!input.firstMouse) input.camera.look(float(x-input.lastX), float(y-input.lastY));
    input.lastX = x; input.lastY = y; input.firstMouse = false;
}
void key(GLFWwindow* window, int code, int, int action, int) {
    if (action != GLFW_PRESS) return;
    if (code == GLFW_KEY_ESCAPE) capture(window, false);
    if (code == GLFW_KEY_Q) glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (code == GLFW_KEY_R) {
        auto& input = *static_cast<Input*>(glfwGetWindowUserPointer(window));
        input.camera = Camera{};
        input.firstMouse = true;
    }
}
void mouse(GLFWwindow* window, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) capture(window, true);
}
void focus(GLFWwindow* window, int focused) {
    if (!focused) capture(window, false);
}
void screenshot(const std::filesystem::path& path, int width, int height) {
    std::vector<unsigned char> pixels(size_t(width)*height*3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    std::ofstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot write screenshot: " + path.string());
    file << "P6\n" << width << ' ' << height << "\n255\n";
    for (int y = height-1; y >= 0; --y)
        file.write(reinterpret_cast<const char*>(pixels.data()+size_t(y)*width*3), width*3);
    if (!file) throw std::runtime_error("Screenshot write failed");
}
void run(GLFWwindow* window, const std::filesystem::path& shaderFolder,
         bool smokeTest, const std::filesystem::path& capturePath) {
    Input input;
    RoomConfig config;
    glfwSetWindowUserPointer(window, &input);
    glfwSetCursorPosCallback(window, cursor);
    glfwSetKeyCallback(window, key);
    glfwSetMouseButtonCallback(window, mouse);
    glfwSetWindowFocusCallback(window, focus);
    if (!smokeTest) capture(window, true);
    Shader shader(shaderFolder / "room.vert", shaderFolder / "room.frag");
    CubeMesh cube;
    const auto room = makeRoom(config);
    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(smokeTest ? 0 : 1);
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << '\n'
              << "Renderer: " << glGetString(GL_RENDERER) << '\n'
              << "WASD: walk | Mouse: look | Esc: release mouse | Click: capture\n"
              << "R: reset view | Q: quit\n";
    double previous = glfwGetTime(), titleTime = previous;
    int frames = 0, totalFrames = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        const double now = glfwGetTime();
        const float dt = float(std::clamp(now-previous, 0.0, 0.05));
        previous = now;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (width == 0 || height == 0) { glfwWaitEventsTimeout(0.1); continue; }
        if (input.captured && glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
            const float forward = float(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                                - float(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
            const float right = float(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                              - float(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
            input.camera.move(forward, right, dt, config.width, config.depth);
        }
        glViewport(0, 0, width, height);
        glClearColor(.025f, .045f, .075f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.matrix("uView", lookAt(input.camera.position, input.camera.direction()));
        shader.matrix("uProjection", perspective(float(width)/height));
        shader.vector("uCameraPosition", input.camera.position);
        for (const Box& box : room) {
            shader.matrix("uModel", modelMatrix(box.center, box.size));
            shader.vector("uColor", box.color);
            shader.scalar("uEmission", box.emission);
            cube.draw();
        }
        ++frames; ++totalFrames;
        if (now-titleTime >= .5 || totalFrames == 1) {
            const int fps = int(frames / std::max(now-titleTime, .001));
            const std::string title = "Server Room - early prototype | " + std::to_string(fps)
                + " FPS | WASD + Mouse | Esc: cursor | R: reset | Q: quit";
            glfwSetWindowTitle(window, title.c_str());
            frames = 0; titleTime = now;
        }
        if (smokeTest && totalFrames == 120 && !capturePath.empty())
            screenshot(capturePath, width, height);
        const GLenum error = glGetError();
        if (error != GL_NO_ERROR)
            throw std::runtime_error("OpenGL error: " + std::to_string(error));
        glfwSwapBuffers(window);
        if (smokeTest && totalFrames >= 120) {
            std::cout << "Smoke test passed: 120 rendered frames, " << room.size()
                      << " boxes, no OpenGL errors.\n";
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
}
}

int main(int argc, char** argv) {
    bool smokeTest = false;
    std::filesystem::path capturePath;
    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];
        if (argument == "--smoke-test") smokeTest = true;
        else if (argument == "--screenshot" && i+1 < argc) capturePath = argv[++i];
        else { std::cerr << "Usage: server_room [--smoke-test] [--screenshot output.ppm]\n"; return 1; }
    }
    if (!capturePath.empty() && !smokeTest) {
        std::cerr << "--screenshot requires --smoke-test\n"; return 1;
    }
    glfwSetErrorCallback([](int code, const char* message) {
        std::cerr << "GLFW " << code << ": " << message << '\n';
    });
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    if (smokeTest) glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Server Room - starting", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    int result = 0;
    try {
        if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)))
            throw std::runtime_error("Could not load OpenGL functions");
        const auto shaderFolder = std::filesystem::absolute(argv[0]).parent_path() / "shaders";
        run(window, shaderFolder, smokeTest, capturePath);
    } catch (const std::exception& error) {
        std::cerr << "Server Room: " << error.what() << '\n'; result = 1;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}
