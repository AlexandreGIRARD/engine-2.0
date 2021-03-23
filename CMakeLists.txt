cmake_minimum_required(VERSION 3.15.0)
project(engine VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/stb)
include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)

find_package(glfw3 REQUIRED)

add_executable(engine 
    src/main.cpp
    src/utils.cpp
    src/scene.cpp
    src/node.cpp
    src/camera.cpp
    src/material.cpp
    src/light.cpp
    src/camera.cpp
    src/mesh.cpp

    src/imgui/imgui.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui_impl_opengl3.cpp
    src/imgui/imgui_impl_glfw.cpp
    
    src/glad.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_compile_definitions(engine PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(engine glfw OpenGL::GL dl)

add_custom_command(
    TARGET engine POST_BUILD
    COMMAND mv engine ..
    VERBATIM)