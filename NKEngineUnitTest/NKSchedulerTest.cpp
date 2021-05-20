// @nolimitk
// 16.05.28
// scheduler test

#include "NKEngine.h"
#include "NKUnittest.h"
#include "NKUnittestLog.h"

using namespace std;
using namespace std::chrono;

class MockJob : public NKScheduler::RealTimeJob
{
public:
	bool _onExecute;
	NKCore::NKClock _clock;
	milliseconds _elapsed_time;

public:
	virtual bool onExecute(uint64_t execute_index)
	{
		_elapsed_time = duration_cast<milliseconds>(_clock.getElapsedTime());
		_onExecute = true;
		return true;
	}

	MockJob(void)
		: RealTimeJob()
		, _elapsed_time(milliseconds::zero())
		, _onExecute(false)
	{}
};

NKTEST(Scheduler_Test)
{
	const milliseconds LOW_TEST_MARGIN = 49ms;
	const milliseconds HIGH_TEST_MARGIN = 10ms;

	{
		const milliseconds reserve_interval = 50ms;

		NKScheduler::SerializerSP serializer = std::make_shared<NKScheduler::Serializer>();
		assert(serializer);
		shared_ptr<MockJob> job = std::make_shared<MockJob>();
		assert(job);
		
		assert(NKScheduler::addJob(serializer, job, reserve_interval) == true);
		
		WAITUNTIL(job, onExecute, 100ms);
		assert(job->_elapsed_time > reserve_interval - LOW_TEST_MARGIN && job->_elapsed_time < reserve_interval + HIGH_TEST_MARGIN);
	}

	{
		const milliseconds reserve_interval = 50ms;

		NKScheduler::SerializerSP serializer = std::make_shared<NKScheduler::Serializer>();
		assert(serializer);
		shared_ptr<MockJob> job = std::make_shared<MockJob>();
		assert(job);

		// ?? 왜 같은 잡을 같은 시간에 2개? 무슨테스트야?
		assert(NKScheduler::addJob(serializer, job, reserve_interval) == true);
		assert(NKScheduler::addJob(serializer, job, reserve_interval) == true);

		WAITUNTIL(job, onExecute, 100ms);
		assert(job->_elapsed_time > reserve_interval - LOW_TEST_MARGIN && job->_elapsed_time < reserve_interval + HIGH_TEST_MARGIN);
	}

	return true;
}