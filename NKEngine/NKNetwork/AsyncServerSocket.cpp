#include "AsyncServerSocket.h"
#include <MSWSock.h>
#include <Ws2tcpip.h>
#include "../NKEngineLog.h"
#include "AsyncSocket.h"
#include "IOCPManager.h"
#include "EventContext.h"
#include "NetworkCallbacks.h"

using namespace NKNetwork;
using namespace std;

AsyncServerSocket::AsyncServerSocket(const std::shared_ptr<ServerCallback>& server_callback, const std::shared_ptr<ClientCallback>& client_callback)
	:_socket(INVALID_SOCKET)
	, _server_callback(server_callback)
	, _client_callback(client_callback)
{
}

AsyncServerSocket::~AsyncServerSocket(void)
{
}

bool AsyncServerSocket::open(USHORT port)
{
	_socket = IOCPManager::getInstance()->openSocket();
	if( _socket == INVALID_SOCKET )
	{
		return false;
	}

	SOCKADDR_IN addr;
	memset( &addr, 0, sizeof(addr) );
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	int err = ::bind( _socket, (SOCKADDR *)&addr, sizeof(addr) );
	if( err == SOCKET_ERROR )
	{
		NKENGINELOG_SOCKETERROR_ASSERT( WSAGetLastError(), L"failed to bind socket,socket %I64u", _socket );

		close();
		return false;
	}

	if( listen( _socket, BACKLOG_DEFAULT ) == SOCKET_ERROR )
	{
		NKENGINELOG_SOCKETERROR_ASSERT( WSAGetLastError(), L"failed to listen socket,socket %I64u", _socket );

		close();
		return false;
	}

	if( accept() == false )
	{
		return false;
	}

	NKENGINELOG_INFO(L"success to open server socket,socket %I64u,port %d", _socket, port);

	return true;
}

bool AsyncServerSocket::close(void)
{
	if (_socket == INVALID_SOCKET)
	{
		NKENGINELOG_ERROR(L"socket handle is invalid");
		return false;
	}

	NKENGINELOG_INFO(L"close an async server socket,socket %I64u", _socket);

	if( closesocket(_socket) == SOCKET_ERROR )
	{
		NKENGINELOG_SOCKETERROR( WSAGetLastError(), L"failed to close,socket %I64u", _socket );
		return false;
	}

	// @TODO closesocket을 호출하면 invalid socket으로 만들기 전에 iocp event가 실행된다.
	_socket = INVALID_SOCKET;

	return true;
}

bool AsyncServerSocket::accept(void)
{
	SOCKET socket = IOCPManager::getInstance()->openSocket();
	if (socket == INVALID_SOCKET)
	{
		return false;
	}

	shared_ptr<AsyncSocket> accept_socket = make_shared<AsyncSocket>(socket, _client_callback);
	if(accept_socket == nullptr )
	{
		NKENGINELOG_ERROR( L"failed to allocate new accept ,socket %I64u", socket);
		return false;
	}
			
	DWORD dwBytes = 0;
	AcceptContext* pAcceptContext = new AcceptContext();
	pAcceptContext->_type = EVENTCONTEXTTYPE::ACCEPT;
	pAcceptContext->_event_object = shared_from_this();
	pAcceptContext->_accept_socket = accept_socket;
	
	// @TODO 빠른 접속을 위해 backlog 만큼의 accept를 실행한다.
	if( AcceptEx( _socket, socket, pAcceptContext->_outputBuffer, 0, sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, &dwBytes, pAcceptContext) == FALSE )
	{
		if( WSAGetLastError() != ERROR_IO_PENDING )
		{
			NKENGINELOG_SOCKETERROR(WSAGetLastError(), L"failed to accept,socket %I64u,accept socket %I64u", _socket, socket);
			
			accept_socket->close();
			return false;
		}
	}

	return true;
}

bool AsyncServerSocket::onProcess(EventContext& event_context, uint32_t transferred)
{
	_ASSERT(event_context._type == EVENTCONTEXTTYPE::ACCEPT );
	
	AcceptContext& accept_context = static_cast<AcceptContext&>(event_context);
		
	setsockopt(accept_context._accept_socket->getHandle(), SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (const char *)&_socket, sizeof(_socket) );

	// peer address를 얻어와서 저장한다.
	SOCKADDR_IN *lpLocalSockaddr = nullptr, *lpRemoteSockaddr = nullptr;
	int LocalSockaddrLen = 0, RemoteSockaddrLen = 0;
	GetAcceptExSockaddrs(accept_context._outputBuffer, 0, sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (sockaddr **)&lpLocalSockaddr, &LocalSockaddrLen, (sockaddr **)&lpRemoteSockaddr, &RemoteSockaddrLen );

	char address[20];
	inet_ntop(AF_INET, &lpRemoteSockaddr->sin_addr.s_addr, address, sizeof(address));
	NKWString waddress = MultibyteToUnicode(address);
	NKENGINELOG_INFO( L"accepted,socket %I64u,accept socket %I64u,peer address : %s", _socket, accept_context._accept_socket->getHandle(), waddress.c_str() );

	accept_context._accept_socket->setAddress(waddress);
	//

	_server_callback->onAccepted(accept_context._accept_socket);

	// peer에서 연결을 바로 종료하면 iocp 등록이 실패할 수 있다.
	if(accept_context._accept_socket->recv() == false )
	{
		return false;
	}
	
	// ready to accept next socket
	if( accept() == false )
	{
		return false;
	}
	//

	return true;
}

bool AsyncServerSocket::onProcessFailed(EventContext& event_context, uint32_t transferred)
{
	_ASSERT(event_context._type == EVENTCONTEXTTYPE::ACCEPT );

	DWORD lastError = GetLastError();
	switch( lastError )
	{
		// @nolimitk IOCP가 종료 할때 accept를 위해 생성 했던 socket이 error를 발생시킨다. 따라서 Server를 종료할 때는 정상이다.
		// @TODO iocp와 associated된 accept socket을 종료하는 다른 방법이 있나?
	case ERROR_OPERATION_ABORTED:
		NKENGINELOG_INFO(L"server socket, terminated,socket %I64u", _socket);
		_server_callback->onClosed();
		break;
	default:
		// @nolimitk server socket은 직접 close를 호출하지 않으면 종료될 일이 없다... 이런 경우를 찾아야 한다.
		NKENGINELOG_SOCKETERROR_ASSERT(lastError, L"server socket, event failed,socket %I64u,transferred %u", _socket, transferred);
		break;
	}

	// @nolimitk close를 호출하지 않고 iocp를 종료하는 등 socket이 close되어 있지 않을 수 있다.
	closesocket(_socket);

	return false;
}