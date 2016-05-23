#pragma once
#ifndef __EVENTCONTEXT_HEADER__
#define __EVENTCONTEXT_HEADER__
// @nolimitk
// 13.11.12
// buffer context

#include "../NKCore.h"

namespace NKNetwork
{
	class AsyncSocket;
	class EventObject;

	enum class EVENTCONTEXTTYPE : int
	{
		NOTHING,
		ACCEPT,
		CONNECT,
		RECEIVE,
		SEND,
		SCHEDULER,
		CUSTOM
	};

	class EventContext : public OVERLAPPED
	{
	public:
		EVENTCONTEXTTYPE _type;
		std::shared_ptr<EventObject> _event_object;

	public:
		EventContext(void);
	};

	class AcceptContext : public EventContext
	{
	public:
		const static int BUFFER_SIZE = (sizeof(SOCKADDR_IN) + 16) * 2;

	public:
		char _outputBuffer[BUFFER_SIZE];
		std::shared_ptr<AsyncSocket> _accept_socket;

	public:
		AcceptContext(void);
	};

	class ConnectContext : public EventContext
	{
	public:
		SOCKADDR_IN _server_addr;

	public:
		ConnectContext(void);
	};

	class SchedulerContext : public EventContext
	{
	public:
		uint64_t _param;

	public:
		SchedulerContext(void);
	};

	class WSABUFContext : public EventContext
	{
	public:
		WSABUF* get(void) { return &_wsabuf; }

	protected:
		WSABUF _wsabuf;

	public:
		WSABUFContext(void);
	};

	class ReceiveContext : public WSABUFContext
	{
	public:
		ReceiveContext(const NKCore::ReadStream& stream);
	};

	class SendContext : public WSABUFContext
	{
	public:
		SendContext(const NKCore::WriteStream& stream);
	};
}

#endif // __EVENTCONTEXT_HEADER__