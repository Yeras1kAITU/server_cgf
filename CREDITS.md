# Credits

## Bundled dependencies

- **GLFW 3.4**: Marcus Geelnard, Camilla Lowy, and GLFW contributors. Window/context creation and keyboard/mouse input. Source: https://github.com/glfw/glfw/tree/3.4 ; documentation: https://www.glfw.org/docs/3.4/quick.html . License: zlib, preserved in `third_party/glfw/LICENSE.md`. The upstream 3.4 source archive is bundled without changes.
- **GLAD OpenGL loader**: David Herberth and contributors, with Khronos API definitions. The generated header is bundled unmodified from https://github.com/glfw/glfw/blob/3.4/deps/glad/gl.h . Generator project: https://github.com/Dav1dde/glad . License: MIT for GLAD; Khronos notices are embedded in the header. The upstream license text is retained in `third_party/glad/LICENSE`. `src/graphics/GlLoader.cpp` enables the header's implementation. Although the supplied loader exposes compatibility symbols, the application requests a 3.3 core context and uses only the modern pipeline.

## Project content

The room, rack models, matrix/camera code, and lighting shaders were authored for this project with OpenAI Codex assistance. No third-party models, textures, or copied tutorial shaders are included. GLFW documentation was consulted for context creation, input, callbacks, and lifecycle behavior.

## Final project content and implementation references

The completed scene uses project-generated rack/door/room/cable geometry, a deterministic tile texture, an original 5x7 bitmap alphabet, and custom shaders, all authored with OpenAI Codex assistance. The floor texture is generated directly in Texture.cpp; no third-party texture file, font, model, tutorial shader or asset pack is used. Technical notes describe the algorithms and AI-use scope.

Khronos documentation was consulted for vertex attribute divisors and indexed instancing:
- https://wikis.khronos.org/opengl/Vertex_Format
- https://wikis.khronos.org/opengl/GLAPI/glDrawElementsInstanced
- https://registry.khronos.org/OpenGL/index.php

These are references consulted, not copied shader/code assets. See the specific third-party license notices above and the matching vendored license files. GLFW and GLAD remain the only runtime implementation dependencies.
