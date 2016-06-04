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
		int64_t getElapsedMicroSec(void)
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _start_tick).count();
		}

		void sleepAccurate(int64_t wait_time_us);

	protected:
		static const int64_t DEFAULT_ACCURACY_VALUE;
		static const int64_t ERROR_RANGE_VALUE;
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