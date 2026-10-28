cmake_minimum_required(VERSION 3.16)
project(autopolyphonic LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# glad: generated OpenGL 3.3 core loader
add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)

add_executable(autopolyphonic src/main.cpp)
target_link_libraries(autopolyphonic PRIVATE SDL2::SDL2 OpenGL::GL glad)
target_compile_options(autopolyphonic PRIVATE -Wall -Wextra)
