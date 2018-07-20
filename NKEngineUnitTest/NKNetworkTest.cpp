// @nolimitk
// 16.04.25
// network test

#include "NKEngine.h"
#include "NKUnittest.h"
#include "NKUnittestLog.h"

using namespace std;
using namespace NKNetwork;

NKTEST(WinSockLib_Test)
{
	WinSockLib winsocklib;
	_ASSERT(winsocklib.open() == 0);
	_ASSERT(winsocklib.close() == 0);

	return true;
}

NKTEST(IOCPManager_Test)
{
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
	static const int BUUFER_SIZE = 8000;

	RecvStream recv_stream(make_shared<NKCore::Buffer>(BUUFER_SIZE));
	_ASSERT(recv_stream.getRemainSize() == BUUFER_SIZE);
	byte* pRawBuffer = recv_stream.getRemainBuffer();
	_ASSERT(pRawBuffer != nullptr);

	writeStream(recv_stream, "abc");

	{
		_ASSERT(recv_stream.update(7) == true);
		_ASSERT(recv_stream.getLength() >= sizeof(PROTOCOLHEAD::size_type));
		readStream(recv_stream);
	}

	writeStream(recv_stream, "defghij");

	{
		_ASSERT(recv_stream.update(1) == true);
		readStream(recv_stream);
		_ASSERT(recv_stream.update(1) == true);
		readStream(recv_stream);
		_ASSERT(recv_stream.update(4) == true);
		readStream(recv_stream);
		_ASSERT(recv_stream.update(5) == true);
		readStream(recv_stream);
	}

	writeStream(recv_stream, "abc");
	_ASSERT(recv_stream.update(7) == true);
	writeStream(recv_stream, "defghij");

	{
		_ASSERT(recv_stream.update(11) == true);
		readStream(recv_stream);
	}

	return true;
}

NKTEST(SendStream_Test)
{
	static const int BUUFER_SIZE = 8000;

	SendStream send_stream(make_shared<NKCore::Buffer>(BUUFER_SIZE));
	byte* pRawBuffer = send_stream.get();
	_ASSERT(pRawBuffer != nullptr);
	
	int length = 0;
	_ASSERT(send_stream.getLength() == 0);
	_ASSERT(send_stream.write(123) == true);
	length += sizeof(int);
	_ASSERT(send_stream.getLength() == length);
	_ASSERT(send_stream.write('A') == true);
	length += sizeof(char);
	_ASSERT(send_stream.getLength() == length);
	_ASSERT(send_stream.write((short)123) == true);
	length += sizeof(short);
	_ASSERT(send_stream.getLength() == length);
	_ASSERT(send_stream.write(0.0f) == true);
	length += sizeof(float);
	_ASSERT(send_stream.getLength() == length);
	_ASSERT(send_stream.write((double)0.0f) == true);
	length += sizeof(double);
	_ASSERT(send_stream.getLength() == length);
	_ASSERT(send_stream.write(L'B') == true);
	length += sizeof(wchar_t);
	_ASSERT(send_stream.getLength() == length);

	struct Data
	{
		int i;
		short s;
		float f;
		double d;
		char c;
		wchar_t w;
	};

	Data d;
	_ASSERT(send_stream.write((byte *)&d,sizeof(d)) == true);
	length += sizeof(d);
	_ASSERT(send_stream.getLength() == length);
	
	return true;
}

NKTEST(Packet_Test)
{
	return true;
}

