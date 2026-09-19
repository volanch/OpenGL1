cmake_minimum_required(VERSION 3.25)
project(ComputerGraphicsProject C CXX)

set(CMAKE_CXX_STANDARD 17)

# Автоматически скачиваем GLFW и GLM через FetchContent
include(FetchContent)

# 1. Скачиваем GLFW
FetchContent_Declare(
        glfw
        URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
)
FetchContent_MakeAvailable(glfw)

# 2. Скачиваем GLM через Git-репозиторий
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# Создаем исполняемый файл и добавляем GLAD (glad.c)
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

# Подключаем пути к заголовочным файлам (включая GLAD)
target_include_directories(ComputerGraphicsProject PRIVATE include)

# Линкуем библиотеки
target_link_libraries(ComputerGraphicsProject PRIVATE glfw glm)

target_include_directories(ComputerGraphicsProject PRIVATE Header include)

file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/Resource Files/Shaders/*.vert"
        "${CMAKE_SOURCE_DIR}/Resource Files/Shaders/*.frag")
file(COPY ${SHADER_FILES} DESTINATION ${CMAKE_BINARY_DIR})

