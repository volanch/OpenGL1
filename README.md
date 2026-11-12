# Assignment 4 - Bezier Patch, Picking / Anti-aliasing, Texture Mapping

**Course:** Computer Graphics Fundamentals
**Instructor:** Saleshova Sabina

## Programs

| Program | Source | Assignment part | What it does |
|---|---|---|---|
| `part1_bezier` | `parts/part1_bezier.cpp` | Part 1 | Flat-shaded bicubic Bezier patch (10x10 start), Phong diffuse per triangle, light attached to the camera, 16 control points as `GL_POINTS`, RGB axes, interactive control point editing and resolution change. |
| `part2_picking` | `parts/part2_picking.cpp` | Part 2 | Three smooth-shaded meshes with colour-ID picking in an FBO, random diffuse colour on click, anti-aliasing toggle (4x multisampled FBO resolved with `glBlitFramebuffer`). |
| `part3a_texture_patch` | `parts/part3a_texture_patch.cpp` | Part 3.1 | Assignment 3 program (Phong only, one glossy material) with a 2D procedural image texture on a 12x12 Bezier mesh. Texture coordinates = (u,v) of the vertex position. The texture colour replaces the constant diffuse colour in the fragment shader. |
| `part3b_procedural` | `parts/part3b_procedural.cpp` | Part 3.2 | Assignment 3 program (Phong only, one glossy material) loading an SMF model with a 3D procedural veined-marble texture computed from world (x,y,z) in the fragment shader and used as the Phong diffuse colour. |

Part 3 extends the Assignment 3 code (cylindrical camera, two lights, parallel/perspective
projection, SMF loader with smooth normals). It keeps only the Phong shading algorithm and
the first material of Assignment 3 (glossy, with a specular highlight). The Gouraud shader
and the material switching were removed.

## Libraries

- OpenGL 3.3 core profile
- **freeglut** - window and input (fetched by CMake, or `external/freeglut` if present)
- **glad** - OpenGL loader (`include/glad`, `include/KHR`, `src/glad.c`)
- **GLM** - math for the Assignment 3 based code (fetched by CMake)

## Build and run

CLion: open the folder, let CMake load, pick a target and press Run.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cd build
./part1_bezier
./part3b_procedural bound-bunny_1k.smf     # model from the models folder, or a path
```

The first configure needs internet (freeglut and GLM are downloaded).
After each build `shaders/` and `models/` are copied next to the executable, so run it
from that folder (CLion does this). `control_points.txt` in the project root is copied
too and used by Part 1 and Part 3a; built-in points are used if it is missing.

## Controls

Part 1: drag / arrows rotate, `z` `x` zoom, `r` reset view, `c` reset control points, `n` `b` select point, `a` `d` / `s` `w` / `q` `e` move point -/+ in x / y / z, `+` `-` resolution.

Part 2: left click random colour, right-drag / arrows rotate, `z` `x` zoom, `r` reset, `a` anti-aliasing on/off.

Part 3a and 3b (Assignment 3 controls):

| Keys | Action |
|---|---|
| arrows | camera angle / height |
| `+` `-` (PgUp/PgDn) | camera radius |
| `a` `d` / `w` `s` / `e` `q` | light 1 angle / height / radius |
| `p` | parallel / perspective |
| `z` `x` | light 1 / light 2 on-off |
| `u` | cylinder axis Z-up / Y-up |
| `n` | flip normals |
| `r` | reset, `Esc` quit |

## Project layout

```
├── CMakeLists.txt  Makefile  README.md  control_points.txt (course file)
├── external/                 optional freeglut sources
├── Header/                   MathUtils  Window  ShaderProgram  VertexBuffer  OrbitCamera
│                             Mesh  BezierPatch  Surfaces  Framebuffer  Texture   (Parts 1-2, texture)
│                             PatchModel  PhongScene                                (Part 3)
│                             shaderClass  VAO  VBO  smfLoader  modelData  materials  orbit   (Assignment 3)
├── Source Files/             implementations
├── parts/                    the four programs
├── Resource Files/Shaders/   part1 / part2 / part3a / part3b .vert .frag, marker .vert .frag
├── models/                   SMF models (bound-lo-sphere, bound-bunny_1k, teapot)
├── include/                  glad/  KHR/
└── src/glad.c
```
