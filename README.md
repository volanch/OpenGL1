# Assignment 3 – Computer Graphics Fundamentals

**Author:** Kovernikov Vladislav, group SE-2415
**Instructor:** Saleshova Sabina
**University:** Astana IT University, School of Software Engineering

One OpenGL 3.3 program (`assignment3`) written in C++17 with FreeGLUT, GLAD and GLM.
It covers both parts of the assignment in a single executable:

| Part | Topic | How to see it |
|---|---|---|
| Part 1 | Viewing, parallel / perspective projection, flat shading (triangle colour = \|normal\|), cylinder camera | key `1` |
| Part 2 | Phong reflection model, Gouraud and Phong shading, materials, two point lights | keys `2`, `3` |

The program starts in **Phong** mode (the final result of Part 2).

---

## Requirements (Windows)

- Windows 11
- CLion (bundled MinGW + CMake) **or** Visual Studio 2022 with the C++ CMake tools
- CMake 3.25 or newer, C++17 compiler
- An internet connection on the first configure: CMake downloads **GLM 1.0.1** and
  **FreeGLUT 3.8.0** and builds FreeGLUT as a static library. No manual FreeGLUT install is needed.
  (If you prefer, put the FreeGLUT sources in `external/freeglut/` and CMake uses them instead.)
- GLAD is already in `/include` and `src/glad.c`.
- A GPU driver with OpenGL 3.3 support.

## Build

### CLion
Open the folder, let CMake load, select the `assignment3` target and click **Build**.
To pass the model, open *Run - Edit Configurations - assignment3 - Program arguments* and type
model name from the folder (`models/`).

### Command line (PowerShell / cmd)
```bat
cmake -S . -B build
cmake --build build --config Release
cd build
assignment3.exe bound-lo-sphere.smf
```

CMake copies the `shaders/` and `models/` folders next to the executable after each build, and the
program looks for them there, so it can be started from any working directory.

## Run

```bat
assignment3.exe <model.smf>
```

The model name is passed as the **command-line argument**. It can be a full path, a relative path,
or just a file name from the `models/` folder next to the executable. You can also drag an `.smf`
file onto `assignment3.exe`. If no argument is given the program prints a usage message and loads
`bound-lo-sphere.smf`.

| File | Faces |
|---|---|
| `models/bound-lo-sphere.smf` | 1088 |
| `models/bound-bunny_1k.smf` | 1000 |

## Controls

| Key | Action |
|---|---|
| `←` / `→` | Camera cylinder angle θ −/+ |
| `↑` / `↓` | Camera height H +/− |
| `+` / `-` (or `PgUp` / `PgDn`) | Camera orbit radius R larger / smaller |
| `a` / `d` | Light 1 angle −/+ |
| `w` / `s` | Light 1 height +/− |
| `e` / `q` | Light 1 radius larger / smaller |
| `1` | Flat shading, colour = \|unit normal\| (Part 1) |
| `2` | Gouraud shading |
| `3` | Phong shading |
| `m` / `M` | Next / previous material |
| `p` | Parallel - perspective projection |
| `z` / `x` | Light 1 / light 2 on / off |
| `u` | Cylinder axis Z-up - Y-up (for models such as the bunny) |
| `n` | Flip normals (for models wound clockwise) |
| `r` | Reset camera and light |
| `Esc` | Quit |

---

## Libraries used

| Library | Purpose |
|---|---|
| FreeGLUT 3.8.0 | window, OpenGL 3.3 core context, keyboard input |
| GLAD | OpenGL function loader |
| GLM 1.0.1 | vectors, matrices, `lookAt`, `perspective`, `ortho`, `inverseTranspose` |

## Project structure

```
assignment3.cpp        main program: window, input, camera, lights, drawing
Header/
  smfLoader.h          SMF file loader interface
  modelData.h          vertex layout, face normals, averaged vertex normals
  materials.h          the 4 materials and the light colours
  orbit.h              cylindrical -> Cartesian coordinates (camera and light)
  shaderClass.h  VAO.h  VBO.h  EBO.h
Source Files/
  smfLoader.cpp  modelData.cpp  shaderClass.cpp  VAO.cpp  VBO.cpp  EBO.cpp
Resource Files/Shaders/
  flat.vert/.frag      Part 1: colour = |normal|
  gouraud.vert/.frag   Part 2: lighting per vertex
  phong.vert/.frag     Part 2: lighting per fragment
  marker.vert/.frag    yellow dot at light 1
models/                sample .smf files
include/  src/glad.c
CMakeLists.txt
```

## Notes

- If a model looks black or inside-out, press `n` (its triangles are wound clockwise).
- If a model lies on its side, press `u` to use Y as the cylinder axis.