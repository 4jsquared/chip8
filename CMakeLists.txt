# CMakeList.txt : CMake project for chip8, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Add source to this project's executable.
add_executable (chip8 WIN32
	"chip8.cpp"
	"display.cpp"
	"image.cpp"
	"program.cpp"
	"program_select.cpp"
	"system.cpp"
	"timer.cpp"
	)

target_link_libraries(chip8
	PRIVATE SDL2::SDL2-static SDL2::SDL2main SDL2_ttf::SDL2_ttf
	)

# TODO: Add tests and install targets if needed.
