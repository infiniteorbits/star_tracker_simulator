cmake_minimum_required(VERSION 2.8)
project( star_tracker_input )
# find_package( OpenCV REQUIRED )
find_package(GLUT)
find_package(OpenGL)

set(CMAKE_CXX_FLAGS "-g -Wall")

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
) # add ${OpenCV_LIBS} later 

#add_subdirectory(Hipparcos)

# Compile external dependencies 
add_subdirectory (external)

include_directories(
	external/glfw-3.1.2/include/GLFW/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	.
)


add_executable( star_tracker_input star_tracker_input.cpp Star.cpp 
	hip_parser.cpp shader.cpp Star.hpp hip_parser.hpp shader.hpp )
target_link_libraries(	star_tracker_input ${ALL_LIBS} )
