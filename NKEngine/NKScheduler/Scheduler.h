#pragma once
#ifndef __SCHEDULER_HEADER__
#define __SCHEDULER_HEADER__
// @nolimitk
// 13.09.04
// scheduler

#include "../NKCore.h"
#include "JobSlot.h"
//#include "ScheduleContainer.h"

namespace NKScheduler
{
	class SchedulerClock : public NKCore::Singleton<SchedulerClock>
	{
	protected:
		volatile uint64_t _executionIndex;

	public:
		SchedulerClock(void):_executionIndex(0){}
		virtual ~SchedulerClock(void) {}
	};

	class SchedulerThread : public NKCore::NKThread
	{
	public:
		bool onRun(void) override;

	public:
		SchedulerThread(void) {}
		virtual ~SchedulerThread(void) {}
	};

	class Scheduler : public NKCore::Singleton<Scheduler>
	{
	public:
		bool start(void);		
		bool stop(void);

		/*
	public:
		bool AddSlot(EventSlot *pSlot, uint32_t tick);
		bool DeleteSlot(EventSlot *pSlot);
		bool ResetSlot(EventSlot *pSlot, uint32_t tick);
		uint64_t GetExecutionIndex(void) { return _executionIndex; }

	protected:
		static const int DEFAULT_SHORTTERMSLOT_SIZE = 31;
		static const int64_t SCHEDULER_TIME_UNIT;
		//ScheduleContainer _container;
		int _sizeShortTermSlot;
		bool _realtime;
		*/

	protected:
		NKCore::TIndexedQueue<RealTimeJob> _shortterm_queue;
		
		/// scheduler thread
	protected:
		bool execute(void);

	protected:
		SchedulerThread _thread;

		friend SchedulerThread;
		///

	public:
		Scheduler(void);
		virtual ~Scheduler(void);
	};
}

#endif // __SCHEDULER_HEADER__