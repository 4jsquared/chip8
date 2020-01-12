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

		// Record data for later use
		mDeviceFrequency = obtainedSpec.freq;

		// Let's create a 100Hz-ish sawtooth
		size_t numSamples = obtainedSpec.freq / 100;
		mWaveform.reserve(numSamples);
		for (size_t sample = 0; sample < numSamples; ++sample)
		{
			mWaveform.push_back(2.f * static_cast<float>(sample) / (numSamples - 1) - 1.f);
		}

		// Start the audio device
		SDL_PauseAudioDevice(mDevice, false);
	}

	SoundTimer::~SoundTimer()
	{
		SDL_CloseAudioDevice(mDevice);
	}

	void SoundTimer::SetValue(uint8_t value)
	{
		// Value is the number of 1/60s to play for, i.e. the timer decrements at 60Hz
		mRemainingSamples.store(value * mDeviceFrequency / 60);
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
		size_t dstOffset = 0;
		size_t srcOffset = mWaveformOffset;

		uint32_t remainingSamples, finalRemainingSamples;
		do
		{
			remainingSamples = mRemainingSamples.load(std::memory_order_relaxed);
			finalRemainingSamples = remainingSamples > bufferLen ? remainingSamples - bufferLen : 0;
		} while (mRemainingSamples.compare_exchange_weak(remainingSamples, finalRemainingSamples) == false);

		while (dstOffset < bufferLen)
		{
			if (remainingSamples > 0)
			{
				size_t copyCount = std::min({
					mWaveform.size() - srcOffset,
					static_cast<size_t>(remainingSamples),
					bufferLen - dstOffset
					});

				std::copy_n(mWaveform.begin() + srcOffset, copyCount, buffer + dstOffset);

				dstOffset += copyCount;
				remainingSamples -= copyCount;
				srcOffset += copyCount;
				srcOffset %= mWaveform.size();
			}
			else
			{
				// Buzzer has stopped, fill with silence
				std::fill_n(buffer + dstOffset, bufferLen - dstOffset, 0.f);
				break;
			}
		}

		// Update the offset for next time
		mWaveformOffset = remainingSamples > 0 ? srcOffset : 0;
	}
}
