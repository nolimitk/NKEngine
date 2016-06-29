#ifndef __SERIALIZER_HEADER__
#define __SERIALIZER_HEADER__
// @nolimitk
// 13.09.06
// serializer

#include "EventSlot.h"
#include "RealTimeJob.h"
//#include "SerializeContainer.h"

namespace NKScheduler
{
	class Serializer : public EventSlot
	{
	public:
		bool Execute(int64 executeIndex);

	public:
		bool AddTimeJob(RealTimeJob *pJob, uint tick);
		bool DeleteTimeJob(RealTimeJob *pJob);
		bool ResetTimeJob(RealTimeJob *pJob, uint tick);

	public:
		bool RegisterScheduler(uint tick);

	public:
		virtual bool Process(NKNetwork::EventContext *pEventContext, DWORD transferred);

	public:
		static const int RECOMMAND_SHORTTERMJOB_SIZE = 41;
		static const int SCHEDULER_TIME_UNIT = 50;

	protected:
		// @TODO lock을 쓰지 않는 방법을 연구해 본다.
		// @TODO Serializer가 하나의 thread에서만 실행이 된다고 보장이 되면 lock을 걸지 않아도 된다.
		// @TODO Serializer에 job을 추가하는 thread도 하나가 되어야 한다.
		SerializeContainer _container;
		volatile uint _lastReservedTick;
		uint64 _lastExecutionIndex;

	public:
		Serializer(void);
		virtual ~Serializer(void);
	};
}

#endif // __SERIALIZER_HEADER__