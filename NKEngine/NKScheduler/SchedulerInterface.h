#pragma once
#ifndef __SCHEDULER_INTERFACE_HEADER__
#define __SCHEDULER_INTERFACE_HEADER__
// @nolimitk
// 18.06.30
// scheduler interface

#include "Scheduler.h"
#include "Serializer.h"
#include "RealTimeJob.h"
#include "../NKEngineLog.h"

namespace NKScheduler
{
	namespace internal
	{
		inline bool postJob(const RealTimeJobSP& job, const std::chrono::milliseconds& interval)
		{
			std::shared_ptr<JobSlot> slot = std::make_shared<JobSlot>();
			slot->_job = job;
			slot->_interval = interval;
			return NKNetwork::IOCPManager::getInstance()->postEvent(slot, 0);
		}

		inline bool addJob(const RealTimeJobSP& job, const std::chrono::milliseconds& interval)
		{
			if (_scheduler._in_workerthread == false)
			{
				return postJob(job, interval);
			}

			ExecutionIndex reserve_index = _scheduler._execution_index.add(interval);

			return _scheduler.addJob(job, reserve_index);
		}
	}

	inline bool addJob(const SerializerSP& serializer, const RealTimeJobSP& job, const std::chrono::milliseconds& interval)
	{
		NKENGINELOG_ASSERT_RETURN(serializer != nullptr);
		NKENGINELOG_ASSERT_RETURN(job != nullptr);
		NKENGINELOG_ASSERT_RETURN(interval > 0ms && interval < SCHEDULER_INTERVAL_UNIT * DEFAULT_JOBSLOT_SHORTTERM_SIZE);

		job->_serializer = serializer;

		return internal::addJob(job, interval);
	}
}

#endif // __SCHEDULER_INTERFACE_HEADER__