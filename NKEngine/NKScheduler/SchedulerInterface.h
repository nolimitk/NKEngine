#pragma once
#ifndef __SCHEDULER_INTERFACE_HEADER__
#define __SCHEDULER_INTERFACE_HEADER__
// @nolimitk
// 18.06.30
// scheduler interface

#include "Scheduler.h"
#include "Serializer.h"
#include "RealTimeJob.h"

namespace NKScheduler
{
	inline bool addJob(const RealTimeJobSP& job, const SerializerSP& serializer, const std::chrono::milliseconds& interval)
	{
		uint64_t reserve_execution_index = NKScheduler::Scheduler::getInstance()->convertToExecutionIndex(interval);
		
		if (serializer->addJob(job, reserve_execution_index) == false)
		{
			return false;
		}
		
		NKScheduler::Scheduler::getInstance()->addSerializer(serializer, reserve_execution_index);
		return true;
	}
}

#endif // __SCHEDULER_INTERFACE_HEADER__