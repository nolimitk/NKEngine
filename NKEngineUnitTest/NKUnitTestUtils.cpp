#include "NKUnitTestUtils.h"
#include "NKUnittestLog.h"

NKUnitTest::ElapsedTime::ElapsedTime(NKWString&& name)
	:_name(std::move(name))
{
}

NKUnitTest::ElapsedTime::~ElapsedTime(void)
{
	uint64_t elapsed_tick = _clock.getElapsedMicroSec();
	NKUNITTESTLOG_INFO(L"%s, elapsed time, %tu us", _name.c_str(), elapsed_tick);
}