cmake_minimum_required(VERSION 3.16)
project(ServerRoom VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glfw)

add_executable(server_room
    src/app/main.cpp
    src/app/Application.cpp
    src/app/Options.cpp
    src/graphics/GlLoader.cpp
    src/graphics/CubeMesh.cpp
    src/graphics/Shader.cpp
    src/graphics/Texture.cpp
    src/ui/Canvas.cpp
    src/ui/Display.cpp
    src/scene/Room.cpp)
target_include_directories(server_room PRIVATE include third_party/glad/include)
target_link_libraries(server_room PRIVATE glfw)
if(MINGW)
    target_link_options(server_room PRIVATE -static)
endif()
if(MSVC)
    target_compile_options(server_room PRIVATE /W4)
else()
    target_compile_options(server_room PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(CMAKE_CONFIGURATION_TYPES)
    set(SHADER_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/shaders")
else()
    set(SHADER_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/shaders")
endif()
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${SHADER_OUTPUT}")
add_dependencies(server_room copy_shaders)
include(CTest)
if(BUILD_TESTING)
    add_executable(scene_tests tests/SceneTests.cpp src/scene/Room.cpp src/app/Options.cpp)
    target_include_directories(scene_tests PRIVATE include)
    if(MINGW)
        target_link_options(scene_tests PRIVATE -static)
    endif()
    add_test(NAME scene_logic COMMAND scene_tests)
endif()
