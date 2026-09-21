cmake_minimum_required(VERSION 3.25)
project(ComputerGraphicsAssignment1 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        glfw
        URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
)

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

set(COMMON_SOURCES
        src/glad.c
        digital_postcard.cpp
        Header/shaderClass.h
        "Source Files/shaderClass.cpp"
        Header/VBO.h
        "Source Files/VBO.cpp"
        Header/EBO.h
        "Source Files/EBO.cpp"
        Header/VAO.h
        "Source Files/VAO.cpp"
        Header/shapeGen.h
)

function(add_task_executable NAME SOURCE_FILE)
    add_executable(${NAME} ${SOURCE_FILE} ${COMMON_SOURCES})
    target_include_directories(${NAME} PRIVATE include Header)
    target_link_libraries(${NAME} PRIVATE glfw glm)
endfunction()

add_task_executable(digital_postcard digital_postcard.cpp)

# Copy the GLSL shader files next to every built executable
file(GLOB SHADER_FILES
        "${CMAKE_SOURCE_DIR}/Resource Files/Shaders/*.vert"
        "${CMAKE_SOURCE_DIR}/Resource Files/Shaders/*.frag")
file(COPY ${SHADER_FILES} DESTINATION ${CMAKE_BINARY_DIR})