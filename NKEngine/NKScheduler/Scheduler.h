#pragma once
#ifndef __SCHEDULER_HEADER__
#define __SCHEDULER_HEADER__
// @nolimitk
// 13.09.04
// scheduler

#include "../NKCore.h"
//#include "JobSlot.h"
//#include "ScheduleContainer.h"

namespace NKScheduler
{
	class Serializer;

	class SchedulerClock
	{
	public:
		inline int64_t getElapsedMicroSec(void) const { return tick.getElapsedMicroSec(); }
		inline void sleep(int64_t wait_time) { tick.sleepAccurate(wait_time); }
	protected:
		NKCore::NKClock tick;
			
		///
	public:
		inline uint64_t executionIndex(void) const { return _execution_index; }
		inline void increaseIndex(void) { _execution_index++; }

	protected:
		volatile uint64_t _execution_index;
		///

	public:
		SchedulerClock(void):_execution_index(0){}
		virtual ~SchedulerClock(void) {}
	};

	class SchedulerThread : public NKCore::NKThread
	{
	public:
		bool onStart(void) override;
		bool onRun(void) override;
		bool onEnd(void) override;

	protected:
		NKCore::NKClock _clock;

	public:
		SchedulerThread(void) {}
		virtual ~SchedulerThread(void) {}
	};

	class Scheduler : public NKCore::Singleton<Scheduler>
	{
	public:
		bool start(void);		
		bool stop(void);

	public:
		bool addSerializer(const std::shared_ptr<Serializer>& serializer, uint32_t tick = SCHEDULER_TIME_UNIT);

		/*
	public:
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
		
		/// slot
	protected:
		static const int RECOMMAND_SHORTTERMJOB_SIZE = 40;
		static const int SCHEDULER_TIME_UNIT = 50;

	protected:
		NKCore::TWaitFreeQueue<Serializer> _shortterm_slot[RECOMMAND_SHORTTERMJOB_SIZE];
		//
		
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