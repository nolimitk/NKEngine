#pragma once
#ifndef __SCHEDULER_HEADER__
#define __SCHEDULER_HEADER__
// @nolimitk
// 13.09.04
// scheduler

#include "../NKCore.h"
#include "SchedulerConstants.h"
#include "Serializer.h"

namespace NKScheduler
{
	class SchedulerClock
	{
	public:
		inline std::chrono::microseconds getElapsedTime(void) const { return tick.getElapsedTime(); }
		inline void sleep(std::chrono::milliseconds wait_time) { tick.sleepAccurate(wait_time); }
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
		bool addSerializer(const SerializerSP& serializer, uint64_t reserve_execution_index);

	protected:
		SchedulerClock _clock;

	public:
		uint64_t convertToExecutionIndex(const std::chrono::milliseconds& interval);
		
		/// slot
	protected:
		static const std::chrono::microseconds SCHEDULER_INTERVAL_MARGIN;
		static const std::chrono::microseconds SCHEDULER_INTERVAL_UNIT;

	protected:
		NKCore::TWaitFreeQueue<Serializer> _shortterm_slot[DEFAULT_JOBSLOT_SHORTTERM_SIZE];
		///
		
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