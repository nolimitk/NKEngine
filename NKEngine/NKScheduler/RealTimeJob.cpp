#include "RealTimeJob.h"
#include "Scheduler.h"
#include "SchedulerInterface.h"

using namespace NKScheduler;

bool JobSlot::onProcess(NKNetwork::EventContext& event_context, uint32_t transferred)
{
	return internal::addJob(_job, _interval);
}
