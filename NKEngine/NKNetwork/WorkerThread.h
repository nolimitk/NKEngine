#pragma once
#ifndef __WORKERTHREAD_HEADER__
#define __WORKERTHREAD_HEADER__
// @nolimitk
// 16.05.23
// worker thread

#include "../NKCore.h"

namespace NKNetwork
{
	class IOCPManager;

	class WorkerThread : public NKCore::NKThread
	{
	public:
		virtual bool onStart(void) override;
		virtual bool onRun(void) override;
		virtual bool onEnd(void) override;

	protected:
		HANDLE _completion_port;
		
	public:
		WorkerThread(const IOCPManager& iocp_manager);
		virtual ~WorkerThread(void);
	};
}

#endif // __WORKERTHREAD_HEADER__
