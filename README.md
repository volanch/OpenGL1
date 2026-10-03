# Assignment 2 – Computer Graphics Fundamentals

**Author:** Kovernikov Vladislav, group SE-2415
**Instructor:** Sabina Saleshova
**University:** Astana IT University, School of Software Engineering

Two OpenGL programs written in C++17 with FreeGLUT, GLAD and GLM.

| Program | Topic |
|---|---|
| `part1` | Interaction, windows, menus and animation |
| `part2` | 3D colour cube with keyboard-controlled transformations |

---

## Requirements

- C++17 compiler
- CMake 3.25 or newer
- FreeGLUT
  - either put the FreeGLUT sources in `external/freeglut/` (CMake builds them), or
  - install it on the system
- An internet connection on the first configure (CMake downloads GLM 1.0.1)
- GLAD is already included in `../../Downloads/include` and `src/glad.c`

## Build

```bash
cmake -S . -B build
cmake --build build
```

CMake copies the `shaders/` folder next to each executable. The shaders are read
at run time with relative paths, so run the programs from the folder that
contains `shaders/` (the folder with the executable).

```bash
cd build
./part1
./part2
```

On Windows the executables are `part1.exe` and `part2.exe` (for CLion, they are in
`cmake-build-debug/`).


## Part 1 – Interaction, Windows, Menus and Animation

Three windows: the **main window** (with a sub-window inside it) and **window 2**.

| Where | Input | Action |
|---|---|---|
| Main window | Right-click | Menu: Stop Animation, Start Animation, Square Colors -> (White, Red, Green) |
| Main window | Left-click | New "breathing" circle with a random colour at the click position (extra credit) |
| Sub-window | Right-click | Menu to change the sub-window background colour |
| Window 2 | `r` `g` `b` `y` `o` `p` `w` | Change the colour of the circle and triangle (red, green, blue, yellow, orange, purple, white) |
| Main window, window 2 | `ESC` | Quit (`q` also works in the main window) |

Animation (idle function): squares rotate counter-clockwise, the triangle rotates
clockwise, and the circles grow and shrink.

### Assignment checklist – Part 1

| # | Requirement | Where |
|---|---|---|
| 1 | Double buffering | `GLUT_DOUBLE`, `glutSwapBuffers()` |
| 2 | Main window with black & white square | `DisplayMain()` |
| 3 | Ellipse in a sub-window with its own background | `glutCreateSubWindow`, `DisplaySub()` |
| 4 | Background colour menu on the sub-window only | `OnSubBg`, menu attached while the sub-window is current |
| 5 | Circle + triangle in "window 2", colour keys | `DisplayWin2()`, `KeyWin2()` |
| 6 | Idle-function animation | `Idle()` |
| 7 | Main window right-click menu | `OnMainMenu`, `OnSquareColor` |
| Extra | Left-click spawns a breathing circle | `MouseMain()` |


## Part 2 – 3D Colour Cube

Right-click opens a menu with **SCALE**, **ROTATE** and **TRANSLATE**. The keys
change the component of the selected transformation. The cube is redrawn only
after a key stroke.

| Key | Action |
|---|---|
| `x` / `X` | Increase / decrease the X component of the current transformation |
| `y` / `Y` | Increase / decrease the Y component |
| `z` / `Z` | Increase / decrease the Z component |
| `+` / `-` | Increase / decrease the delta of the current transformation |
| `r` | Reset all transformations and deltas |
| `i` | Isometric view (extra shortcut) |
| `q` / `ESC` | Quit |

Each transformation has its own delta (scale 0.10, rotation 5 degrees,
translation 0.10 at the start). The matrix is `M = T * R * S` (scale, then
rotation, then translation), calculated in the application and applied to the
vertices in the vertex shader (`uTransform`). There is no camera or projection,
so the default view volume [-1, 1] is used and z translation does not change the
size of the cube. The program prints only the instructions to the terminal.

### Assignment checklist – Part 2

| # | Requirement | Where |
|---|---|---|
| 1 | Cube geometry and colours from the lecture | `cube.h` |
| 2 | Depth test | `glEnable(GL_DEPTH_TEST)`, `GLUT_DEPTH` |
| 3 | Menu SCALE / ROTATE / TRANSLATE, order S -> R -> T | `menuHandler`, `TransformState::Matrix()` |
| 4 | Six keys for X, Y, Z, redraw only on key stroke | `keyboard()`, `TransformState::Adjust()` |
| 5 | Two keys for the delta, one delta per transformation | `+` / `-`, `TransformState::AdjustDelta()` |
| 6 | Reset key | `r`, `TransformState::Reset()` |
| 7, 8 | Matrix calculated in the app, applied in the vertex shader | `shaders/cube.vert` |
| 9 | No camera or projection | no `lookAt`, `perspective` or `ortho` calls |
| 10 | Instructions printed to the terminal, no other text output | `PrintInstructions()` |

---

## Project structure

```
part1.cpp
part2.cpp
shapes2d.h           ellipse / square / triangle generators (Part 1)
mesh.h               VAO + VBO + draw mode + vertex count
cube.h               36-vertex colour cube (Part 2)
transform.h          scale / rotate / translate state and matrix (Part 2)
shaderClass.h/.cpp   Shader loader (reads .vert/.frag files)
VAO.h/.cpp  VBO.h/.cpp  EBO.h/.cpp
shaders/             flat2d.vert/.frag (Part 1), cube.vert/.frag (Part 2)
include/  src/       GLAD
CMakeLists.txt
```