# Assignment 1

**Student:** Kovernikov Vladislav
**Course:** Computer Graphics Fundamentals
**Instructor:** Saleshova Sabina

## What this submission contains

Four small OpenGL 3.3 programs, built on a shared set of
GPU-object wrapper classes (`Shader`, `VAO`, `VBO`, `EBO`) and a shape-generator
header. Everything is drawn through vertex buffers, a vertex array object,
and GLSL shaders.

| Program | Source file | Task | What it draws |
|---|---|---|---|
| `red_triangle` | `red_triangle.cpp` | Task 1, Step 1 | Getting started example: an 800×800 window with a solid red triangle, drawn from 3 vertices with a single VBO. |
| `blue_square` | `blue_square.cpp` | Task 1, Step 2 | `red_triangle` modified into a solid blue square, built from an indexed quad (2 triangles with an EBO). |
| `polygons_part1` | `polygons_part1.cpp` | Task 2, Part 1 | A 500×500 window with a shaded circle (used `GL_TRIANGLE_FAN`), an solid ellipse, a 3‑colour triangle, and six black/white squares. |
| `polygons_part2` | `polygons_part2.cpp` | Task 2, Part 2 | A sunset sky, a sun, two layered mountains, and a 5‑pointed star. |

All shapes are generated procedurally with parametric equations
(`x = cx + r·cos(θ), y = cy + r·sin(θ)`) in `Header/shapeGen.h`, packed into
a single interleaved vertex buffer per program (there {x, y, r, g, b} per vertex),
and drawn with one `glDrawArrays/glDrawElements` call per shape. Per-vertex
color is passed to `default.vert`/`default.frag` (or `solid.vert`/`solid.frag`
for the solid figures).

## Requirements

- CMake ≥ 3.25
- A C++17 compiler (GCC, Clang, or MSVC)
- An internet connection the *first* time you build — CMake's `FetchContent`
  downloads GLFW 3.4 and GLM 1.0.1 automatically. GLAD is vendored in
  `include/` and `src/glad.c`, so it does not need to be downloaded.
- A GPU/driver with OpenGL 3.3 core-profile support and a display (all four
  programs open a window; none of them run headless).

## Build

```bash
make
```

This runs `cmake -S . -B build` followed by `cmake --build build`, producing
four executables inside `build/`: `red_triangle`, `blue_square`,
`polygons_part1`, and `polygons_part2`.

## Run

```bash
make run-red_triangle
make run-blue_square
make run-polygons_part1
make run-polygons_part2
```

Close a window to exit.

Or you can run it just by commenting all executable files except one and run it.
example:
```
add_executable(ComputerGraphicsProject
    #red_triangle.cpp
    src/glad.c
    #blue_square.cpp
    Header/shaderClass.h
    "Source Files/shaderClass.cpp"
    Header/VBO.h
    "Source Files/VBO.cpp"
    Header/EBO.h
    "Source Files/EBO.cpp"
    Header/VAO.h
    "Source Files/VAO.cpp"
    Header/shapeGen.h
    #polygons_part1.cpp
    polygons_part2.cpp
)
```

## Tested on

Windows 11, CLion

## Project layout

```
vk_a1/
├── red_triangle.cpp
├── blue_square.cpp
├── polygons_part1.cpp
├── polygons_part2.cpp
├── CMakeLists.txt
├── Makefile
├── Header/                  Shader/VAO/VBO/EBO headers + shapeGen.h
├── Source Files/            Shader/VAO/VBO/EBO classes
├── Resource Files/Shaders/  GLSL vertex/fragment shaders
├── include/
└── src/glad.c
```
