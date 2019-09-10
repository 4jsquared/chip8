#include "display.h"

#include <algorithm>

#include <cassert>
#include <cstdio>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

namespace
{
	constexpr char kResetToOrigin[] = "\x1b[1;1f";
}

namespace chip8
{
	Display::Display()
	{
#ifdef _WIN32
		// Enable control sequences
		// See https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		assert(console != INVALID_HANDLE_VALUE);

		DWORD mode = 0;
		BOOL result = GetConsoleMode(console, &mode);
		assert(result == TRUE);

		mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		result = SetConsoleMode(console, mode);
		assert(result == TRUE);
#endif
	}

	bool Display::Draw(uint8_t x, uint8_t y, uint8_t height, const uint8_t* data)
	{
		// Drawing assuming 8 wide sprites, with each bit representing a pixel
		// (so a single uint8_t is one line of the sprite)
		assert(x + 8 <= 64);
		assert(y + height <= 32);

		// Need to track if any bits were flipped
		bool flipped = false;

		// Drawing occurs by flipping the state of each affected bit
		for (uint8_t row = 0; row < height; row++)
		{
			uint64_t newData = static_cast<uint64_t>(data[row]) << (64 - 8 - x);
			flipped |= static_cast<bool>(mRows[y + row] & newData);
			mRows[y + row] ^= newData;
		}

		Update();

		return flipped;
	}

	void Display::Clear()
	{
		// Clear the display
		std::fill(std::begin(mRows), std::end(mRows), 0);

		Update();
	}

	void Display::Update()
	{
		// Reset to the start origin
#ifdef _WIN32
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		WriteConsole(console, kResetToOrigin, sizeof(kResetToOrigin) - 1, nullptr, nullptr);
#else
		printf(kResetToOrigin);
#endif

		// Print rows
		for (uint64_t rowData : mRows)
		{
			char buffer[66] = {};
			for (size_t i = 0; i < 64; i++)
				buffer[i] = (rowData & (1ull << (63 - i))) ? '#' : ' ';
			buffer[64] = '\n';

#ifdef _WIN32
			WriteConsole(console, buffer, sizeof(buffer) - 1, nullptr, nullptr);
#else
			printf(buffer);
#endif
		}
	}
}