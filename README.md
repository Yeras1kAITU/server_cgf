# Server Room

A C++17 / OpenGL 3.3 core application for the Computer Graphics Fundamentals final project. The default window is Full HD (1920 x 1080). Walk through two rows of server racks, inspect the independently blinking status LEDs, and press **F** to switch the room from cold blue to flashing red with additional alarm fog.

## Build and run

From a Windows terminal:

```text
cd *path to the project*
make
make run
```

Prerequisites: Windows GNU Make, MinGW-w64 GCC/G++, and an OpenGL 3.3-capable graphics driver. The Makefile automatically finds the MSYS2 UCRT64 or MinGW64 compiler under `C:/msys64`. Otherwise put GCC/G++ on PATH, or use `make TOOLCHAIN=D:/tools/mingw64/bin`. Substitute `mingw32-make` for `make` if that is the name of your GNU Make executable. The Makefile uses `cmd.exe` recipes; it does not invoke PowerShell or batch scripts.

GLFW 3.4 and GLAD are bundled as source. The build needs no internet access or separately installed graphics packages. Compiler runtimes and GLFW are linked statically; keep `build/shaders/` beside `build/server_room.exe` when distributing the program. The executable can be launched from another working directory.

```text
make -j4          # parallel build
make run         # incremental build and launch
make test        # scene, math, camera and CLI checks
make smoke-test  # real GPU checks for all five visual scenarios
make benchmark   # uncapped local frame-time measurements
make clean       # remove generated build output
```

The first build compiles GLFW; later builds track source and header changes. Build artifacts stay in the ignored `build/` directory.

## Controls

| Input | Action |
| --- | --- |
| W / A / S / D | Walk forward / left / backward / right |
| Mouse | Look around |
| F | Toggle the room-wide overheat alarm |
| E | Open/close the rack under the crosshair, within 3 metres |
| [ / ] | Reduce/increase cooling fog in 5% steps; hold to repeat |
| H | Show/hide the controls panel |
| R | Reset the camera to the entrance |
| Escape | Release the mouse |
| Left click | Capture the mouse again |
| Q / close button | Exit |

The targeted rack grille is cyan and an E prompt appears. Rack doors animate around their hinges. Movement stays at eye height and slides along racks, doors, and room boundaries. Losing focus releases the mouse. FPS and frame time remain visible when help is hidden.

## Implemented features

- Modern indexed geometry (VAO/VBO/EBO) and separate custom GLSL vertex/fragment shaders.
- Eight copies of one rack design, each containing eight server units and 24 LEDs: **192 independently seeded emissive LED quads**. Blink timing runs entirely in the fragment shader.
- Room shell, server chassis, vent slots, door grilles/handles, ceiling fixtures, cable trays with cable bundles, floor, and a wall-mounted monitor.
- Real scene hierarchy: rack root -> server parts and door hinge -> animated door components. Model, view, projection, and inverse-transpose normal transforms are implemented in project code.
- Per-fragment Blinn-Phong ambient, diffuse, and specular lighting from three ceiling light sources.
- **F** changes actual light colors/intensities and emissive fixtures from blue to synchronized flashing red and adds alarm fog, even when cooling fog is set to zero. Returning to normal restores the blue room and removes only the alarm fog contribution.
- Texture-mapped floor tiles with explicit UVs, repeated one-metre tiles, trilinear mipmapping, and a deterministic generated texture. No external assets are needed.
- Adjustable distance/height-dependent cold-air fog.
- A monitor displaying actual live FPS and frame time plus a rolling FPS graph (up to 24 seconds of samples, updated at 10 Hz).
- Instanced rendering: static room/rack geometry and LED quads upload once. Door instance data updates only while a door moves or selection changes. UI/monitor pixels update at 10 Hz.
- Smooth keyboard/mouse navigation, collision boundaries, four-sample MSAA, on-screen help, target prompt, mode/cooling display, and persistent FPS counter.

## Project layout

```text
include/server_room/  Header declarations and small math/camera definitions
src/app/              Application lifecycle, CLI, input callbacks, render loop
src/graphics/         Shader, instanced mesh, texture and OpenGL loader modules
src/scene/            Reusable rack layout, scene hierarchy, picking, collisions
src/ui/               Original bitmap font, help overlay and FPS monitor
shaders/              GLSL world and overlay programs
third_party/          GLFW 3.4 and generated GLAD header, with licenses
assets/               Reserved for assets; the current scene generates its own
tests/                Scene/camera/picking/hierarchy/CLI regression checks
docs/                 Shared technical notes, testing evidence and defense checklist
```

Every project-owned implementation file is below approximately 500 lines. Third-party sources are preserved separately.

## Team responsibilities and contributions

The three roles remain balanced in implementation, integration, and testing:

| Role | Scope |
| --- | --- |
| Yerassyl Ibrayev | Geometry/materials, reusable racks, Mesh and Texture modules, UVs/mipmapping, model transforms, rack/server/door hierarchy and hinged-door behavior |
| Dayana Kulmagambetova | Shader module and GLSL, per-fragment Blinn-Phong lighting, blue lights, seeded LED timing, flashing red alarm, fog and visual controls |
| Zhalgas Torekeldi | Build/application setup, render loop, camera/input, shared state, interaction wiring, controls help, FPS counter, integrated tests and performance |

**Documentation, report, credits, slides, and demo preparation are shared by the whole team.**

## Optional CMake build

CMake 3.16+ can build the same sources with a compatible compiler:

```text
cmake -S . -B build/cmake
cmake --build build/cmake --config Release
ctest --test-dir build/cmake -C Release --output-on-failure
```

Visual Studio puts the executable in `build/cmake/Release/`; single-configuration generators put it in `build/cmake/`. The shaders are copied to the matching directory, including when only shader sources change. Other platforms require the usual GLFW platform development libraries; Windows Make is the primary course workflow.

## Validation and screenshots

```text
make run ARGS="--1080p"
make run ARGS="--720p"
make run ARGS="--smoke-test --scenario alarm --screenshot build/alarm.ppm"
make run ARGS="--benchmark --scenario fog"
```

Scenarios: `normal`, `alarm`, `doors`, `fog`, and `monitor`. Interactive scenarios set an initial view/state and retain normal controls.

The normal application enables VSync and may therefore show the display refresh rate or a driver-imposed GPU limit rather than maximum throughput. The team achieved stable 144 FPS under a GPU cap and no noticeable lag.

## Credits and AI use

See [CREDITS.md](CREDITS.md) for dependency authors, source links, and license notices. Geometry, floor pixels, font, UI, math, and shaders were created for this project. No external models/textures, game engines, high-level renderers, sound libraries, or downloaded tutorial shaders are used.

**AI Use Note:** OpenAI Codex assisted with preparing plan of works, dividing responsibilities, providing mathematical formulas, code debugging and used as objective rater and provided with improvement suggestions. It also assisted with finding appropriate articles.
