#include "program.h"

#include <cassert>
#include <cstdio>

#include <vector>

int main(int argc, const char* argv[])
{
	assert(argc == 2);

	// Grab the file passed in on the command line
	FILE* file = fopen(argv[1], "rb");
	assert(file != nullptr);

	int result = fseek(file, 0, SEEK_END);
	assert(result == 0);

	long fileSize = ftell(file);
	assert(fileSize > 0);
	result = fseek(file, 0, SEEK_SET);
	assert(fileSize);

	std::vector<uint8_t> fileData(fileSize);
	size_t readElements = fread(fileData.data(), fileData.size(), 1, file);
	assert(readElements == 1);

	fclose(file);

	// Create and run the program
	chip8::Program program(fileData.data(), fileData.size());

	program.Execute();

	return 0;
}
