cmake_minimum_required(VERSION 2.8)
project( star_tracker_input )
find_package( OpenCV REQUIRED )
find_package(GLUT)
find_package(OpenGL)

set(CMAKE_CXX_FLAGS "-g -Wall")

#add_subdirectory(Hipparcos)
add_executable( star_tracker_input star_tracker_input.cpp Star.cpp hip_parser.cpp Star.hpp hip_parser.hpp )
target_link_libraries( star_tracker_input ${OpenCV_LIBS} )