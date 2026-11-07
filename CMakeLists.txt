cmake_minimum_required(VERSION 3.16)
project(ServerRoom VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glfw)

add_executable(server_room
    src/app/main.cpp
    src/graphics/GlLoader.cpp
    src/graphics/CubeMesh.cpp
    src/graphics/Shader.cpp
    src/scene/Room.cpp)
target_include_directories(server_room PRIVATE include third_party/glad/include)
target_link_libraries(server_room PRIVATE glfw)
if(MSVC)
    target_compile_options(server_room PRIVATE /W4)
else()
    target_compile_options(server_room PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_custom_command(TARGET server_room POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "$<TARGET_FILE_DIR:server_room>/shaders")
