# Open GL Engine

![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus)
![OpenGL](https://img.shields.io/badge/OpenGL-3.3%20core-5586A4)
![Platform](https://img.shields.io/badge/platform-Windows%20x64-0078D6)

A small real-time 3D rendering engine written in C++20 on top of OpenGL 3.3 and SDL3, built around an entity-component architecture. It ships as a static library plus a demo application with a heightmap terrain, a skybox, textured primitives and a free-fly camera.



[Features](#features) · [Requirements](#requirements) · [Build and run](#build-and-run) · [Usage](#usage) · [Project structure](#project-structure) · [Credits](#credits)

## Features

- **Entity-component scene:** `Scene` hands out entity ids and stores components in per-type pools (transform, mesh, texture, camera, movement).
- **Task-based main loop:** `Kernel` polls SDL events, updates input and runs the tasks every frame in this order: camera control → transform update → render → post-process.
- **Transform hierarchy:** entities can be parented to each other, and children follow their parent.
- **Built-in primitives:** cube, sphere, cylinder, cone and plane.
- **Terrain from a heightmap:** builds a mesh from a grayscale image, with configurable height and size.
- **Model import:** loads any Assimp-supported format and keeps the node hierarchy of the file.
- **Textures with transparency:** textures with any non-opaque pixel are drawn after the opaque geometry, sorted back to front, without writing depth.
- **Lighting:** Blinn-Phong shading with one point light and one directional light.
- **Cubemap skybox:** six-face skyboxes, several can be registered by name.
- **Post-processing chain:** vignette followed by FXAA, rendered through ping-pong framebuffers.
- **Free-fly camera:** keyboard and mouse look, with the current FPS shown in the window title.

## Requirements

| Component | Requirement |
|---|---|
| Operating system | Windows 10/11, 64-bit |
| IDE / compiler | Visual Studio 2022 (MSVC toolset `v143`, C++20) with the *Desktop development with C++* workload and a Windows 10/11 SDK. The solution was created with 17.14 |
| Graphics | GPU and driver with OpenGL 3.3 core profile support |
| Prebuilt libraries | Static `.lib` files for SDL3, GLAD, Assimp, SOIL2 and zlib (see below) |

> [!IMPORTANT]
> **The prebuilt static libraries are not in the repository.** The engine project links them from `open_gl_engine/engine/libraries/*/lib/x64/`, but the root `.gitignore` ignores every `x64/` folder, so a fresh clone has the headers and **no `.lib` files** and the engine will not link. Add them before building:
>
> ```text
> open_gl_engine/engine/libraries/
> ├── sdl3/lib/x64/      sdl3-static-debug.lib    sdl3-static-release.lib
> ├── glad/lib/x64/      glad-static-debug.lib    glad-static-release.lib
> ├── assimp/lib/x64/    assimp-static-debug.lib  assimp-static-release.lib
> └── soil2/lib/x64/     soil2-static-debug.lib   soil2-static-release.lib
> ```
>
> `zlib-static-debug.lib` and `zlib-static-release.lib` are also needed; put them in any of the four folders above (the project searches all of them). Build every library for x64 with the MSVC `v143` toolset and the default dynamic runtime (`/MDd` for Debug, `/MD` for Release), using the versions listed in [Credits](#credits).

## Build and run

1. Clone the repository:

   ```bash
   git clone https://github.com/Izana-L/Open-GL-Engine.git
   cd Open-GL-Engine
   ```

2. Add the prebuilt libraries described in [Requirements](#requirements).
3. Open `open_gl_engine/executable/projects/Executable.sln` in Visual Studio 2022 and select **x64** with **Debug** or **Release**.
4. Build the `open_gl_engine` project first (right-click → **Build**). It produces `open_gl_engine/engine/output/<Configuration>/open_gl_engine.lib`.
5. Build the `Executable` project. It produces `open_gl_engine/executable/output/<Configuration>/Executable.exe`.
6. Run with **F5** or **Ctrl+F5**.

> [!NOTE]
> `Executable` has no project reference to the engine, so building the whole solution in one go does not guarantee the library is built before the executable links. Build the engine first, as in step 4.

> [!IMPORTANT]
> Assets are loaded through relative paths (`../assets/shaders/`, `../assets/textures/`, `../assets/meshs/`), so the working directory must be `open_gl_engine/executable/projects`. Visual Studio uses it by default. To run the executable by hand, start it from that folder:
>
> ```bat
> cd open_gl_engine\executable\projects
> ..\output\Debug\Executable.exe
> ```

### What you should see

A 1024×576 window with a cube-mapped sky, a heightmap terrain below the camera, a textured cube that spins and a wooden cylinder, with vignette and FXAA applied. The title bar shows the FPS, refreshed about once per second.

| Input | Action |
|---|---|
| `W` / `S` | Move forward / backward |
| `A` / `D` | Strafe left / right |
| `Space` / `Shift` | Move up / down |
| Mouse | Look around (the cursor is captured) |

There is no Esc shortcut. Close the window with `Alt+F4` (or stop the debugger with `Shift+F5`).

## Usage

The demo in `open_gl_engine/executable/code/main.cpp` is the best starting point. It creates a window, a scene and a few entities, then runs the main loop:

```cpp
#include <Scene.hpp>
#include <Window.hpp>
#include <Input_Manager.hpp>
#include <Movement_Component.hpp>
#include "Entity_Creator.hpp"
using namespace open_gl_engine;

int main(int, char* [])
{
    Window::OpenGL_Context_Settings settings{ 3, 3 };
    Window window("Open GL engine", 1024, 576, settings);
    Input_Manager::initialize(window);
    Scene scene(window);

    create_skybox(scene, "day", { "sky-cube-map-0.png", "sky-cube-map-1.png", "sky-cube-map-2.png",
                                  "sky-cube-map-3.png", "sky-cube-map-4.png", "sky-cube-map-5.png" });
    create_camera(scene, glm::vec3(0.0f, 0.0f, 5.0f));
    create_terrain(scene, "height-map.png", 10.0f, 50.0f);   // heightmap, max height, size in XZ

    Id cube = create_cube(scene, glm::vec3(-2.0f, 0.0f, -11.0f), INVALID_ID, "uv-checker.png");
    scene.add_component<Movement_Component>(cube, glm::vec3(0.0f), glm::vec3(0.0f),
        glm::vec3(glm::radians(30.0f), glm::radians(45.0f), glm::radians(60.0f)));   // angular velocity

    scene.run();   // blocks until the window is closed
    return 0;
}
```

To add your own content:

- **Primitives:** `create_cube`, `create_sphere`, `create_cylinder`, `create_cone` and `create_plane` take a position, an optional parent id and an optional texture file name.
- **Textures:** put images in `open_gl_engine/executable/assets/textures/` and refer to them by file name.
- **Models:** create `open_gl_engine/executable/assets/meshs/`, copy a model there and call `create_object(scene, "model.obj", "texture.png", position)`. No sample models are included.

Public headers carry Doxygen-style `///` comments (in Spanish).

## Project structure

```text
Open-GL-Engine/
├── .gitignore
└── open_gl_engine/
    ├── engine/                  # Static library (open_gl_engine.lib)
    │   ├── code/
    │   │   ├── headers/         # Engine headers: scene, components, tasks, managers
    │   │   └── sources/         # Engine implementation
    │   ├── libraries/           # Bundled third-party headers (SDL3, GLAD, GLM, Assimp, SOIL2, half)
    │   └── projects/            # Visual Studio project of the engine
    └── executable/              # Demo application
        ├── assets/
        │   ├── shaders/         # GLSL 3.30 shaders: scene, sky, vignette, FXAA, copy
        │   └── textures/        # Textures, heightmap and skybox faces
        ├── code/                # main.cpp and Entity_Creator (entity factory helpers)
        └── projects/            # Executable.sln (open this one) and the NuGet package folder
```

## Limitations

- Only Visual Studio project files are provided, so the supported build is Windows with MSVC.
- There are no automated tests yet.
- The prebuilt libraries must be supplied separately (see [Requirements](#requirements)).

## Credits

- **Author:** Izana ([@Izana-L](https://github.com/Izana-L))



The window and OpenGL context setup in `engine/code/sources/Window.cpp` is based on public-domain code by Ángel Rodríguez (see the header of that file).

Third-party code bundled in this repository keeps its original license:

| Library | Version | License | Location |
|---|---|---|---|
| [SDL](https://www.libsdl.org) | 3.2.26 | zlib | `engine/libraries/sdl3` |
| [GLAD](https://github.com/Dav1dde/glad) (OpenGL loader) | 2.0.8 | MIT | `engine/libraries/glad` |
| [GLM](https://github.com/g-truc/glm) | 1.0.2 | Happy Bunny or MIT | `engine/libraries/glm` |
| [Assimp](https://github.com/assimp/assimp) | 5.4.3 | BSD 3-Clause | `engine/libraries/assimp` |
| [SOIL2](https://github.com/SpartanJ/SOIL2) (uses stb_image) | n/a | MIT No Attribution | `engine/libraries/soil2` |
| [half](https://half.sourceforge.net) | 2.2.0 | MIT | `engine/libraries/half` |
| [Microsoft.Windows.CppWinRT](https://github.com/microsoft/cppwinrt) (NuGet) | 2.0.220531.1 | MIT | `executable/projects/packages` |

