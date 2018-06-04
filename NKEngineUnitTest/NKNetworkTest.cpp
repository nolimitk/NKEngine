#pragma once

#ifndef __NKNETWORKTEST_HEADER__
#define __NKNETWORKTEST_HEADER__
// @nolimitk
// 16.04.25
// network test

#include "NKEngine.h"
#include "NKUnittest.h"
#include "NKUnittestLog.h"

using namespace std;
using namespace NKNetwork;

#define WAITFOR(owner,func) while (owner->_ ##func == false) { }

NKTEST(WinSockLib_Test)
{
	WinSockLib winsocklib;
	_ASSERT(winsocklib.open() == 0);
	_ASSERT(winsocklib.close() == 0);

	return true;
}

NKTEST(IOCPManager_Test)
{
	_ASSERT(IOCPManager::getInstance()->create() == true);

	class MockEvent : public EventObject
	{
		bool onProcess(EventContext& event_context, uint32_t transferred) override
		{
			_onProcess = true;
			return true;
		}

	public:
		bool _onProcess;

		MockEvent(void) :_onProcess(false) {}
	};
	
	shared_ptr<MockEvent> test_event = make_shared<MockEvent>();
	_ASSERT(IOCPManager::getInstance()->postEvent(dynamic_pointer_cast<EventObject>(test_event), 0) == true);

	WAITFOR(test_event, onProcess);

	// for worker thread update event test, it remains log.
	//this_thread::sleep_for(500ms);

	IOCPManager::getInstance()->close();

	return true;
}

void writeStream(RecvStream& stream, const NKString& str)
{
	ReceiveContext *pRecvContext = new ReceiveContext(stream);
	byte* pRawBuffer = stream.getRemainBuffer();
	WSABUF* pWSABuf = pRecvContext->get();
	_ASSERT(pWSABuf->buf != nullptr);
	PROTOCOLHEAD::command_type command = 0;
	PROTOCOLHEAD::size_type size = (PROTOCOLHEAD::size_type)(sizeof(PROTOCOLHEAD) + str.length());
	memcpy(pWSABuf->buf, &size, sizeof(size));
	memcpy(pWSABuf->buf + sizeof(size), &command, sizeof(command));
	memcpy(pWSABuf->buf + sizeof(PROTOCOLHEAD), str.c_str(), str.length());

	for (int i = 0; i < str.length(); ++i)
	{
		_ASSERT(pRawBuffer[i + sizeof(PROTOCOLHEAD)] == str[i]);
	}

	pWSABuf->len = 7;
	SAFE_DELETE(pRecvContext);
}

void readStream(RecvStream& stream)
{
	while (stream.getLength() >= sizeof(PROTOCOLHEAD::size_type))
	{
		PROTOCOLHEAD::size_type length = stream.peek<PROTOCOLHEAD::size_type>();
		if (length <= stream.getLength())
		{
			Packet packet;
			_ASSERT(packet.read(stream) == true);
		}
		else
		{
			break;
		}
	}
	_ASSERT(stream.moveRead());
}

NKTEST(RecvStream_Test)
{
	const static int BUUFER_SIZE = 8000;

	RecvStream _recv_stream(make_shared<NKCore::Buffer>(BUUFER_SIZE));
	_ASSERT(_recv_stream.getRemainSize() == BUUFER_SIZE);
	byte* pRawBuffer = _recv_stream.getRemainBuffer();
	_ASSERT(pRawBuffer != nullptr);

	writeStream(_recv_stream, "abc");

	{
		_ASSERT(_recv_stream.update(7) == true);
		_ASSERT(_recv_stream.getLength() >= sizeof(PROTOCOLHEAD::size_type));
		readStream(_recv_stream);
	}

	writeStream(_recv_stream, "defghij");

	{
		_ASSERT(_recv_stream.update(1) == true);
		readStream(_recv_stream);
		_ASSERT(_recv_stream.update(1) == true);
		readStream(_recv_stream);
		_ASSERT(_recv_stream.update(4) == true);
		readStream(_recv_stream);
		_ASSERT(_recv_stream.update(5) == true);
		readStream(_recv_stream);
	}

	writeStream(_recv_stream, "abc");
	_ASSERT(_recv_stream.update(7) == true);
	writeStream(_recv_stream, "defghij");

	{
		_ASSERT(_recv_stream.update(11) == true);
		readStream(_recv_stream);
	}

	return true;
}

