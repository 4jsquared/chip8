#include "keyboard.h"

#include <algorithm>
#include <cassert>

namespace
{
	// Original keyboard layout is
	//
	// 1 2 3 C
	// 4 5 6 D
	// 7 8 9 E
	// A 0 B F
	//
	// We'll map this from
	//
	// 1 2 3 4
	// q w e r
	// a s d f
	// z x c v
	SDL_Scancode kScancodes[] = {
		SDL_SCANCODE_X,
		SDL_SCANCODE_1,
		SDL_SCANCODE_2,
		SDL_SCANCODE_3,
		SDL_SCANCODE_Q,
		SDL_SCANCODE_W,
		SDL_SCANCODE_E,
		SDL_SCANCODE_A,
		SDL_SCANCODE_S,
		SDL_SCANCODE_D,
		SDL_SCANCODE_Z,
		SDL_SCANCODE_C,
		SDL_SCANCODE_4,
		SDL_SCANCODE_R,
		SDL_SCANCODE_F,
		SDL_SCANCODE_V,
	};

	constexpr uint8_t kNumKeys = 16;

	static_assert(sizeof(kScancodes) == sizeof(SDL_Scancode) * kNumKeys, "Incorrect number of scancodes");
}

namespace chip8
{
	void Keyboard::OnKeyDown(const SDL_Keysym& keysym)
	{
		auto it = std::find(std::begin(kScancodes), std::end(kScancodes), keysym.scancode);
		if (it != std::end(kScancodes))
		{
			size_t keyIndex = it - std::begin(kScancodes);
			uint16_t mask = (1u << keyIndex);
			mKeyState |= mask;
			mPressedState |= mask;
		}
	}

	void Keyboard::OnKeyUp(const SDL_Keysym& keysym)
	{
		auto it = std::find(std::begin(kScancodes), std::end(kScancodes), keysym.scancode);
		if (it != std::end(kScancodes))
		{
			size_t keyIndex = it - std::begin(kScancodes);
			uint16_t mask = ~(1u << keyIndex);

			// Clear only the key state, the pressed state is cleared on frame end.
			mKeyState &= mask;
		}
	}

	bool Keyboard::GetKeyState(uint8_t keyIndex)
	{
		assert(keyIndex < kNumKeys);
		return mKeyState & (1u << keyIndex);
	}

	bool Keyboard::GetKeyPressed(uint8_t keyIndex)
	{
		assert(keyIndex < kNumKeys);
		return mPressedState & (1u << keyIndex);
	}

	void Keyboard::ClearPressedKeys()
	{
		mPressedState = 0;
	}
}
