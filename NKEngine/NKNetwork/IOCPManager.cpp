#include "IOCPManager.h"
#include "../NKEngineLog.h"
#include "WorkerThread.h"
#include "EventContext.h"

using namespace NKNetwork;
using namespace std;

LPFN_CONNECTEX IOCPManager::CONNECTEXFUNC = nullptr;
const std::chrono::milliseconds IOCPManager::DEFAULT_GAP_WORKERTHREAD_TIMEOUT(500);

IOCPManager::IOCPManager(void)
	:_completion_port(nullptr)
{
}


IOCPManager::~IOCPManager(void)
{
}

bool IOCPManager::create(void)
{
	int err = _win_sock_lib.open();

	if (err != 0)
	{
		NKENGINELOG_SOCKETERROR_ASSERT(GetLastError(), L"failed to open windows socket library");
		return false;
	}

	_completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, (ULONG_PTR)0, 0);
	if (_completion_port == nullptr)
	{
		NKENGINELOG_SOCKETERROR_ASSERT(GetLastError(), L"failed to create IO completion port");
		return false;
	}

	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	
	_worker_thread_vector.reserve(system_info.dwNumberOfProcessors);
	for (DWORD i = 0; i < system_info.dwNumberOfProcessors; i++)
	{
		unique_ptr<WorkerThread> worker_thread = make_unique<WorkerThread>(_completion_port);
		if (worker_thread->create() == false)
		{
			NKENGINELOG_ERROR_ASSERT(L"failed to create worker thread, %u/%u, error code %u", i, system_info.dwNumberOfProcessors, GetLastError());
			return false;
		}
		_worker_thread_vector.push_back(std::move(worker_thread));
	}

	SOCKET socket = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED );
	if (socket == INVALID_SOCKET)
	{
		NKENGINELOG_SOCKETERROR_ASSERT( WSAGetLastError(), L"failed to create socket for ConnectEx function" );
		return false;
	}

	GUID guidConnectEx = WSAID_CONNECTEX;
	DWORD bytes;
	if (WSAIoctl( socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx),
		&CONNECTEXFUNC, sizeof(CONNECTEXFUNC), &bytes, NULL, NULL ) == SOCKET_ERROR)
	{
		closesocket(socket);

		NKENGINELOG_SOCKETERROR_ASSERT( WSAGetLastError(), L"failed to set ConnectEx function" );
		return false;
	}

	closesocket( socket );

	NKENGINELOG_INFO( L"success to create IO completion port,%p", _completion_port);

	return true;
}

void IOCPManager::close(void)
{
	// @TODO before winsock close, all server sockets and sockets must be closed.
	_win_sock_lib.close();

	do
	{
		// post stop until all worker threads are joined.
		for (auto&& iter : _worker_thread_vector)
		{
			postStop();
		}

	} while ( allWorkerThreadsJoinable() == false );
	
	_worker_thread_vector.clear();
	CloseHandle(_completion_port);

	Singleton<IOCPManager>::destroy();

	return;
}

bool IOCPManager::postEvent(const std::shared_ptr<EventObject>& event_object, int64_t param)
{
	if (_completion_port == nullptr)
	{
		NKENGINELOG_ERROR_ASSERT(L"iocp manager is not created");
		return false;
	}

	if (event_object == nullptr)
	{
		NKENGINELOG_ERROR_ASSERT(L"event object is null");
		return false;
	}

	// @TODO refactoring using shared_ptr, do not use native pointer
	SchedulerContext *pContext = new SchedulerContext;
	
	memset( pContext, 0, sizeof(SchedulerContext) );
	pContext->_type = EVENTCONTEXTTYPE::SCHEDULER;
	pContext->_param = param;
	pContext->_event_object = event_object;
		
	if( PostQueuedCompletionStatus(_completion_port, 0, (ULONG_PTR)COMPLETION_KEY::PROCESS_EVENT, pContext ) == 0 )
	{
		NKENGINELOG_SOCKETERROR_ASSERT( GetLastError(), L"failed to post event" );

		SAFE_DELETE(pContext);
		return false;
	}
	return true;
}

SOCKET IOCPManager::openSocket(void)
{
	if (_completion_port == nullptr)
	{
		NKENGINELOG_ERROR_ASSERT(L"iocp manager is not created");
		return INVALID_SOCKET;
	}

	SOCKET socket;
	socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (socket == INVALID_SOCKET)
	{
		NKENGINELOG_SOCKETERROR_ASSERT(WSAGetLastError(), L"failed to open a socket");
		return INVALID_SOCKET;
	}

	if (CreateIoCompletionPort((HANDLE)socket, _completion_port, (ULONG_PTR)COMPLETION_KEY::PROCESS_EVENT, 0) == nullptr)
	{
		NKENGINELOG_SOCKETERROR(GetLastError(), L"failed to associate completion port with ,socket %I64u", socket);

		if (closesocket(socket) == SOCKET_ERROR)
		{
			NKENGINELOG_SOCKETERROR(WSAGetLastError(), L"failed to close,socket %I64u", socket);
		}
		return INVALID_SOCKET;
	}

	NKENGINELOG_INFO(L"success to open socket,socket %I64u", socket);

	return socket;
}

bool NKNetwork::IOCPManager::postStop(void)
{
	if (PostQueuedCompletionStatus(_completion_port, 0, (ULONG_PTR)COMPLETION_KEY::END_EVENT, nullptr) == 0)
	{
		NKENGINELOG_SOCKETERROR_ASSERT(GetLastError(), L"failed to post event");
		return false;
	}
	return true;
}

bool NKNetwork::IOCPManager::allWorkerThreadsJoinable(void)
{
	for (auto&& iter : _worker_thread_vector)
	{
		if (iter->join(DEFAULT_GAP_WORKERTHREAD_TIMEOUT) == false)
		{
			return false;
		}
	}
	return true;
}