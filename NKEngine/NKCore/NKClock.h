#pragma once
#ifndef __NKCLOCK_HEADER__
#define __NKCLOCK_HEADER__
// @nolimitk
// 16.05.11
// clock
// wraps std::high_resolution_clock

#include <stdint.h>
#include <chrono>

namespace NKCore
{
	// margin of error is under 1ms(1000us)
	class NKClock
	{
	public:
		inline std::chrono::microseconds getElapsedTime(void) const
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _start_tick);
		}

		void sleepAccurate(const std::chrono::microseconds& wait_time);
		inline void sleepAccurate(const std::chrono::milliseconds& wait_time) { sleepAccurate(std::chrono::microseconds(wait_time)); }

	protected:
		static const std::chrono::microseconds DEFAULT_ACCURACY_VALUE;
		static const std::chrono::microseconds ERROR_RANGE_VALUE;
		std::chrono::time_point<std::chrono::high_resolution_clock> _start_tick;

	public:
		NKClock(void)
			:_start_tick(std::chrono::high_resolution_clock::now())
		{
		}
		virtual ~NKClock(void) {}
	};
}

#endif // __NKCLOCK_HEADER__