#ifndef __EVENTSLOT_HEADER__
#define __EVENTSLOT_HEADER__
// @nolimitk
// 13.09.05
// event slot

#include "Event.h"
#include <atomic>

namespace NKNetwork
{
	class EventSlot : public EventObject
	{
	public:
		void UnRegister(void);

	public:
		bool TryReserve(void);
		void ReleaseReserve(void);
		inline bool GetReserved(void) { return (_reserved != 0); }

	public:
		inline void registerAsShort(int slotIndex, uint64_t executeIndex) { registerSlot(1, slotIndex, executeIndex); }
		inline void registerAsLong(uint64_t executeIndex) { registerSlot(2, -1, executeIndex); }
		inline void registerAsInstant(uint64_t executeIndex) { registerSlot(3, -1, executeIndex); }
		inline void registerSlot(int slotType, int slotIndex, uint64_t executeIndex) { _slotType = slotType; _slotIndex = slotIndex; _executeSlotIndex = executeIndex; }

	public:
		inline void SetNext(EventSlot *pNext) { _pNext = pNext; }
		inline EventSlot *GetNext(void) { return _pNext; }
		inline void SetPrev(EventSlot *pPrev) { _pPrev = pPrev; }
		inline EventSlot *GetPrev(void) { return _pPrev; }
		inline int getSlotType(void) { return _slotType; }
		inline int getSlotIndex(void) { return _slotIndex; }
		inline uint64_t getExecuteSlotIndex(void) { return _executeSlotIndex; }

	public:
		virtual bool onProcess(EventContext& event_context, uint32_t transferred) override;

	protected:
		// @transaction reserved에 의해 모두 thread safe하다.
		std::atomic<bool> _reserved;
		EventSlot *_pNext;
		EventSlot *_pPrev;
		// 1 short, 2 long , 3 instant
		int _slotType;
		int _slotIndex;
		uint64_t _executeSlotIndex;

	public:
		EventSlot(void);
		virtual ~EventSlot(void);
	};
}

#endif // __EVENTSLOT_HEADER__

