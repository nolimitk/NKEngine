#include "NKUnitTestUtils.h"
#include "NKUnittestLog.h"

NKUnitTest::ElapsedTime::ElapsedTime(NKWString&& name)
	:_name(std::move(name))
{
}

NKUnitTest::ElapsedTime::~ElapsedTime(void)
{
	std::chrono::microseconds elapsed_tick = _clock.getElapsedTime();
	NKUNITTESTLOG_INFO(L"%s, elapsed time, %I64d us", _name.c_str(), elapsed_tick.count());
}