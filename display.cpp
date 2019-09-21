#include "display.h"

#include <algorithm>
#include <cassert>

namespace chip8
{
	bool Display::Draw(uint8_t x, uint8_t y, uint8_t height, const uint8_t* data)
	{
		// Drawing assuming 8 wide sprites, with each bit representing a pixel
		// (so a single uint8_t is one line of the sprite)
		// Sprite may be placed overlapping the edge of the screen on the right hand side
		assert(x < 64);
		assert(y + height <= 32);

		// Need to track if any bits were flipped
		bool flipped = false;

		// Drawing occurs by flipping the state of each affected bit
		for (uint8_t row = 0; row < height; row++)
		{
			uint8_t shiftUp = (128 - 8 - x) % 64;
			uint8_t shiftDown = (8 + x) % 64;
			uint64_t newData = static_cast<uint64_t>(data[row]) << shiftUp;

			// Wrap around on overflow
			newData |= static_cast<uint64_t>(data[row]) >> shiftDown;

			flipped |= static_cast<bool>(mRows[y + row] & newData);
			mRows[y + row] ^= newData;
		}

		return flipped;
	}

	void Display::Clear()
	{
		// Clear the display
		std::fill(std::begin(mRows), std::end(mRows), 0);
	}

	void Display::Render(SDL_Renderer* renderer)
	{
		// Clear anything on the display
		int result = SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		assert(result == 0);
		result = SDL_RenderClear(renderer);
		assert(result == 0);

		// Check how much space we have
		int width, height;
		result = SDL_GetRendererOutputSize(renderer, &width, &height);
		assert(result == 0);
		int pixelWidth = width / kWidth;
		int pixelHeight = height / kHeight;

		// Draw display
		result = SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		assert(result == 0);
		for (size_t row = 0; row < kHeight; row++)
		{
			uint64_t mask = 0x1ull << 63;
			for (size_t col = 0; col < kWidth; col++)
			{
				if (mask & mRows[row])
				{
					SDL_Rect rect{ col * pixelWidth, row * pixelHeight, pixelWidth, pixelHeight };
					result = SDL_RenderFillRect(renderer, &rect);
					assert(result == 0);
				}
				mask >>= 1;
			}
		}

	}
}