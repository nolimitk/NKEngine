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
		
		// @TODO ������ socket�� ���� class�� �������� ���� �ؾ� �ϴ°�?
		// �̷��� �Ǹ� ������ socket�� Connection���� ������ �� ����.
		// Template�� �޴´ٸ�?
		virtual std::shared_ptr<AsyncSocket> socketAllocator(void);

		// @TODO ServerNetworkEvent interface�� ���� ó������...
		// �ƴϸ� ���⼭�� virtual function�� ���� �̺�Ʈ�� �����ϰ�... �ڽ� class Server�� �����ؼ� �ű⼭ ServerNetworkEvent interface�� ����ϴ°��ε�...
		// ��Ӱ��踦 �ΰ� virtual function���� ó���� �ϸ� � ������ ������?
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