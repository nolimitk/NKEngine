#pragma once
#ifndef __ASYNCSOCKET_HEADER__
#define __ASYNCSOCKET_HEADER__
// @nolimitk
// 13.08.27
// async socket

#include "../NKCore.h"
#include "Event.h"

namespace NKNetwork
{
	class Packet;
	class WriteStream;

	class AsyncSocket : public EventObject
	{
	public:
		const static int BUFFER_LENGTH_8K = 8000; // tbb small object size

	public:
		bool open(HANDLE hCompletionPort);
		bool connect(const NKString& address, USHORT port);
		bool close(void);
		bool recv(void);
		/*bool send(byte *pSendData, uint16_t len);*/
		bool send(const NKCore::WriteStream& stream);
		bool disconnect(void);

	public:
		inline SOCKET getHandle(void) { return _socket; }
		inline void setAddress(const NKWString& address) { _address = address; }
		inline NKWString getAddress(void) { return _address; }

	public:
		virtual bool onProcess(EventContext& event_context, uint32_t transferred) override;
		virtual bool onProcessFailed(EventContext& event_context, uint32_t transferred) override;
		virtual bool onClosed(void);
		virtual bool onConnected(void);
		virtual bool onConnectFailed(void);
		virtual bool onReceived(const Packet& packet);
		virtual bool onSent(void);

	protected:
		SOCKET _socket;
		NKWString _address;
		NKCore::ReadStream _recv_stream;

	public:
		AsyncSocket(void);
		virtual ~AsyncSocket(void);
	};
}

#endif // __ASYNCSOCKET_HEADER__
