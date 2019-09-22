#ifndef CHIP8_KEYBOARD_H
#define CHIP8_KEYBOARD_H

#include "SDL.h"

namespace chip8
{
	class Keyboard
	{
	public:
		void OnKeyDown(const SDL_Keysym& keysym);
		void OnKeyUp(const SDL_Keysym& keysym);

		bool GetKeyState(uint8_t keyIndex);
		bool GetKeyPressed(uint8_t keyIndex);
		void ClearPressedKeys();

	private:
		uint16_t mKeyState = 0;
		uint16_t mPressedState = 0;
	};
}

#endif // CHIP8_DISPLAY_H