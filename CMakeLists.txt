cmake_minimum_required(VERSION 3.25)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(ComputerGraphicsAssignment3 C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        glm
        URL https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip
)
FetchContent_MakeAvailable(glm)

set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/freeglut/CMakeLists.txt")
    add_subdirectory(external/freeglut EXCLUDE_FROM_ALL)
    set(GLUT_TARGET freeglut_static)
    set(GLUT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/external/freeglut/include")
else()
    if(NOT WIN32)
        find_package(GLUT QUIET)
    endif()
    if(GLUT_FOUND)
        set(GLUT_TARGET GLUT::GLUT)
    else()
        FetchContent_Declare(
                freeglut
                URL https://github.com/FreeGLUTProject/freeglut/releases/download/v3.8.0/freeglut-3.8.0.tar.gz
        )
        FetchContent_MakeAvailable(freeglut)
        set(GLUT_TARGET freeglut_static)
        set(GLUT_INCLUDE_DIRS "${freeglut_SOURCE_DIR}/include")
    endif()
endif()

add_executable(assignment3
        assignment3.cpp
        "Source Files/shaderClass.cpp"
        "Source Files/VBO.cpp"
        "Source Files/VAO.cpp"
        "Source Files/smfLoader.cpp"
        "Source Files/modelData.cpp"
        src/glad.c)
target_include_directories(assignment3 PRIVATE include Header ${GLUT_INCLUDE_DIRS})
if(GLUT_TARGET STREQUAL "freeglut_static")
    target_compile_definitions(assignment3 PRIVATE FREEGLUT_STATIC)
endif()
target_link_libraries(assignment3 PRIVATE ${GLUT_TARGET} glm)

if(MINGW)
    target_link_options(assignment3 PRIVATE -mconsole)
endif()

add_custom_command(TARGET assignment3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Resource Files/Shaders" "$<TARGET_FILE_DIR:assignment3>/shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/models" "$<TARGET_FILE_DIR:assignment3>/models")