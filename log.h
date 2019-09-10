#ifndef CHIP8_LOG_H
#define CHIP8_LOG_H

#include <cstdio>

#define LOG(message, ...) do { \
	printf(message "\n", __VA_ARGS__); \
} while(0)

#endif // CHIP8_LOG_H