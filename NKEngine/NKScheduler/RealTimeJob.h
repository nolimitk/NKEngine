#ifndef __REALTIMEJOB_HEADER__
#define __REALTIMEJOB_HEADER__
// @nolimitk
// 16.05.28
// realtime-job

#include "EventSlot.h"

namespace NKScheduler
{
	class Serializer;

	class RealTimeJob : public EventSlot
	{
	public:
		bool ReExecute(uint tick);
		bool ReExecute(void);

	public:
		void UnRegister(void);

	public:
		bool TryReserve(void);
		void ReleaseReserve(void);
		inline bool GetReserved(void) { return (_reserved != 0); }

	public:
		inline void registerAsShort(int slotIndex, uint64 executeIndex) { registerJob(1, slotIndex, executeIndex); }
		inline void registerAsLong(uint64 executeIndex) { registerJob(2, -1, executeIndex); }
		inline void registerAsInstant(uint64 executeIndex) { registerJob(3, -1, executeIndex); }
		inline void registerJob(int slotType, int slotIndex, uint64 executeIndex) { _slotType = slotType; _slotIndex = slotIndex; _executeSlotIndex = executeIndex; }

	public:
		inline int getSlotType(void) { return _slotType; }
		inline int getSlotIndex(void) { return _slotIndex; }
		inline uint64 getExecuteSlotIndex(void) { return _executeSlotIndex; }

	public:
		inline void SetContainer(Serializer *pSerializer) { _pContainer = pSerializer; }
		inline void SetTick(uint tick) { _tick = tick; }

	public:
		virtual bool Process(uint64 executeIndex) = 0;
		
	protected:
		// 재등록을 위해 사용한다.
		Serializer *_pContainer;
		uint _tick;

	protected:
		// @transaction reserved에 의해 모두 thread safe하다.
		volatile uint _reserved;
		// 1 short, 2 long , 3 instant
		int _slotType;
		int _slotIndex;
		uint64 _executeSlotIndex;

	public:
		RealTimeJob(void);
		virtual ~RealTimeJob(void);
	};
}

#endif // __REALTIMEJOB_HEADER__
