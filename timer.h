#ifndef CHIP8_TIMER_H
#define CHIP8_TIMER_H

#include <chrono>

#include <cstdint>

namespace chip8
{
	class Timer
	{
		using ClockType = std::chrono::steady_clock;

	public:
		void SetValue(uint8_t value);
		uint8_t GetValue();

	private:
		ClockType::time_point mLastUpdate = ClockType::now();
		uint8_t mLastValue = 0;
	};
}

#endif // CHIP8_TIMER_H