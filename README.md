# Assignment Week 3

**Student:** Kovernikov Vladislav
**Course:** Computer Graphics Fundamentals
**Instructor:** Saleshova Sabina

## What this submission contains

Small OpenGL 3.3 program, built on a shared set of
GPU-object wrapper classes (`Shader`, `VAO`, `VBO`, `EBO`) and a shape-generator
header. Everything is drawn through vertex buffers, a vertex array object,
and GLSL shaders.

| Program | Source file | Task | What it draws |
|---|---|---|---|
| `digital_postcard` | `digital_postcard.cpp` | Assignment week 3 - OpenGL Digital Postcard | A "Sunset to Night" scene built directly on top of the Assignment 1 scaffold. |

## Digital Postcard ("Sunset to Night")

`digital_postcard.cpp` reuses the Assignment 1 `Shader`/`VAO`/`VBO` wrapper
classes and the `Header/shapeGen.h` shape generators as-is, and reuses the sky /
sun / mountains / star shapes from `polygons_part2.cpp` my previous work's "Sunset" scene
almost unchanged. On top of that it adds a star field, a small flock of
birds, and a Day/Night toggle, to satisfy every Assignment 3 requirement:

| Requirement | How it's met |
|---|---|
| 1 window, 1 complete 2D scene | Single window, one layered scene |
| ≥ 5 visible objects | Sky, sun/moon, back mountain, front mountain, evening star, star field, birds = 7 objects |
| `GL_POINTS` | Star field (`MakeStarField`) |
| `GL_LINES` | Birds (`MakeBird`) |
| `GL_TRIANGLES` | Both mountains |
| Complex objects from basic primitives | Sky/sun/moon/star are triangle fans built from a ring of triangles (same technique as Assignment 1's circle) |
| VAO + VBO | One shared `VAO`/`VBO`, all shapes packed into one interleaved buffer |
| ≥ 4 RGB colors | Sky top/bottom, sun/moon, two mountain tones, star, star field, birds |
| Color interpolation on one object | The sky quad: dusk-purple top vertices → warm-orange bottom vertices |
| Vertex + Fragment shader | `Resource Files/Shaders/default.vert` / `Resource Files/Shaders/default.frag` (extended, still back-compatible with `polygons_part1/2`) |
| Uniform variable | `uOffset` (slides the sun into moon position), `uNightMix` + `uNightTint` (blend every color toward a night tint) |
| Interaction | **Press Space** to crossfade between Day mode (sun, birds) and Night mode (moon, star field, evening star) |

Run it with:

```bash
make run-digital_postcard
```

Close a window to exit.

If you are in Clion just select executable and run it.

## Requirements

- CMake ≥ 3.25
- A C++17 compiler (GCC, Clang, or MSVC)
- An internet connection the *first* time you build — CMake's `FetchContent`
  downloads GLFW 3.4 and GLM 1.0.1 automatically. GLAD is vendored in
  `include/` and `src/glad.c`, so it does not need to be downloaded.
- A GPU/driver with OpenGL 3.3 core-profile support and a display (all four
  programs open a window; none of them run headless).

## Tested on

Windows 11, CLion

## Project layout

```
vk_a2/
├── digital_postcard.cpp
├── CMakeLists.txt
├── Makefile
├── Header/                  Shader/VAO/VBO/EBO headers + shapeGen.h
├── Source Files/            Shader/VAO/VBO/EBO classes
├── Resource Files/Shaders/  GLSL vertex/fragment shaders
├── include/
└── src/glad.c
```
