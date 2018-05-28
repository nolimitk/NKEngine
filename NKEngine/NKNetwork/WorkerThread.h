#pragma once
#ifndef __WORKERTHREAD_HEADER__
#define __WORKERTHREAD_HEADER__
// @nolimitk
// 16.05.23
// worker thread

#include "../NKCore.h"

namespace NKNetwork
{
	//class TimerNode : public NKCore::TNode<TimerNode>
	//{
	//public:
	//	bool tryReserve(void);
	//	bool releaseReserve(void);

	//protected:
	//	// @thread-safe, @wait-free
	//	std::atomic<bool> _reserved;
	//	///

	//public:
	//	TimerNode(void);
	//	virtual ~TimerNode(void);
	//};

	//class NKTimer
	//{
	//public:
	//	bool insert(int index, TimerNode &node);

	//public:
	//	bool onExpired(void);

	//protected:
	//	const static int DEFAULT_SHORTTERM_SIZE = 401;

	//protected:
	//	NKCore::TIndexedQueue<TimerNode> _shortTermJob;

	//public:
	//	NKTimer(void);
	//	NKTimer(int size);
	//	virtual ~NKTimer(void);
	//};

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

	//thread_local NKTimer _timer;
}

#endif // __WORKERTHREAD_HEADER__
