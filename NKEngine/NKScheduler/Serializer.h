#pragma once
#ifndef __SERIALIZER_HEADER__
#define __SERIALIZER_HEADER__
// @nolimitk
// 18.06.10
// serializer

#include <memory>
#include "../NKCore.h"
#include "../NKNetwork.h"

namespace NKScheduler
{
	class RealTimeJob;
	using RealTimeJobSP = std::shared_ptr<RealTimeJob>;

	class Serializer
	{
	public:
		using RealTimeJobQueue = NKCore::TSpinLockQueue<RealTimeJobSP>;
		RealTimeJobQueue _queue;

	public:
		bool execute(uint64_t execution_index, const RealTimeJobSP& job);
		void onExecute(uint64_t execution_index);

		//DWORD _current_thread_id;
		std::atomic<int> _count;

	public:
		Serializer(void);
		virtual ~Serializer(void);
	};

	using SerializerSP = std::shared_ptr<Serializer>;
}

#endif // __SERIALIZER_HEADER__