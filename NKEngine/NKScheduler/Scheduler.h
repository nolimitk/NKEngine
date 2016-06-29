#ifndef __SCHEDULER_HEADER__
#define __SCHEDULER_HEADER__
// @nolimitk
// 13.09.04
// scheduler

#include "../NKCore.h"
#include "EventSlot.h"
//#include "ScheduleContainer.h"

namespace NKScheduler
{
	class SchedulerClock : public NKCore::Singleton<SchedulerClock>
	{
	protected:
		volatile uint64 _executionIndex;

	public:
		SchedulerClock(void):_executionIndex(0){}
		virtual ~SchedulerClock(void) {}
	};

	class SchedulerThread : public NKCore::NKThread
	{
	public:
		virtual bool onRun(void) override;

	public:
		SchedulerThread(void) {}
		virtual ~SchedulerThread(void) {}
	};

	//class Scheduler// : public NKCore::Singleton<Scheduler>
	//{
	//public:
	//	bool Create(int size = DEFAULT_SHORTTERMSLOT_SIZE, bool realtime = true);
	//	bool Start(void);
	//	bool Execute(void);
	//	bool Stop(void);

	//public:
	//	bool AddSlot(EventSlot *pSlot, uint tick);
	//	bool DeleteSlot(EventSlot *pSlot);
	//	bool ResetSlot(EventSlot *pSlot, uint tick);
	//	uint64 GetExecutionIndex(void) { return _executionIndex; }

	//protected:
	//	static const int DEFAULT_SHORTTERMSLOT_SIZE = 31;
	//	static const int64 SCHEDULER_TIME_UNIT;


	//protected:
	//	bool _bExecute;
	//	SchedulerThread _thread;

	//	//ScheduleContainer _container;
	//	volatile uint64 _executionIndex;

	//	int _sizeShortTermSlot;
	//	bool _realtime;

	//public:
	//	Scheduler(uint size);
	//	virtual ~Scheduler(void);
	//};

	//Scheduler* getScheduler(void);
}

#endif // __SCHEDULER_HEADER__