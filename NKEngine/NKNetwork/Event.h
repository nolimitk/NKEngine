#ifndef __EVENT_HEADER__
#define __EVENT_HEADER__
// @nolimitk
// 16.05.23
// event object

#include <WinSock2.h>
#include <memory>

namespace NKNetwork
{
	class EventContext;

	// interface
	class EventObject : public std::enable_shared_from_this<EventObject>
	{
	public:
		virtual bool onProcess(EventContext& event_context, uint32_t transferred) = 0;
		virtual bool onProcessFailed(EventContext& event_context, uint32_t transferred);

	public:
		EventObject(void) {}
		virtual ~EventObject(void) {}
	};
}

#endif // __EVENT_HEADER__