#include "sound_timer.h"

#include <algorithm>
#include <cassert>

namespace chip8
{
	SoundTimer::SoundTimer()
	{
		SDL_AudioSpec desiredSpec = {};
		SDL_AudioSpec obtainedSpec = {};

		desiredSpec.format = AUDIO_F32SYS;
		desiredSpec.channels = 1;
		desiredSpec.callback = &RenderCallback;
		desiredSpec.userdata = this;

		mDevice = SDL_OpenAudioDevice(NULL, SDL_FALSE, &desiredSpec, &obtainedSpec, SDL_TRUE);
		assert(mDevice > 0);

		assert(obtainedSpec.format == AUDIO_F32SYS);
		assert(obtainedSpec.channels == 1);

		// Let's create a 100Hz-ish sawtooth
		size_t numSamples = obtainedSpec.freq / 100;
		mWaveform.reserve(numSamples);
		for (size_t sample = 0; sample < numSamples; ++sample)
		{
			mWaveform.push_back(2.f * static_cast<float>(sample) / (numSamples - 1) - 1.f);
		}
	}

	SoundTimer::~SoundTimer()
	{
		SDL_CloseAudioDevice(mDevice);
	}

	void SoundTimer::SetValue(uint8_t value)
	{
		// Remove any timer that might also try to alter audio state
		if (mTimer)
		{
			SDL_RemoveTimer(mTimer);
			mTimer = 0;
		}

		SDL_PauseAudioDevice(mDevice, value == 0);

		if (value > 0)
		{
			// Value is decremented at a frequency of 60Hz - on running out, the buzzer stops
			mTimer = SDL_AddTimer(value * 1000 / 60, [](Uint32 interval, void * soundObject) -> Uint32
			{
				SDL_PauseAudioDevice(reinterpret_cast<SoundTimer*>(soundObject)->mDevice, true);
				return 0;
			}, this);
			assert(mTimer != 0);
		}
	}

	void SoundTimer::RenderCallback(void * soundObject, Uint8 * buffer, int bufferLen)
	{
		assert(bufferLen >= 0);
		assert(bufferLen % sizeof(float) == 0);
		assert(soundObject != nullptr);
		
		reinterpret_cast<SoundTimer*>(soundObject)->Render(
			reinterpret_cast<float*>(buffer),
			bufferLen / sizeof(float)
		);
	}

	void SoundTimer::Render(float * buffer, size_t bufferLen)
	{
		size_t srcOffset = mWaveformOffset;
		size_t dstOffset = 0;
		size_t copyCount = mWaveform.size() - srcOffset;

		while (dstOffset < bufferLen)
		{
			// Check that the copied data will fit - reduce if not
			// Don't forget to update the offset for the next time around
			size_t dstCount = bufferLen - dstOffset;
			if (copyCount > dstCount)
			{
				mWaveformOffset = mWaveform.size() - (copyCount - dstCount);
				copyCount = dstCount;
			}
			else
			{
				mWaveformOffset = 0;
			}

			std::copy_n(mWaveform.begin() + srcOffset, copyCount, buffer + dstOffset);

			dstOffset += copyCount;
			srcOffset = 0;
			copyCount = mWaveform.size();
		}
	}
}
