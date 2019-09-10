# CMakeList.txt : CMake project for chip8, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

# Add source to this project's executable.
add_executable (chip8
	"chip8.cpp"
	"display.cpp"
	"program.cpp"
	"timer.cpp"
	)

# TODO: Add tests and install targets if needed.
