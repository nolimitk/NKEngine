#include "NKClock.h"
#include <thread>

using namespace NKCore;
using namespace std;

const int64_t NKClock::DEFAULT_ACCURACY_VALUE = 10 * 1000;

void NKClock::sleepAccurate(int64_t wait_time_us)
{
	int64_t sleep_start_tick = getElapsedMicroSec();
	int64_t sleep_gap = 0;
	int64_t diff = 0;

	do
	{
		diff = wait_time_us - sleep_gap;

		if (diff > DEFAULT_ACCURACY_VALUE)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(diff / 2));
		}
		else
		{
			std::this_thread::yield();
		}

		sleep_gap = getElapsedMicroSec() - sleep_start_tick;

	} while (diff > 0);
}