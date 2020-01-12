#ifndef CHIP8_SOUND_TIMER_H
#define CHIP8_SOUND_TIMER_H

#include "SDL.h"

#include <vector>

namespace chip8
{
	class SoundTimer
	{
	public:
		SoundTimer();
		~SoundTimer();

		void SetValue(uint8_t value);

	private:
		static void RenderCallback(void * soundObject,
			Uint8 * buffer,
			int bufferLen);

		void Render(float * buffer, size_t bufferLen);

	private:
		SDL_AudioDeviceID mDevice = 0;;
		SDL_TimerID mTimer = 0;

		std::vector<float> mWaveform;
		size_t mWaveformOffset = 0;
	};
}

#endif // CHIP8_SOUND_TIMER_H