NKTEST(AsyncServerSocket_Test)
{
	setlocale(LC_ALL, "korean");
	
	class MockServerCallback : public ServerCallback
	{
		void onAccepted(ConnectionSP& socket) override
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
	static const USHORT TEST_PORT = 10000;
	shared_ptr<MockServerCallback> server_callback = make_shared<MockServerCallback>();
	shared_ptr<AsyncServerSocket> server_socket = make_shared<AsyncServerSocket>(server_callback, make_shared<DefaultClientCallback>());
	_ASSERT(server_socket);
	_ASSERT(server_socket->open(TEST_PORT) == true);
	
	_ASSERT(server_socket->close() == true);
	
	WAITFOR(server_callback, onClosed);

	return true;
}

class MockCallback : public ClientCallback
{
public:
	void onConnected(const ConnectionSP& socket) override
	{
		shared_ptr<NKCore::Buffer> buffer = make_shared<NKCore::Buffer>(1024);
		SendStream send_stream(buffer);
		_ASSERT(send_stream.write(5) == true);
		_ASSERT(send_stream.write('A') == true);
		_ASSERT(socket->send(send_stream) == true);

		_onConnected = true;
	}
	void onReceived(const ConnectionSP& socket, const Packet& packet) override{ _onReceived = true; }
	void onSent(const ConnectionSP& socket) override { _onSent = true; }
	void onConnectFailed(const ConnectionSP& socket) override { _ASSERT(false); }
	void onClosed(const ConnectionSP& socket) override { _onClosed = true;  }

public:
	bool _onConnected;
	bool _onSent;
	bool _onReceived;
	bool _onClosed;

	MockCallback(void) :_onConnected(false), _onSent(false), _onReceived(false), _onClosed(false) {}
};

NKTEST(AsyncSocket_Test)
{
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

	return true;
}

class MockServerCallback : public ServerCallback
{
	void onAccepted(ConnectionSP& socket) override
	{
		_ASSERT(socket->getAddress().compare(L"127.0.0.1") == 0);
		_onAccepted = true;
	}
	void onClosed(void) override
	{
		_onClosed = true;
	}

public:
	bool _onAccepted;
	bool _onClosed;
	MockServerCallback(void) :_onAccepted(false), _onClosed(false) {}
};

class MockClientCallback : public ClientCallback
{
public:
	void onConnected(const shared_ptr<AsyncSocket>& socket) override { _onConnected = true; }
	void onReceived(const shared_ptr<AsyncSocket>& socket, const Packet& packet) override
	{
		shared_ptr<NKCore::Buffer> buffer = make_shared<NKCore::Buffer>(1024);
		SendStream send_stream(buffer);
		_ASSERT(send_stream.write(5) == true);
		_ASSERT(send_stream.write('B') == true);
		_ASSERT(socket->send(send_stream) == true);

		_onReceived = true;
	}
	void onSent(const shared_ptr<AsyncSocket>& socket) override { _onSent = true; }
	void onConnectFailed(const shared_ptr<AsyncSocket>& socket) override { _ASSERT(false); }
	void onClosed(const shared_ptr<AsyncSocket>& socket) override {}

public:
	bool _onConnected;
	bool _onSent;
	bool _onReceived;

	MockClientCallback(void) :_onConnected(false), _onSent(false), _onReceived(false) {}
};

NKTEST(AsyncServerSocket_AsyncSocket_Test)
{	
	// Server
	static const uint16_t TEST_PORT = 10000;
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

		WAITFOR(server_callback, onAccepted);

		_ASSERT(client_socket->disconnect() == true);
		
		WAITFOR(client_callback, onClosed);

		Sleep(100);
	}
	
	_ASSERT(server_socket->close() == true);

	Sleep(500);

	return true;
}

NKTEST(Service_Test)
{
	static const uint16_t TEST_PORT = 10000;
	NKNetwork::Service service(TEST_PORT);

	_ASSERT(service.start() == true);

	shared_ptr<MockServerCallback> server_callback = make_shared<MockServerCallback>();
	_ASSERT(service.registerServerCallback(server_callback) == true);

	shared_ptr<AsyncSocket> client_socket = createSocket<MockClientCallback>();
	_ASSERT(client_socket);

	_ASSERT(client_socket->connect("localhost", TEST_PORT) == true);

	WAITFOR(server_callback, onAccepted);

	_ASSERT(client_socket->disconnect() == true);
	
	service.close();

	return true;
}