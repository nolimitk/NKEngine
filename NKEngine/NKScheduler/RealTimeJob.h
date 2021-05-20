#pragma once
#ifndef __REALTIMEJOB_HEADER__
#define __REALTIMEJOB_HEADER__
// @nolimitk
// 16.05.28
// realtime-job

#include "..\NKCore.h"
#include "Serializer.h"
#include "ExecutionIndex.h"

namespace NKScheduler
{
	class RealTimeJob
	{
		/// references
	public:
		SerializerSP _serializer;
		///

	public:
		virtual bool onExecute(uint64_t execute_index) = 0;

	public:
		RealTimeJob(void)
		{
		}
		virtual ~RealTimeJob(void)
		{
		}
	};

	class JobSlot : public NKNetwork::EventObject
	{
	public:
		std::shared_ptr<RealTimeJob> _job;
		std::chrono::milliseconds _interval;

	public:
		virtual bool onProcess(NKNetwork::EventContext& event_context, uint32_t transferred);
	};
}

#endif // __REALTIMEJOB_HEADER__
