#pragma once
#ifndef __SCHEDULER_HEADER__
#define __SCHEDULER_HEADER__
// @nolimitk
// 13.09.04
// scheduler

#include <vector>
#include "SchedulerConstants.h"
#include "RealTimeJob.h"
#include "ExecutionIndex.h"

namespace NKScheduler
{
	class Scheduler
	{
		/// references
	public:
		ExecutionIndex _execution_index;
		bool _in_workerthread;
		///

	public:
		bool addJob(const RealTimeJobSP& job, ExecutionIndex reserve_index);
		
	protected:
		using RealTimeJobQueue = NKCore::TQueue<RealTimeJobSP>;
		RealTimeJobQueue _shortterm_slot[DEFAULT_JOBSLOT_SHORTTERM_SIZE];
		
	public:
		bool execute(void);

	public:
		Scheduler(void);
		virtual ~Scheduler(void);
	};

	extern thread_local Scheduler _scheduler;
}

#endif // __SCHEDULER_HEADER__