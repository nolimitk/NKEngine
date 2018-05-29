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

	protected:
		virtual bool onProcess(EventContext& event_context, uint32_t transferred) override;
		virtual bool onProcessFailed(EventContext& event_context, uint32_t transferred) override;
		
		// @TODO 접속한 socket을 위한 class의 다형성을 지원 해야 하는가?
		// 이렇게 되면 접속한 socket을 Connection으로 생성할 수 없다.
		// Template을 받는다면?
		virtual std::shared_ptr<AsyncSocket> socketAllocator(void);

		// @TODO ServerNetworkEvent interface를 통해 처리하자...
		// 아니면 여기서는 virtual function을 통한 이벤트를 제공하고... 자식 class Server를 구현해서 거기서 ServerNetworkEvent interface를 사용하는것인데...
		// 상속관계를 두고 virtual function으로 처리를 하면 어떤 단점이 있을까?
		virtual void onAccept(std::shared_ptr<AsyncSocket>& async_socket);

	protected:
		bool accept(void);

	protected:
		const static int BACKLOG_DEFAULT = 1024;

	protected:
		const HANDLE _completion_port;
		SOCKET _socket;
		AcceptContext _accept_context;

	public:
		AsyncServerSocket(const HANDLE completion_port);
		virtual ~AsyncServerSocket(void);

		// it calls shared_from_this() function internally, so it must not be created by new operator
	private:
		void* operator new(size_t size) {}
	};
}

#endif // __ASYNCSERVERSOCKET_HEADER__