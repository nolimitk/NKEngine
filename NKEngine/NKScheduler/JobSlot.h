#pragma once
#ifndef __JOBSLOT_HEADER__
#define __JOBSLOT_HEADER__
// @nolimitk
// 18.06.09
// event slot

#include "../NKNetwork.h"
#include "RealTimeJob.h"

namespace NKScheduler
{
	// @nolimitk 50ms * 40, 20��, 20�� ������ job ������ ����Ұ����� ����, ��Ȳ�� �ð� ��������
	static const int JOBSLOT_SHORTTERM_SIZE = 400;

	class JobSlot : public NKNetwork::EventObject
	{
		/*
	public:
		void UnRegister(void);

	public:
		bool TryReserve(void);
		void ReleaseReserve(void);
		inline bool GetReserved(void) const { return (_reserved != 0); }

	public:
		inline void registerAsShort(int slotIndex, uint64_t executeIndex) { registerSlot(1, slotIndex, executeIndex); }
		inline void registerAsLong(uint64_t executeIndex) { registerSlot(2, -1, executeIndex); }
		inline void registerAsInstant(uint64_t executeIndex) { registerSlot(3, -1, executeIndex); }
		inline void registerSlot(int slotType, int slotIndex, uint64_t executeIndex) { _slotType = slotType; _slotIndex = slotIndex; _executeSlotIndex = executeIndex; }

	public:
		inline int getSlotType(void) const { return _slotType; }
		inline int getSlotIndex(void) const { return _slotIndex; }
		inline uint64_t getExecuteSlotIndex(void) const { return _executeSlotIndex; }
		*/

	public:
		virtual bool onProcess(NKNetwork::EventContext& event_context, uint32_t transferred) override;

	protected:
		/// @transaction reserved�� ���� ��� thread safe�ϴ�.
		//std::atomic<bool> _reserved;
		// 1 short, 2 long , 3 instant
		//int _slotType;
		//int _slotIndex;
		//uint64_t _executeSlotIndex;
		///

		NKCore::TWaitFreeQueue<RealTimeJob> _shortterm_slot[JOBSLOT_SHORTTERM_SIZE];
		
	public:
		JobSlot(void);
		virtual ~JobSlot(void);
	};
}

#endif // __JOBSLOT_HEADER__

