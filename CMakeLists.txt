cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

project(lesser-doom)

add_executable(${PROJECT_NAME} src/main.c)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake/)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC src)
add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_LIBRARY} ${SDL2_LIBRARY} GLEW::glew Threads::Threads -lm)