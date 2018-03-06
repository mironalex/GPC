cmake_minimum_required(VERSION 3.9)
project(hw2)

set(CMAKE_CXX_STANDARD 11)

add_executable(hw2 main.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

target_link_libraries(hw2 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )