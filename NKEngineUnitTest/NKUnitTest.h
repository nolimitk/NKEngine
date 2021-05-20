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
	class NKUnitQueue
	{
	public:
		typedef std::function<bool(void)> UNITTEST_FUNC;

	protected:
		class TestNode
		{
		public:
			UNITTEST_FUNC _func;
			bool _benchmark;

		public:
			TestNode(UNITTEST_FUNC func, bool benchmark)
				:_func(func)
				, _benchmark(benchmark)
			{
			}
		};

	public:
		bool push(UNITTEST_FUNC func, bool benchmark)
		{
			_queue.push(TestNode(func, benchmark));
			return true;
		}
		bool run(bool benchmark_execute);

	protected:
		std::queue<TestNode> _queue;

	public:
		NKUnitQueue(void)
		{
		}
	};

	class NKReporter
	{
	protected:
		class TestResult
		{
		public:
			NKWString _filename;
			int _line;
			NKWString _msg;

		public:
			TestResult(NKWString& filename, int line, NKWString& msg)
				:_filename(filename)
				, _line(line)
				, _msg(msg)
			{
			}
		};

	public:
		bool push(NKWString& filename, int line, NKWString& msg)
		{
			_queue.push(TestResult(filename, line, msg));
			return true;
		}
		bool print(void);

	protected:
		std::queue<TestResult> _queue;
	};

	class NKUnitTestFramework
	{
	public:
		inline bool pushUnit(NKUnitQueue::UNITTEST_FUNC func, bool benchmark) { return _unit_queue.push(func, benchmark); }
		bool run(bool benchmark_execute);

	public:
		inline bool pushResult(NKWString& filename, int line, NKWString& msg) { return _reporter.push(filename, line, msg); }

	protected:
		NKUnitQueue _unit_queue;
		NKReporter _reporter;

	public:
		NKUnitTestFramework(void)
		{
		}
	};

	NKUnitTestFramework& getInstance(void);

	void thread_test(const uint32_t count, std::function<void(void)> func);
	bool register_test(NKUnitQueue::UNITTEST_FUNC func, bool benchmark = false);
	bool register_result(NKWString filename, int line, NKWString msg);

#define NKTEST(testname) bool NKTest_##testname##(void); \
bool NKTest_##testname##_flag = NKUnitTest::register_test(NKTest_##testname##); \
bool NKTest_##testname##(void)

#define NKTEST_BENCHMARK(testname) bool NKTest_Benchmark_##testname##(void); \
bool NKTest_Benchmark_##testname##_flag = NKUnitTest::register_test(NKTest_Benchmark_##testname##,true); \
bool NKTest_Benchmark_##testname##(void)

#define _TEST(expression) if( (expression) == false ){ NKUnitTest::register_result(__FILEW__,__LINE__,L ## #expression); }

//@deprecated
#define WAITFOR(owner,func) while (owner->_ ##func == false) { std::this_thread::yield(); }

#define WAITUNTIL(owner,func,time)					\
	{												\
		NKCore::NKClock clock;						\
		while (owner->_ ##func == false) {			\
			if (clock.getElapsedTime() > time) {	\
			NKUnitTest::register_result(__FILEW__, __LINE__, L"time over" ## #time); break;	\
		} std::this_thread::yield(); }				\
	}
}
#endif // __NKUNITTEST_HEADER__
