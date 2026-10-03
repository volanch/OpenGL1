cmake_minimum_required(VERSION 3.25)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(ComputerGraphicsAssignment2 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        glm
        URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
)
FetchContent_MakeAvailable(glm)

# FreeGLUT configuration
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/freeglut/CMakeLists.txt")
    set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
    set(FREEGLUT_REPLACE_GLUT ON CACHE BOOL "" FORCE)
    add_subdirectory(external/freeglut EXCLUDE_FROM_ALL)
    set(GLUT_TARGET freeglut_static)
    set(GLUT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/external/freeglut/include")
else()
    find_package(GLUT REQUIRED)
    set(GLUT_TARGET GLUT::GLUT)
    set(GLUT_INCLUDE_DIRS ${GLUT_INCLUDE_DIRS})
endif()

include_directories(
        include
        Header
        ${GLUT_INCLUDE_DIRS}
)

# Part 1 executable
add_executable(part1
        part1.cpp
        "Source Files/shaderClass.cpp"
        "Source Files/VBO.cpp"
        "Source Files/VAO.cpp"
        src/glad.c)
target_include_directories(part1 PRIVATE include Header ${GLUT_INCLUDE_DIRS})
target_link_libraries(part1 PRIVATE ${GLUT_TARGET} glm)

# Part 2 executable
add_executable(part2
        part2.cpp
        "Source Files/shaderClass.cpp"
        "Source Files/VBO.cpp"
        "Source Files/VAO.cpp"
        src/glad.c)
target_include_directories(part2 PRIVATE include Header ${GLUT_INCLUDE_DIRS})
target_link_libraries(part2 PRIVATE ${GLUT_TARGET} glm)

foreach(t part1 part2)
    add_custom_command(TARGET ${t} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Resource Files/shaders" $<TARGET_FILE_DIR:${t}>/shaders)
endforeach()