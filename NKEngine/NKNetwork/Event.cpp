#include "Event.h"
#include "EventContext.h"
#include "../NKEngineLog.h"

using namespace NKNetwork;

bool EventObject::onProcessFailed(EventContext& event_context, uint32_t transferred)
{
	NKENGINELOG_SOCKETERROR( GetLastError(), L"event object, failed, event type %d", event_context._type );
	return true;
}