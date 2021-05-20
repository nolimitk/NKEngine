#include "Scheduler.h"
#include "../NKEngineLog.h"

using namespace NKScheduler;
using namespace std;

thread_local Scheduler NKScheduler::_scheduler;

Scheduler::Scheduler(void)
	:_in_workerthread(false)
{
}

Scheduler::~Scheduler(void)
{
}

bool Scheduler::addJob(const RealTimeJobSP& job, ExecutionIndex reserve_index)
{
	NKENGINELOG_ASSERT_RETURN(job != nullptr);

	uint32_t round_slice = reserve_index % DEFAULT_JOBSLOT_SHORTTERM_SIZE;

	if (_shortterm_slot[round_slice].push(job) == false)
	{
		NKENGINELOG_ERROR(L"[SCHEDULER],scheduler addjob failed,reserve %I64u,slot %u",
			reserve_index, round_slice);
		return false;
	}

	NKENGINELOG_INFO(L"[SCHEDULER],thread %u,%I64u,add job,slot %u,reserve %I64u",
		this_thread::get_id(), (uint64_t)_execution_index, round_slice, (uint64_t)reserve_index);

	return true;
}

bool Scheduler::execute(void)
{
	/// update execution index
	ExecutionIndex execution_index = _execution_index.increase();
	///

	// @TODO job 처리 시간을 측정하여 monitoring 한다.
	int64_t execute_time = 0;

	int64_t round_slice = execution_index % DEFAULT_JOBSLOT_SHORTTERM_SIZE;

	auto node = _shortterm_slot[round_slice].popQueue();
	RealTimeJobQueue::iterator_type next_node = nullptr;

	/// execution
	while (node != nullptr)
	{
		next_node = node->getNext();

		if (node->getValue()->_serializer)
		{
			node->getValue()->_serializer->execute(execution_index, node->getValue());
		}
		else
		{
			//@TODO serialize가 없으면 안된다.
		}

		NKENGINELOG_INFO(L"[SCHEDULER],thread %u,%I64u,job executed,slot %I64u",
			this_thread::get_id(), (uint64_t)execution_index, round_slice);

		node = next_node;
	}
	///

	return true;
}