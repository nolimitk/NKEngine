#pragma once
#ifndef __SERIALIZER_HEADER__
#define __SERIALIZER_HEADER__
// @nolimitk
// 18.06.10
// serializer

#include "../NKCore.h"
#include "SchedulerConstants.h"
#include "../NKNetwork.h"
#include "RealTimeJob.h"

namespace NKScheduler
{
	class Serializer : public NKNetwork::EventObject
	{
	public:
		bool addJob(const RealTimeJobSP& job, uint64_t reserve_execution_index);
		
	public:
		std::atomic<bool> _reserved;
		std::atomic<uint64_t> _reserved_execution_index;

	public:
		bool canReserve(uint64_t reserve_execution_index) const { if (_reserved == true && _reserved_execution_index >= reserve_execution_index) { return false; } return true; }
		void setReserve(uint64_t reserve_execution_index) { _reserved = true; _reserved_execution_index = reserve_execution_index; }
		void setReserveFalse(void) { _reserved = false; }

	protected:
		static const int SCHEDULER_TIME_UNIT = 50;
		// index 50~99 -> 0, 100~149 -> 1, 150->199 -> 2, 200->249 -> 3,,, 950~999 -> 18
		int convertToExecuteIndex(int tick) { return (tick / SCHEDULER_TIME_UNIT) - 1; }
		
		/// short-term job slot
	protected:
		NKCore::TWaitFreeQueue2<RealTimeJobSP> _shortterm_slot[DEFAULT_JOBSLOT_SHORTTERM_SIZE];
		///

	protected:
		bool execute(uint64_t execution_index);

	public:
		virtual bool onProcess(NKNetwork::EventContext& event_context, uint32_t transferred) override;

	public:
		Serializer(void);
		virtual ~Serializer(void);
	};

	using SerializerSP = std::shared_ptr<Serializer>;
}

#endif // __SERIALIZER_HEADER__