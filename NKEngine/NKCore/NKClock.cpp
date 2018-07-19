#include "NKClock.h"
#include <thread>

//#define _NKCLOCK_DEBUG_

#if defined _NKCLOCK_DEBUG_
#include <vector>
#include <algorithm>
#include "../NKEngineLog.h"
#endif

using namespace NKCore;
using namespace std;

const std::chrono::microseconds NKClock::DEFAULT_ACCURACY_VALUE = 10000us;
const std::chrono::microseconds NKClock::ERROR_RANGE_VALUE = 100us;

void NKClock::sleepAccurate(const std::chrono::microseconds& wait_time)
{
	if (wait_time == 0us) return;

	std::chrono::microseconds sleep_start_tick = getElapsedTime();
	std::chrono::microseconds sleep_gap = 0us;
	std::chrono::microseconds diff = 0us;

#if defined _NKCLOCK_DEBUG_
	vector<int64_t> _list_sleepgap;
#endif

	do
	{
		diff = wait_time - sleep_gap;

		if (diff > DEFAULT_ACCURACY_VALUE)
		{
			std::this_thread::sleep_for(diff/2);
		}
		else
		{
			std::this_thread::yield();
		}

		sleep_gap = getElapsedTime() - sleep_start_tick;
#if defined _NKCLOCK_DEBUG_
		_list_sleepgap.push_back(sleep_gap);
#endif
	} while (sleep_gap < wait_time - ERROR_RANGE_VALUE);

#if defined _NKCLOCK_DEBUG_
	for( const int64_t& tick : _list_sleepgap)
	{
		NKENGINELOG_INFO(L"tick %d, %d", tick, wait_time_us);
	}
	NKENGINELOG_INFO(L"loop count %d", _list_sleepgap.size());
#endif
}