NKTEST(AsyncServerSocket_Test)
{
	setlocale(LC_ALL, "korean");

	_ASSERT(IOCPManager::getInstance()->create() == true);
	
	class MockServerCallback : public ServerCallback
	{
		void onAccepted(shared_ptr<AsyncSocket> socket) override
		{
		}
		void onClosed(void) override
		{
			_onClosed = true;
		}

	public:
		bool _onClosed;
		MockServerCallback(void) :_onClosed(false) {}
	};
	
	// Server
	const static USHORT TEST_PORT = 10000;
	shared_ptr<MockServerCallback> server_callback = make_shared<MockServerCallback>();
	shared_ptr<AsyncServerSocket> server_socket = make_shared<AsyncServerSocket>(server_callback, make_shared<DefaultClientCallback>());
	_ASSERT(server_socket);
	_ASSERT(server_socket->open(TEST_PORT) == true);
	
	_ASSERT(server_socket->close() == true);
	
	WAITFOR(server_callback, onClosed);

	IOCPManager::getInstance()->close();

	return true;
}

class MockCallback : public ClientCallback
{
public:
	void onConnected(std::shared_ptr<AsyncSocket> socket) override
	{
		shared_ptr<NKCore::Buffer> buffer = make_shared<NKCore::Buffer>(1024);
		SendStream send_stream(buffer);
		_ASSERT(send_stream.write(5) == true);
		_ASSERT(send_stream.write('A') == true);
		_ASSERT(socket->send(send_stream) == true);

		_onConnected = true;
	}
	void onReceived(std::shared_ptr<AsyncSocket> socket, const Packet& packet) override{ _onReceived = true; }
	void onSent(std::shared_ptr<AsyncSocket> socket) override { _onSent = true; }
	void onConnectFailed(std::shared_ptr<AsyncSocket> socket) override { _ASSERT(false); }
	void onClosed(std::shared_ptr<AsyncSocket> socket) override { _onClosed = true;  }

public:
	bool _onConnected;
	bool _onSent;
	bool _onReceived;
	bool _onClosed;

	MockCallback(void) :_onConnected(false), _onSent(false), _onReceived(false), _onClosed(false) {}
};

NKTEST(AsyncSocket_Test)
{
	_ASSERT(IOCPManager::getInstance()->create() == true);

	SOCKET socket = IOCPManager::getInstance()->openSocket();
	_ASSERT(socket != INVALID_SOCKET);

	// Connection
	shared_ptr<MockCallback> client_callback = make_shared<MockCallback>();
	shared_ptr<AsyncSocket> client_socket = make_shared<AsyncSocket>(socket, client_callback);
	_ASSERT(client_socket);
	_ASSERT(client_socket->connect("www.google.com", 80) == true);

	WAITFOR(client_callback, onConnected);
	WAITFOR(client_callback, onSent);
	
	Sleep(100);

	// there will be 2 errors
	// receiving error and force close
	
	IOCPManager::getInstance()->close();

	return true;
}

