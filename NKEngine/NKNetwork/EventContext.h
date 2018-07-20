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
	class RecvStream;

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
		// @nolimitk IOCP에 pointer를 넘기면 virtual table이 nullptr이 된다.
		//virtual ~EventContext(void) {}
	};

	class AcceptContext : public EventContext
	{
	public:
		static const int BUFFER_SIZE = (sizeof(SOCKADDR_IN) + 16) * 2;

	public:
		char _outputBuffer[BUFFER_SIZE];
		// @nolimitk EventContext가 virtual destructor를 사용하지 못하기 때문에 shared_ptr을 쓰지 못하고 수동으로 관리해야 한다.
		AsyncSocket* _accept_socket;

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
		ReceiveContext(const RecvStream& stream);
	};

	class SendContext : public WSABUFContext
	{
	public:
		SendContext(byte* buffer, uint32_t size);
	};
}

#endif // __EVENTCONTEXT_HEADER__