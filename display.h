#ifndef CHIP8_DISPLAY_H
#define CHIP8_DISPLAY_H

#include "SDL.h"

#include <cstdint>

namespace chip8
{
	class Display
	{
	public:
		void Render(SDL_Renderer* renderer);

		bool Draw(uint8_t x, uint8_t y, uint8_t height, const uint8_t* data);
		void Clear();

	private:
		// CHIP-8 uses 64x32 screen
		static constexpr size_t kHeight = 32;
		static constexpr size_t kWidth = 64;

		uint64_t mRows[kHeight] = {};
	};
}

#endif // CHIP8_DISPLAY_H