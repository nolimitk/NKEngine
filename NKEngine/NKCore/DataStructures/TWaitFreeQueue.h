#pragma once
#ifndef __TWAITFREEQUEUE_HEADER__
#define __TWAITFREEQUEUE_HEADER__
// @nolimitk
// 18.06.09
// template waitfree queue

#include <atomic>

namespace NKCore
{
	// T must be derived from TNode
	template<typename T>
	class TWaitFreeQueue
	{
	public:
		bool push(T* node);
		// @not-thread-safe
		T* popQueue(void);

	protected:
		T * _head;
		std::atomic<T*> _tail;

	public:
		TWaitFreeQueue(void)
			:_head(nullptr), _tail(nullptr)
		{

		}
		virtual ~TWaitFreeQueue(void)
		{
		}
	};

	template<typename T>
	inline bool TWaitFreeQueue<T>::push(T * node)
	{
		if (node == nullptr) return false;

		T* pos = _tail.exchange(node);

		if (pos == nullptr)
		{
			_head = node;
		}
		else
		{
			pos->setNext(node);
		}
		return true;
	}
	template<typename T>
	inline T * TWaitFreeQueue<T>::popQueue(void)
	{
		T* pos = _tail.exchange(nullptr);
		if (pos == nullptr)
		{
			return nullptr;
		}
		return _head;
	}
}

#endif // __TWAITFREEQUEUE_HEADER__