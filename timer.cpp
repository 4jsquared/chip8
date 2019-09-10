#include "timer.h"

namespace chip8
{
	void Timer::SetValue(uint8_t value)
	{
		mLastUpdate = ClockType::now();
		mLastValue = value;
	}

	uint8_t Timer::GetValue()
	{
		constexpr uint32_t kUpdateFrequency = 60;

		ClockType::time_point now = ClockType::now();
		ClockType::duration timeSinceUpdate = now - mLastUpdate;

		// Timers decrement at frequency of 60 hertz
		uint64_t decrementCount = std::chrono::duration_cast<std::chrono::seconds>(timeSinceUpdate * kUpdateFrequency).count();

		// Update the last value, making sure the timer doesn't loop?
		mLastValue = decrementCount > mLastValue ? 0 : mLastValue - decrementCount;

		// Update the last time the value was changed
		mLastUpdate += decrementCount * std::chrono::duration_cast<ClockType::duration>(std::chrono::seconds(1) / kUpdateFrequency);

		return mLastValue;
	}
}