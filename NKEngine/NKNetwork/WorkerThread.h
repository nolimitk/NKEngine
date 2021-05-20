#pragma once
#ifndef __WORKERTHREAD_HEADER__
#define __WORKERTHREAD_HEADER__
// @nolimitk
// 16.05.23
// worker thread

#include "../NKCore.h"
#include <list>

namespace NKNetwork
{
	class WorkerCallback;

	class WorkerThread : public NKCore::NKThread
	{
		/// worker events
	public:
		void registerCallback(const std::shared_ptr<WorkerCallback>& callback);

	protected:
		std::list<std::shared_ptr<WorkerCallback>> _callback_list;
		///

	public:
		const static std::chrono::microseconds UPDATE_UNIT;

	public:
		virtual bool onStart(void) override;
		virtual bool onRun(void) override;
		virtual bool onEnd(void) override;
		virtual bool onUpdate(int64_t delta);

	protected:
		const HANDLE _completion_port;
		NKCore::NKClock _clock;
		std::chrono::microseconds _next_update_time;
		
	public:
		WorkerThread(const HANDLE completion_port);
		virtual ~WorkerThread(void);
	};
}

#endif // __WORKERTHREAD_HEADER__
