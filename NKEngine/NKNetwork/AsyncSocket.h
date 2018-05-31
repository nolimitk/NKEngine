#pragma once
#ifndef __ASYNCSOCKET_HEADER__
#define __ASYNCSOCKET_HEADER__
// @nolimitk
// 13.08.27
// async socket

#include "../NKCore.h"
#include "Event.h"
#include "RecvStream.h"

namespace NKNetwork
{
	class Packet;
	class SendStream;

	class AsyncSocket : public EventObject
	{
	public:
		bool connect(const NKString& address, USHORT port);
		bool disconnect(void);
		bool send(const SendStream& stream);

	public:
		// @TODO it would be protected. AsyncServerSocket에서 accept를 위한 socket 풀을 만들어놀때 필요하다.
		bool open(const HANDLE completion_port);
		
		// @TODO it would be protected. disconnect 통지가 오면 AsyncServerSocket에서 사용하는 함수 
		bool close(void);

		// @TODO it would be protected. recv event를 IOCP에 등록하기 위해 AsyncServerSocket에서 사용
		bool recv(void);
				
	public:
		// @TODO AsyncSocket과 연결을 위해 사용
		inline SOCKET getHandle(void) const { return _socket; }

	public:
		inline void setAddress(const NKWString& address) { _address = address; }
		inline NKWString getAddress(void) const { return _address; }

	protected:
		virtual bool onProcess(EventContext& event_context, uint32_t transferred) override;
		virtual bool onProcessFailed(EventContext& event_context, uint32_t transferred) override;

	protected:
		virtual bool onClosed(void);
		virtual bool onConnected(void);
		virtual bool onConnectFailed(void);
		virtual bool onReceived(const Packet& packet);
		virtual bool onSent(void);

	protected:
		const static int BUFFER_LENGTH_8K = 8000; // tbb small object size

	protected:
		SOCKET _socket;
		NKWString _address;
		RecvStream _recv_stream;
		
	public:
		AsyncSocket(void);
		virtual ~AsyncSocket(void);

		// it calls shared_from_this() function internally, so it must not be created by new operator
	private:
		void* operator new(size_t size) {}
	};
}

#endif // __ASYNCSOCKET_HEADER__
