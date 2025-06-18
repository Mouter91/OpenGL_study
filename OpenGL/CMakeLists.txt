cmake_minimum_required(VERSION 3.10)
project(OpenGL_study_ver1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(Light_study
    src/main.cpp
    src/glad.c
    src/config/config.h
    src/config/config.cpp
    src/shader/shader.cpp
    src/stb_image.h
    src/stb_image.cpp
)

target_include_directories(Light_study PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(Light_study PRIVATE
    glfw
    GL
    X11
    pthread
    Xrandr
    Xi
    dl
)

