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
	
	for (DWORD i = 0; i < system_info.dwNumberOfProcessors; i++)
	{
		unique_ptr<WorkerThread> worker_thread = make_unique<WorkerThread>(*this);
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
		// post stop when all worker threads are joined.
		for (auto iter = _worker_thread_vector.begin(); iter != _worker_thread_vector.end(); iter++)
		{
			postStop();
		}

	} while ( allWorkerThreadsJoinable() == false );
	
	_worker_thread_vector.clear();
	CloseHandle(_completion_port);

	Singleton<IOCPManager>::destroy();

	return;
}

bool IOCPManager::postEvent(std::shared_ptr<EventObject>& event_object, int64_t param)
{
	if (event_object == nullptr) return false;

	// @TODO refactoring using shared_ptr, do not use native pointer
	SchedulerContext *pContext = new SchedulerContext;
	
	memset( pContext, 0, sizeof(SchedulerContext) );
	pContext->_type = EVENTCONTEXTTYPE::SCHEDULER;
	pContext->_param = param;
	pContext->_event_object = event_object;
		
	if( PostQueuedCompletionStatus(_completion_port, 0, (ULONG_PTR)COMPLETION_KEY::PROCESS_EVENT, pContext ) == 0 )
	{
		NKENGINELOG_SOCKETERROR_ASSERT( GetLastError(), L"failed to post event" );

		delete pContext;
		pContext = nullptr;
		return false;
	}
	return true;
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
	for (auto iter = _worker_thread_vector.begin(); iter != _worker_thread_vector.end(); iter++)
	{
		if (iter->get()->join(DEFAULT_GAP_WORKERTHREAD_TIMEOUT) == false)
		{
			return false;
		}
	}
	return true;
}