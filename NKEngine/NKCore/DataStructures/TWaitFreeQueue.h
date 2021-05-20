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
	class TWaitFreeQueue
	{
	public:
		using iterator_type = std::shared_ptr<TNode<T>>;

	public:
		bool push(const T& node);
		iterator_type popQueue(void);

	protected:
		iterator_type _head;
		iterator_type _tail;

	public:
		TWaitFreeQueue(void)
			: _head(nullptr)
			, _tail(nullptr)
		{

		}
		virtual ~TWaitFreeQueue(void)
		{
		}
	};
	
	template<class T>
	inline bool TWaitFreeQueue<T>::push(const T& value)
	{
		iterator_type node = std::make_shared<TNode<T>>(value);

		iterator_type pos = std::atomic_exchange(&_tail, node);

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
	inline typename TWaitFreeQueue<T>::iterator_type TWaitFreeQueue<T>::popQueue(void)
	{
		iterator_type pos = std::atomic_exchange(&_tail, iterator_type(nullptr));
		if (pos == nullptr)
		{
			return nullptr;
		}
		return _head;
	}
}

#endif // __TWAITFREEQUEUE_HEADER__