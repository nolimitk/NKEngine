#pragma once
#ifndef __ASYNCSERVERSOCKET_HEADER__
#define __ASYNCSERVERSOCKET_HEADER__
// @nolimitk
// 13.08.27
// async server socket

#include <WinSock2.h>
#include "Event.h"
#include "EventContext.h"

namespace NKNetwork
{
	class AsyncSocket;

	class AsyncServerSocket : public EventObject
	{
	public:
		bool open(USHORT port);
		bool close(void);

	public:
		virtual bool onProcess(EventContext& event_context, uint32_t transferred) override;
		virtual bool onProcessFailed(EventContext& event_context, uint32_t transferred) override;
		virtual std::shared_ptr<AsyncSocket> socketAllocator(void);
		virtual void onAccept(std::shared_ptr<AsyncSocket>& async_socket);

	protected:
		bool accept(HANDLE hCompletionPort);

	protected:
		const static int BACKLOG_DEFAULT = 1024;

	protected:
		const HANDLE _completion_port;
		SOCKET _socket;
		AcceptContext _accept_context;

	public:
		AsyncServerSocket(const HANDLE completion_port);
		virtual ~AsyncServerSocket(void);
	};
}

#endif // __ASYNCSERVERSOCKET_HEADER__