#include "EventSlot.h"
#include "../NKEngineLog.h"

using namespace NKScheduler;

EventSlot::EventSlot(void)
	:_slotIndex(-1)
	,_slotType(0)
	,_executeSlotIndex(0)
{
}

EventSlot::~EventSlot(void)
{
}

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

bool EventSlot::onProcess(NKNetwork::EventContext& event_context, uint32_t transferred)
{
	_ASSERT(event_context._type == NKNetwork::EVENTCONTEXTTYPE::SCHEDULER );

	NKNetwork::SchedulerContext& scheduler_context = static_cast<NKNetwork::SchedulerContext&>(event_context);
	NKENGINELOG_INFO( L"slot executed, %I64u", scheduler_context._param );
	
	return true;
}