#include "Serializer.h"
#include "../NKEngineLog.h"
#include "RealTimeJob.h"

using namespace NKScheduler;

Serializer::Serializer(void)
	: _count(0)
	//, _current_thread_id(0)
{
}

Serializer::~Serializer(void)
{
}

inline bool IsMyown(int count)
{
	return count != 1;
}

bool Serializer::execute(uint64_t execution_index, const RealTimeJobSP& job)
{
	DWORD empty_thread_id = 0;

	int result_count = ++_count;
	_queue.push(job);

	if (IsMyown(result_count))
	{
		return false;
	}

	do
	{
		//_current_thread_id = GetCurrentThreadId();

		onExecute(execution_index);

		//_current_thread_id = empty_thread_id;

	} while (--_count > 0);

	return true;
}

void Serializer::onExecute(uint64_t execution_index)
{
	auto iter = _queue.popQueue();
	while (iter != nullptr)
	{
		iter->getValue()->onExecute(execution_index);
		iter = iter->getNext();
	}
}

//bool Serializer::addJob(const RealTimeJobSP& job, uint64_t reserve_execution_index)
//{
//	if (job == nullptr){ _ASSERT(false); return false;}
//	//if (job->getNext() != nullptr) { _ASSERT(false); return false; }
//	
//	uint32_t round_slice = reserve_execution_index % DEFAULT_JOBSLOT_SHORTTERM_SIZE;
//
//	if (_shortterm_slot[round_slice].push(job) == false)
//	{
//		//NKENGINELOG_ERROR(L"[SERIALIZER], failed to push the job into queue,tick %u, slice %d", tick, slice);
//		return false;
//	}
//	
//#if defined _SCHEDULER_DEBUG_LOG_
//	NKENGINELOG_INFO( L"[SERIALIZER],add job, tick %u, slice %d, slot %I64u", tick, slice, pJob->getExecuteSlotIndex() );
//#endif
//
//	return true;
//}
//
//bool Serializer::execute(uint64_t execution_index)
//{
//#if defined _SCHEDULER_DEBUG_LOG_
//	//NE_DEBUGLOG( L"serializer executed, %I64u", execute_index );
//#endif
//
//	// @TODO job 처리 시간을 측정하여 monitoring 한다.
//	NKCore::NKClock clock;
//	int64_t execute_time = 0;
//
//	//_lastReservedTick = (uint32_t)(-1);
//
//	int64_t round_slice = execution_index % DEFAULT_JOBSLOT_SHORTTERM_SIZE;
//
//	NKCore::TWaitFreeQueue2<RealTimeJobSP>::iterator_type node = _shortterm_slot[round_slice].popQueue();
//	NKCore::TWaitFreeQueue2<RealTimeJobSP>::iterator_type next_node = nullptr;
//	
//	/// execution
//	while (node != nullptr)
//	{
//		// @nolimitk job을 실행중에 다시 등록할 수 있도록 초기화를 먼저 한다.
//		next_node = node->getNext();
//		
//		//job->UnRegister();
//		//job->ReleaseReserve();
//
//		node->getValue()->onExecute(execution_index);
//		NKENGINELOG_INFO(L"[SCHEDULER],%I64u,job executed,slot %I64u", execution_index, round_slice);
//
//		node = next_node;
//	}
//	///
//
//	return true;
//}