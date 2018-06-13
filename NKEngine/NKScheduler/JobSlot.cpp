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