cmake_minimum_required(VERSION 3.25)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(ComputerGraphicsAssignment4 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
include(FetchContent)

FetchContent_Declare(
        glm
        URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
)
FetchContent_MakeAvailable(glm)

set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)

if (EXISTS "${CMAKE_SOURCE_DIR}/external/freeglut/CMakeLists.txt")
    add_subdirectory(external/freeglut EXCLUDE_FROM_ALL)
    set(FREEGLUT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/freeglut/include")
else ()
    FetchContent_Declare(
            freeglut
            URL https://github.com/FreeGLUTProject/freeglut/releases/download/v3.8.0/freeglut-3.8.0.tar.gz
    )
    FetchContent_MakeAvailable(freeglut)
    set(FREEGLUT_INCLUDE_DIR "${freeglut_SOURCE_DIR}/include")
endif ()

add_library(assignment4 STATIC
        src/glad.c
        "Source Files/MathUtils.cpp"
        "Source Files/Window.cpp"
        "Source Files/ShaderProgram.cpp"
        "Source Files/VertexBuffer.cpp"
        "Source Files/OrbitCamera.cpp"
        "Source Files/BezierPatch.cpp"
        "Source Files/Surfaces.cpp"
        "Source Files/Framebuffer.cpp"
        "Source Files/Texture.cpp"
        "Source Files/PatchModel.cpp"
        "Source Files/PhongScene.cpp"
        "Source Files/shaderClass.cpp"
        "Source Files/VAO.cpp"
        "Source Files/VBO.cpp"
        "Source Files/smfLoader.cpp"
        "Source Files/modelData.cpp"
)
target_include_directories(assignment4 PUBLIC include Header "${FREEGLUT_INCLUDE_DIR}")
target_compile_definitions(assignment4 PUBLIC FREEGLUT_STATIC)
target_link_libraries(assignment4 PUBLIC freeglut_static OpenGL::GL glm)

set(SHADER_DIR "${CMAKE_SOURCE_DIR}/Resource Files/Shaders")

function(add_part NAME SOURCE)
    add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} PRIVATE assignment4)

    # shaders are loaded at runtime from <exe folder>/shaders
    add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SHADER_DIR}" "$<TARGET_FILE_DIR:${NAME}>/shaders")

    # SMF models for Part 3b
    add_custom_command(TARGET ${NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/models" "$<TARGET_FILE_DIR:${NAME}>/models")

    # optional data file next to the exe (course control points)
    foreach (DATA control_points.txt)
        if (EXISTS "${CMAKE_SOURCE_DIR}/${DATA}")
            add_custom_command(TARGET ${NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    "${CMAKE_SOURCE_DIR}/${DATA}" "$<TARGET_FILE_DIR:${NAME}>")
        endif ()
    endforeach ()
endfunction()

add_part(part1_bezier          parts/part1_bezier.cpp)
add_part(part2_picking         parts/part2_picking.cpp)
add_part(part3a_texture_patch  parts/part3a_texture_patch.cpp)
add_part(part3b_procedural     parts/part3b_procedural.cpp)
