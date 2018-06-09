#pragma once
#ifndef __WORKERTHREAD_HEADER__
#define __WORKERTHREAD_HEADER__
// @nolimitk
// 16.05.23
// worker thread

#include "../NKCore.h"

namespace NKNetwork
{
	class WorkerThread : public NKCore::NKThread
	{
	public:
		const static int UPDATE_UNIT = 50 * 1000; // 50ms

	public:
		virtual bool onStart(void) override;
		virtual bool onRun(void) override;
		virtual bool onEnd(void) override;
		virtual bool onUpdate(int64_t delta);

	protected:
		const HANDLE _completion_port;
		NKCore::NKClock _clock;
		uint64_t _update_tick;
		
	public:
		WorkerThread(const HANDLE completion_port);
		virtual ~WorkerThread(void);
	};
}

#endif // __WORKERTHREAD_HEADER__
