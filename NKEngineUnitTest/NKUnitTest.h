#pragma once
#ifndef __NKUNITTEST_HEADER__
#define __NKUNITTEST_HEADER__
// @nolimitk
// 16.04.15
// unit test

#include "NKEngine.h"

#include <queue>
#include <functional>

#include "NKUnittestLog.h"
#include "NKUnitTestUtils.h"

namespace NKUnitTest
{
	class NKUnitTestFramework
	{
	public:
		typedef std::function<bool(void)> UNITTEST_FUNC;

	protected:
		class TestNode
		{
		public:
			UNITTEST_FUNC _func;
			bool _benchmark;
		};

	public:
		bool push(UNITTEST_FUNC func, bool benchmark)
		{
			TestNode test_node;
			test_node._func = func;
			test_node._benchmark = benchmark;
			_queue.push(test_node);
			return true;
		}

		bool run(bool benchmark_execute);

	protected:
		std::queue<TestNode> _queue;
		bool _benchmark_execute;

	public:
		NKUnitTestFramework(void)
			:_benchmark_execute(true)
		{
		}
	};

	NKUnitTestFramework& getInstance(void);

	void thread_test(const uint32_t count, std::function<void(void)> func);
	bool register_test(NKUnitTestFramework::UNITTEST_FUNC func, bool benchmark = false);
	
#define NKTEST(testname) bool NKTest_##testname##(void); \
bool NKTest_##testname##_flag = NKUnitTest::register_test(NKTest_##testname##); \
bool NKTest_##testname##(void)

#define NKTEST_BENCHMARK(testname) bool NKTest_Benchmark_##testname##(void); \
bool NKTest_Benchmark_##testname##_flag = NKUnitTest::register_test(NKTest_Benchmark_##testname##,true); \
bool NKTest_Benchmark_##testname##(void)

}

#endif // __NKUNITTEST_HEADER__
