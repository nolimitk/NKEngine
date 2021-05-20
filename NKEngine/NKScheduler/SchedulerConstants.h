#pragma once
#ifndef __SCHEDULER_CONSTANTS_HEADER__
#define __SCHEDULER_CONSTANTS_HEADER__
// @nolimitk
// 18.06.30
// scheduler constants

#include <cstdint>
#include <chrono>

namespace NKScheduler
{
	using namespace std::literals;

	// @nolimitk 50ms * 400, 20초, 20초 이하의 job 실행이 빈번할것으로 예상, 상황에 맡게 조정가능
	const uint32_t DEFAULT_JOBSLOT_SHORTTERM_SIZE = 400;

	const std::chrono::microseconds SCHEDULER_INTERVAL_MARGIN = 500us;
	const std::chrono::microseconds SCHEDULER_INTERVAL_UNIT = 50ms;
}

#endif // __SCHEDULER_CONSTANTS_HEADER__