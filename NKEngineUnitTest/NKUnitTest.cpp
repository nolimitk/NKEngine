#include "NKUnitTest.h"
#include <thread>

bool NKUnitTest::NKUnitQueue::run(bool benchmark_execute)
{
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

	return true;
}

bool NKUnitTest::NKReporter::print(void)
{
	while (_queue.empty() == false)
	{
		TestResult test_result = _queue.front();
		_queue.pop();
		NKUNITTESTLOG_ERROR(L"unittest failed, %s, %d, %s", test_result._filename.c_str(), test_result._line, test_result._msg.c_str());
	}

	return false;
}

bool NKUnitTest::NKUnitTestFramework::run(bool benchmark_execute)
{
	NKUNITTESTLOG_INFO(L"nkunittest has started");

	if (_unit_queue.run(benchmark_execute) == false)
	{
		NKUNITTESTLOG_INFO(L"nkunittest failed to run");
		_ASSERT(false);
		return false;
	}

	_reporter.print();

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

bool NKUnitTest::register_test(NKUnitQueue::UNITTEST_FUNC func, bool benchmark)
{
	return getInstance().pushUnit(func, benchmark);
}

bool NKUnitTest::register_result(NKWString filename, int line, NKWString msg)
{
	return getInstance().pushResult(filename, line, msg);
}
