cmake_minimum_required(VERSION 3.20)

project(lesser-doom)

add_executable(${PROJECT_NAME} src/main.c)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake/)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC src)
add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC -mconsole ${OPENGL_LIBRARY} ${SDL2_LIBRARY} GLEW::glew )