#pragma once
#ifndef __EXECUTIONINDEX_HEADER__
#define __EXECUTIONINDEX_HEADER__
// @nolimitk
// 21.01.18
// execution index

#include "SchedulerConstants.h"

namespace NKScheduler
{
	using namespace std::literals;

	class ExecutionIndex
	{
	public:
		inline operator uint64_t(void) const { return _v; }
		inline ExecutionIndex increase(void) { return ++_v; }
		inline ExecutionIndex add(const std::chrono::milliseconds& interval)
		{
			// @TODO interval의 max값을 정한다.
			// index 0~49 -> 1, 50~99 -> 1, 100~149 -> 2, 150->199 -> 3, 200->249 -> 4,,, 950~999 -> 
			if (interval < 50ms) return _v + 1;
			int64_t slice = (interval / SCHEDULER_INTERVAL_UNIT);
			if (slice < 0 || slice >= DEFAULT_JOBSLOT_SHORTTERM_SIZE)
			{
				_ASSERT(false);
				return DEFAULT_JOBSLOT_SHORTTERM_SIZE;
			}

			return _v + slice;
		}

	protected:
		uint64_t _v;

	public:
		ExecutionIndex(void) :_v(0) {}
		ExecutionIndex(uint64_t value) :_v(value) {}
		virtual ~ExecutionIndex(void) {}
	};
}

#endif // __EXECUTIONINDEX_HEADER__