NKTEST(AsyncServerSocket_AsyncSocket_Test)
{
	_ASSERT(IOCPManager::getInstance()->create() == true);

	class MockClientCallback : public ClientCallback
	{
	public:
		void onConnected(shared_ptr<AsyncSocket> socket) override { _ASSERT(false); }
		void onReceived(shared_ptr<AsyncSocket> socket, const Packet& packet) override
		{
			shared_ptr<NKCore::Buffer> buffer = make_shared<NKCore::Buffer>(1024);
			SendStream send_stream(buffer);
			_ASSERT(send_stream.write(5) == true);
			_ASSERT(send_stream.write('B') == true);
			_ASSERT(socket->send(send_stream) == true);

			_onReceived = true;
		}
		void onSent(shared_ptr<AsyncSocket> socket) override { _onSent = true; }
		void onConnectFailed(shared_ptr<AsyncSocket> socket) override { _ASSERT(false); }
		void onClosed(shared_ptr<AsyncSocket> socket) override {}

	public:
		bool _onConnected;
		bool _onSent;
		bool _onReceived;

		MockClientCallback(void) :_onConnected(false), _onSent(false), _onReceived(false) {}
	};

	class MockServerCallback : public ServerCallback
	{
		void onAccepted(shared_ptr<AsyncSocket> socket) override
		{
			_ASSERT(socket->getAddress().compare(L"127.0.0.1") == 0);
		}
		void onClosed(void) override
		{
			_onClosed = true;
		}

	public:
		bool _onClosed;
		MockServerCallback(void) :_onClosed(false) {}
	};

	// Server
	const static USHORT TEST_PORT = 10000;
	shared_ptr<MockServerCallback> server_callback = make_shared<MockServerCallback>();
	shared_ptr<MockClientCallback> client_callback = make_shared<MockClientCallback>();
	shared_ptr<AsyncServerSocket> server_socket = make_shared<AsyncServerSocket>(server_callback, client_callback);
	_ASSERT(server_socket);
	_ASSERT(server_socket->open(TEST_PORT) == true);

	// Connection
	{
		SOCKET socket = IOCPManager::getInstance()->openSocket();
		_ASSERT(socket != INVALID_SOCKET);

		shared_ptr<MockCallback> client_callback = make_shared<MockCallback>();
		shared_ptr<AsyncSocket> client_socket = make_shared<AsyncSocket>(socket, client_callback);
		_ASSERT(client_socket);
		_ASSERT(client_socket->connect("localhost", TEST_PORT) == true);

		WAITFOR(client_callback, onConnected);
		WAITFOR(client_callback, onSent);
		WAITFOR(client_callback, onReceived);

		_ASSERT(client_socket->disconnect() == true);

		WAITFOR(client_callback, onClosed);

		Sleep(100);
	}
	
	_ASSERT(server_socket->close() == true);

	Sleep(500);

	IOCPManager::getInstance()->close();

	return true;
}

//NKTEST(Connection_Test)
//{
//	_ASSERT(IOCPManager::getInstance()->create() == true);
//
//	HANDLE completionPort = IOCPManager::getInstance()->getCompletionPort();
//	_ASSERT(completionPort != INVALID_HANDLE_VALUE);
//
//	const static int TEST_PORT = 10000;
//		
//	shared_ptr<AsyncServerSocket> spServerSocket = make_shared<AsyncServerSocket>(completionPort);
//	_ASSERT(spServerSocket);
//	
//	_ASSERT(spServerSocket->open(TEST_PORT) == true);
//
//	shared_ptr<AsyncSocket> spClientSocket = make_shared<AsyncSocket>();
//	_ASSERT(spClientSocket->open(completionPort) == true);
//	_ASSERT(spClientSocket->connect("localhost", TEST_PORT) == true);
//
//	Sleep(100);
//
//	_ASSERT(spClientSocket->disconnect() == true);
//
//	Sleep(100);
//
//	_ASSERT(spClientSocket->connect("localhost", TEST_PORT) == true);
//
//	Sleep(100);
//
//	_ASSERT(spClientSocket->close() == true);
//
//	// @TODO 비동기는 unittest를 어떻게 해야 하는가?
//
//	// @TODO server socket을 닫을때 iocp와의 연결을 종료처리 해야한다.
//	_ASSERT(spServerSocket->close() == true);
//
//	IOCPManager::getInstance()->close();
//	return true;
//}

#endif // __NKNETWORKTEST_HEADER__