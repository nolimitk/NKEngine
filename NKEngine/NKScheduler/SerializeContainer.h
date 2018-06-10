#ifndef __SERIALIZECONTAINER_HEADER__
#define __SERIALIZECONTAINER_HEADER__
// @nolimitk
// 16.05.28
// serialize container( sorted list + indexed queue + instant execution queue )

#include "../NKCore.h"
#include "RealTimeJob.h"

namespace NKScheduler
{
	class SerializeContainer
	{
	//public:
	//	bool Destroy(void);

	//	bool insert(int index, RealTimeJob *pJob);
	//	RealTimeJob *PopQueue(int64 index);
	//	RealTimeJob *PickQueue(int64 index);
	//	bool Remove(RealTimeJob *pJob);
	//	// lock을 최소화 하기 위해 따로 제공한다.
	//	bool RemoveAndInsert(int index, RealTimeJob *pJob);
	//	RealTimeJob *PopInstantQueue(void);
	//	
	//protected:
	//	std::mutex _mutexShortTerm;
	//	NKCore::TIndexedQueue<RealTimeJob> _shortTermJob;
	//	std::mutex _mutexLongTerm;
	//	NKCore::TSortedList<RealTimeJob> _longTermJob;

	//	uint _sizeShortTermSlot;

	//protected:
	//	NKCore::TSpinLockQueue<RealTimeJob> _instant_queue;
		
	public:
		//SerializeContainer(uint size);
		SerializeContainer(void);
		virtual ~SerializeContainer(void);
	};
}

#endif // __SERIALIZECONTAINER_HEADER__