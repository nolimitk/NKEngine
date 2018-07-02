#pragma once
#ifndef __ASYNCSERVERSOCKET_HEADER__
#define __ASYNCSERVERSOCKET_HEADER__
// @nolimitk
// 13.08.27
// async server socket

#include <WinSock2.h>
#include "Event.h"

namespace NKNetwork
{
	class ServerCallback;
	class ClientCallback;

	class AsyncServerSocket : public EventObject
	{
	public:
		bool open(USHORT port);
		bool close(void);
				
	protected:
		virtual bool onProcess(EventContext& event_context, uint32_t transferred) override;
		virtual bool onProcessFailed(EventContext& event_context, uint32_t transferred) override;

	protected:
		bool accept(void);

	protected:
		static const int BACKLOG_DEFAULT = 1024;

	protected:
		SOCKET _socket;
		std::shared_ptr<ServerCallback> _server_callback;
		std::shared_ptr<ClientCallback> _client_callback;

	public:
		AsyncServerSocket(const std::shared_ptr<ServerCallback>& server_callback, const std::shared_ptr<ClientCallback>& client_callback);
		virtual ~AsyncServerSocket(void);

		// it calls shared_from_this() function internally, so it must not be created by new operator
	private:
		void* operator new(size_t size) {}
	};
}

#endif // __ASYNCSERVERSOCKET_HEADER__