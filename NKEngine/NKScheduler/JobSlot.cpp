#include "JobSlot.h"
#include "../NKEngineLog.h"

using namespace NKScheduler;

JobSlot::JobSlot(void)
//	:_short_term_queue(size_queue)
//	:_slotIndex(-1)
//	,_slotType(0)
//	,_executeSlotIndex(0)
{
}

JobSlot::~JobSlot(void)
{
}

/*
void EventSlot::UnRegister(void)
{
	_slotIndex = -1;
	_executeSlotIndex = 0;
	
	setNext(nullptr);
	setPrev(nullptr);
}

bool EventSlot::TryReserve(void)
{
	bool init = _reserved.exchange(true);
	if (init == true) return false;
	return true;
}

void EventSlot::ReleaseReserve(void)
{
	bool init = _reserved.exchange(false);
	if (init == false)
	{
		NKENGINELOG_ERROR( L"event slot, release reserve, it would be bug." );
		_ASSERT(0);
	}
}
*/

bool JobSlot::onProcess(NKNetwork::EventContext& event_context, uint32_t transferred)
{
	_ASSERT(event_context._type == NKNetwork::EVENTCONTEXTTYPE::SCHEDULER );

	NKNetwork::SchedulerContext& scheduler_context = static_cast<NKNetwork::SchedulerContext&>(event_context);
	NKENGINELOG_INFO( L"slot executed, %I64u", scheduler_context._param );

	// @TODO execution index를 비교해서 지연 체크?

	// @TODO job slot당 job의 count와 실행시간을 monitoring한다.
	RealTimeJob* pJob = _job_queue.popQueue();

	while (pJob != nullptr)
	{
		pJob->Process(scheduler_context._param);
	}
	
	return true;
}