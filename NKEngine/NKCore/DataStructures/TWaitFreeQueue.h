#pragma once
#ifndef __TWAITFREEQUEUE_HEADER__
#define __TWAITFREEQUEUE_HEADER__
// @nolimitk
// 18.06.09
// template waitfree queue

#include <atomic>

namespace NKCore
{
	template<class T>
	class TNode2
	{
	public:
		inline void setNext(const std::shared_ptr<T>& node) { _next = node; }
		inline std::shared_ptr<T> getNext(void) const { return _next; }

	protected:
		std::shared_ptr<T> _next;

	public:
		TNode2(void)
			: _next(nullptr)
		{
		}

		virtual ~TNode2(void) {}
	};

	template<class T>
	class TWaitFreeQueue
	{
	public:
		bool push(const std::shared_ptr<T>& node);
		std::shared_ptr<T> popQueue(void);

	protected:
		std::shared_ptr<T> _head;
		std::shared_ptr<T> _tail;

	public:
		TWaitFreeQueue(void)
			:_head(nullptr), _tail(nullptr)
		{

		}
		virtual ~TWaitFreeQueue(void)
		{
		}
	};

	template<class T>
	inline bool TWaitFreeQueue<T>::push(const std::shared_ptr<T>& node)
	{
		if (node == nullptr) return false;
				
		std::shared_ptr<T> pos = std::atomic_exchange(&_tail, node);

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
	template<class T>
	inline std::shared_ptr<T> TWaitFreeQueue<T>::popQueue(void)
	{
		std::shared_ptr<T> pos = std::atomic_exchange(&_tail, std::shared_ptr<T>(nullptr));
		if (pos == nullptr)
		{
			return pos; // return nullptr;
		}
		return _head;
	}
}

#endif // __TWAITFREEQUEUE_HEADER__