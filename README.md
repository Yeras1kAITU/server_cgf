# Server Room

A real-time interactive 3D server room built in C++ with OpenGL 3.3+ core profile for the Computer Graphics Fundamentals final project.

The scene will feature rows of reusable server racks, independently blinking LEDs, cold blue ceiling lights, a walk camera, and an overheat alarm that switches the room to flashing red lighting.

## Current status

Initial folder structure only. Implementation, dependencies, and build configuration have not been added. The project cannot be built or run yet.

## Project structure

```text
source/
|-- readme.md
|-- src/
|   |-- app/                 Application lifecycle, timing, and shared state
|   |-- graphics/            Meshes, textures, shaders, lights, and render passes
|   |-- scene/               Room layout, racks, hierarchy, and object behavior
|   |-- input/               Walk camera and keyboard/mouse controls
|   `-- ui/                  Controls help, FPS counter, and monitor graph
|-- include/
|   `-- server_room/         Public headers, organized to match src/
|       |-- app/
|       |-- graphics/
|       |-- scene/
|       |-- input/
|       `-- ui/
|-- shaders/                Separate GLSL .vert and .frag files
|-- assets/
|   |-- models/             Model assets, if used
|   `-- textures/           Floor, rack, and other material textures
|-- third_party/            Permitted dependencies bundled when required
`-- docs/                   Shared report, presentation, and supporting material
```

All directories start empty. No placeholder files are included. Git does not track empty directories; this tree records the intended layout until implementation files are added.

## Team responsibilities

| Student | Implementation responsibilities |
| --- | --- |
| Yerassyl | Room geometry; reusable racks and efficient repetition; Mesh and Texture modules; UVs and mipmapping; model transformations; rack/server/door hierarchy; hinged door behavior |
| Dayana | Shader module and GLSL programs; per-fragment Blinn-Phong lighting; blue ceiling lights; seeded LED blinking; flashing red alarm appearance; fog and its visual controls | Cold-air fog |
| Zhalgas | Application setup and build configuration; render loop; walk camera; input and shared scene state; interaction wiring; controls help; FPS counter; integrated build and performance checks |

Every student tests and fixes their own modules, makes regular commits, participates in integration and learns enough to explain the entire application. Documentation, README updates, credits, report, references, presentation, and demo preparation are the responsibility of the whole team.

## Planned features and requirements

- Modern OpenGL pipeline with VAO/VBO/EBO and custom vertex and fragment shaders.
- At least five distinct scene object types: racks, floor, walls, ceiling fixtures, and a monitor.
- Model, view, and projection matrices, with a working parent-child hierarchy for racks, server units, and doors.
- Per-fragment ambient, diffuse, and specular lighting with at least two light sources, including a toggled or changing light.
- Texture mapping with correct UVs and mipmapping.
- Keyboard-and-mouse walk camera with frame-rate-independent movement.
- Three scene interactions: toggle the overheat alarm, open/close a rack door, and adjust cooling/fog density.
- Visible controls help and an FPS counter.
- Minimum performance target: stable 60+ FPS at 1920 x 1080 on the TA machine; preferred target: 120+ FPS.
- Stretch goals: cable trays and textured floor tiles, cold-air fog, and a monitor showing a live FPS graph.

## Build and run

Build commands will be added when the application setup is implemented. Use the same C++ toolchain and permitted library setup as the course assignments. Any additional permitted dependencies must be included and build without extra installation on the TA machine.

The shared team README must eventually include exact build/run commands, final controls, implemented features, member names, and actual contributions.

## Credits and AI use

No third-party assets or implementation dependencies have been added yet. The whole team must record asset authors, source links, and licenses in CREDITS.md when assets are introduced.

Initial AI-use note: OpenAI Codex assisted with reading the course requirements, proposing the responsibility split. The team must review this plan and update the AI-use disclosures in this README and the report as development proceeds.
