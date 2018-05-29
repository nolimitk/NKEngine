#include "AsyncServerSocket.h"
#include <MSWSock.h>
#include <Ws2tcpip.h>
#include "../NKEngineLog.h"
#include "AsyncSocket.h"
#include "IOCPManager.h"

using namespace NKNetwork;
using namespace std;

AsyncServerSocket::AsyncServerSocket(const HANDLE completion_port)
	:_completion_port(completion_port)
	,_socket(INVALID_SOCKET)
{
}

AsyncServerSocket::~AsyncServerSocket(void)
{
}

bool AsyncServerSocket::open(USHORT port)
{
	_socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
	if( _socket == INVALID_SOCKET )
	{
		NKENGINELOG_SOCKETERROR_ASSERT( WSAGetLastError(), L"failed to open socket,socket %I64u", _socket);
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

	// listen 전에 completion port와 연결한다.
	if( CreateIoCompletionPort( (HANDLE)_socket, _completion_port, (ULONG_PTR)IOCPManager::COMPLETION_KEY::PROCESS_EVENT, 0 ) == nullptr)
	{
		NKENGINELOG_SOCKETERROR_ASSERT( GetLastError(), L"failed to bind socket to completion port,socket %I64u", _socket );

		close();
		return false;
	}

	if( listen( _socket, BACKLOG_DEFAULT ) == SOCKET_ERROR )
	{
		NKENGINELOG_SOCKETERROR( WSAGetLastError(), L"failed to listen socket,socket %I64u", _socket );

		close();
		return false;
	}

	if( accept() == false )
	{
		return false;
	}

	NKENGINELOG_INFO( L"success to open server socket,socket %I64u,port %d", _socket, port );

	return true;
}

bool AsyncServerSocket::close(void)
{
	if (_socket == INVALID_SOCKET)
	{
		NKENGINELOG_ERROR(L"socket handle is invalid");
		return false;
	}

	if( closesocket(_socket) == SOCKET_ERROR )
	{
		NKENGINELOG_SOCKETERROR( WSAGetLastError(), L"failed to close,socket %I64u", _socket );
		return false;
	}

	NKENGINELOG_INFO( L"close an async server socket,socket %I64u", _socket );

	_socket = INVALID_SOCKET;

	return true;
}

bool AsyncServerSocket::accept(void)
{
	shared_ptr<AsyncSocket> accept_socket = socketAllocator();
	if(accept_socket == nullptr )
	{
		NKENGINELOG_ERROR( L"failed to allocate new accept socket,socket %I64u", _socket);
		return false;
	}

	if(accept_socket->open(_completion_port) == false )
	{
		return false;
	}
		
	DWORD dwBytes = 0;
	AcceptContext* pAcceptContext = new AcceptContext();
	pAcceptContext->_type = EVENTCONTEXTTYPE::ACCEPT;
	pAcceptContext->_event_object = shared_from_this();
	pAcceptContext->_accept_socket = accept_socket;
	
	// @TODO 빠른 접속을 위해 backlog 만큼의 accept를 실행한다.
	if( AcceptEx( _socket, accept_socket->getHandle(), pAcceptContext->_outputBuffer, 0, sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, &dwBytes, pAcceptContext) == FALSE )
	{
		if( WSAGetLastError() != ERROR_IO_PENDING )
		{
			NKENGINELOG_SOCKETERROR( WSAGetLastError(), L"failed to accept,socket %I64u,accept socket %I64u", _socket, accept_socket->getHandle() );

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

	onAccept(accept_context._accept_socket);

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
	case 995:
		NKENGINELOG_INFO( L"server socket, terminated,socket %I64u", _socket );
		break;
	default:
		NKENGINELOG_SOCKETERROR(lastError, L"server socket, event failed,socket %I64u", _socket );
		break;
	}

	AcceptContext& accept_context = static_cast<AcceptContext&>(event_context);
	shared_ptr<AsyncServerSocket> async_socket = dynamic_pointer_cast<AsyncServerSocket>(accept_context._event_object);
	if (async_socket == nullptr)
	{
		NKENGINELOG_ERROR_ASSERT(L"server socket, casting failed,socket %I64u,transferred %u", _socket, transferred);
		return false;
	}

	async_socket->close();

	NKENGINELOG_ERROR( L"event failed,socket %I64u,transferred %u", _socket, transferred);

	return true;
}

std::shared_ptr<AsyncSocket> AsyncServerSocket::socketAllocator(void)
{
	return make_shared<AsyncSocket>();
}

void AsyncServerSocket::onAccept(std::shared_ptr<AsyncSocket>& async_socket)
{
	return;
}