#ifndef CHIP8_DISPLAY_H
#define CHIP8_DISPLAY_H

#include <cstdint>

namespace chip8
{
	class Display
	{
	public:
		Display();

		bool Draw(uint8_t x, uint8_t y, uint8_t height, const uint8_t* data);
		void Clear();

	private:
		void Update();

	private:
		uint64_t mRows[32] = {};
	};
}

#endif // CHIP8_DISPLAY_H