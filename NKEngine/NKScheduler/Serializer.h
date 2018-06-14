#pragma once
#ifndef __SERIALIZER_HEADER__
#define __SERIALIZER_HEADER__
// @nolimitk
// 18.06.10
// serializer

#include "../NKCore.h"
#include "../NKNetwork.h"
#include "RealTimeJob.h"

namespace NKScheduler
{
	class Serializer : public NKNetwork::EventObject, public NKCore::TNode2<Serializer>
	{
	public:
		bool addJob(const RealTimeJobSP& job, uint32_t tick = SCHEDULER_TIME_UNIT);

		/*
	public:
		bool DeleteTimeJob(RealTimeJob *pJob);
		bool ResetTimeJob(RealTimeJob *pJob, uint tick);

	public:
		bool RegisterScheduler(uint tick);
				
	protected:
		// @TODO lock�� ���� �ʴ� ����� ������ ����.
		// @TODO Serializer�� �ϳ��� thread������ ������ �ȴٰ� ������ �Ǹ� lock�� ���� �ʾƵ� �ȴ�.
		// @TODO Serializer�� job�� �߰��ϴ� thread�� �ϳ��� �Ǿ�� �Ѵ�.
		SerializeContainer _container;
		volatile uint _lastReservedTick;
		uint64 _lastExecutionIndex;
		*/

	protected:
		static const int RECOMMAND_SHORTTERMJOB_SIZE = 40;
		static const int SCHEDULER_TIME_UNIT = 50;

	protected:
		NKCore::TWaitFreeQueue<RealTimeJob> _shortterm_slot[RECOMMAND_SHORTTERMJOB_SIZE];

	protected:
		bool execute(uint64_t execute_index);

	public:
		virtual bool onProcess(NKNetwork::EventContext& event_context, uint32_t transferred) override;

	public:
		Serializer(void);
		virtual ~Serializer(void);
	};

	using SerializerSP = std::shared_ptr<Serializer>;
}

#endif // __SERIALIZER_HEADER__