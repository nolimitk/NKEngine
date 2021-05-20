#pragma once
#ifndef __IOCPMANAGER_HEADER__
#define __IOCPMANAGER_HEADER__
// @nolimitk
// 16.05.23
// iocp manager is a singleton

#include "../NKCore.h"
#include <MSWSock.h>
#include <vector>
#include "WinSockLib.h"

namespace NKNetwork
{
	class WorkerThread;
	class EventObject;
	class WorkerCallback;

	class IOCPManager : public NKCore::Singleton<IOCPManager>
	{
	public:
		bool create(void);
		void close(void);
		bool postEvent(const std::shared_ptr<EventObject>& event_object, int64_t param);

	public:
		SOCKET openSocket(void);
	
	public:
		static LPFN_CONNECTEX CONNECTEXFUNC;
		static const std::chrono::milliseconds DEFAULT_GAP_WORKERTHREAD_TIMEOUT;
		enum class COMPLETION_KEY : int64_t
		{
			PROCESS_EVENT = 0,
			END_EVENT = 1
		};

	protected:
		bool postStop(void);
		bool allWorkerThreadsJoinable(void);

	public:
		void registerWorkerEvent(const std::shared_ptr<WorkerCallback>& callback);

	protected:
		WinSockLib _win_sock_lib;
		HANDLE _completion_port;
		std::vector<std::unique_ptr<WorkerThread>> _worker_thread_vector;

	public:
		IOCPManager(void);
		virtual ~IOCPManager(void);
	};
}

#endif // __IOCPMANAGER_HEADER__
