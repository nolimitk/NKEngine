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

const int64_t NKClock::DEFAULT_ACCURACY_VALUE = 10 * 1000;
const int64_t NKClock::ERROR_RANGE_VALUE = 100;

void NKClock::sleepAccurate(int64_t wait_time_us)
{
	if (wait_time_us == 0) return;

	int64_t sleep_start_tick = getElapsedMicroSec();
	int64_t sleep_gap = 0;
	int64_t diff = 0;

#if defined _NKCLOCK_DEBUG_
	vector<int64_t> _list_sleepgap;
#endif

	do
	{
		diff = wait_time_us - sleep_gap;
		
		if (diff > DEFAULT_ACCURACY_VALUE)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(diff>>1));
		}
		else
		{
			std::this_thread::yield();
		}

		sleep_gap = getElapsedMicroSec() - sleep_start_tick;
#if defined _NKCLOCK_DEBUG_
		_list_sleepgap.push_back(sleep_gap);
#endif
	} while (sleep_gap < wait_time_us - ERROR_RANGE_VALUE);

#if defined _NKCLOCK_DEBUG_
	for_each(_list_sleepgap.begin(), _list_sleepgap.end(), [&wait_time_us](int64_t tick) {
		NKENGINELOG_INFO(L"tick %d, %d", tick, wait_time_us);
	});
	NKENGINELOG_INFO(L"loop count %d", _list_sleepgap.size());
#endif
}