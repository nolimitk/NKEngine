#include "NKUnitTest.h"
#include <thread>

bool NKUnitTest::NKUnitTestFramework::run(bool benchmark_execute)
{
	NKUNITTESTLOG_INFO(L"nkunittest has started");

	while (_queue.empty() == false)
	{
		TestNode test_node = _queue.front();
		_queue.pop();
		if (benchmark_execute == false && test_node._benchmark == true)
		{
			continue;
		}
		test_node._func();
	}

	NKUNITTESTLOG_INFO(L"nkunittest has ended");

	NKUnittestLogSingleton::destroy();

	return true;
}

NKUnitTest::NKUnitTestFramework& NKUnitTest::getInstance(void)
{
	static NKUnitTest::NKUnitTestFramework instance;
	return instance;
}

void NKUnitTest::thread_test(const uint32_t count, std::function<void(void)> func)
{
	std::thread* t = new std::thread[count];
	for (uint32_t i = 0; i < count; ++i)
	{
		t[i] = std::thread(func);
	}

	for (uint32_t i = 0; i < count; ++i)
	{
		t[i].join();
	}
	SAFE_DELETE_ARRAY(t);
}

bool NKUnitTest::register_test(NKUnitTestFramework::UNITTEST_FUNC func, bool benchmark)
{
	return getInstance().push(func, benchmark);
}