cmake_minimum_required(VERSION 3.12.0)
project(OpenGL_CarGUI VERSION 1.0.0)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(OpenGL REQUIRED)

add_executable(OpenGL_CarGUI
    src/config.h
    src/config.cpp
    src/main.cpp 
    src/glad.c
    src/factories/factory.cpp
    src/systems/camera_system.cpp
    src/systems/render_system.cpp
    src/view/shader.cpp
    src/controller/app.cpp
)

target_include_directories(OpenGL_CarGUI 
    PRIVATE
    dependencies 
)

target_link_libraries(OpenGL_CarGUI
    glfw
    OpenGL::GL ${CMAKE_DL_LIBS}
)