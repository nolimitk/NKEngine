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

	class IOCPManager : public NKCore::Singleton<IOCPManager>
	{
	public:
		bool create(void);
		void close(void);
		bool postEvent(std::shared_ptr<EventObject>& event_object, int64_t param);
	
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

	protected:
		inline HANDLE getCompletionPort(void) const { return _completion_port; }

	protected:
		WinSockLib _win_sock_lib;
		HANDLE _completion_port;
		std::vector<std::unique_ptr<WorkerThread>> _worker_thread_vector;

	public:
		IOCPManager(void);
		virtual ~IOCPManager(void);

		friend class WorkerThread;
		friend class AsyncServerSocket;
		friend class Connection;
	};
}

#endif // __IOCPMANAGER_HEADER__
