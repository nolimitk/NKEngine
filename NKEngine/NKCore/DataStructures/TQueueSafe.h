#pragma once
#ifndef __TQUEUESAFE_HEADER__
#define __TQUEUESAFE_HEADER__
// @nolimitk
// 16.04.23
// template queue with thread safe
// using shared pointer

#include <memory>
#include <mutex>
#include <atomic>

namespace NKCore
{
	class SpinLock
	{
	public:
		void lock(void)
		{
			while (_flag.test_and_set());
		}

		void unlock(void)
		{
			_flag.clear();
		}

	protected:
		std::atomic_flag _flag = ATOMIC_FLAG_INIT;

	public:
		SpinLock(void)
		{
		}

		virtual ~SpinLock(void)
		{
		}
	};

	template<typename T, typename _Mutex>
	class TQueueSafe
	{
	public:
		using iterator_type = std::shared_ptr<TNode<T>>;

	public:
		bool push(const T& value);
		iterator_type popQueue(void);
		//iterator_type pick(void);

	protected:
		_Mutex _mutex;
		iterator_type _head;
		iterator_type _tail;

	public:
		TQueueSafe(void)
			:_head(nullptr)
			, _tail(nullptr)
		{
		}
		TQueueSafe(const TQueueSafe& queue)
			:_head(queue._head)
			, _tail(queue._tail)
		{
		}
		virtual ~TQueueSafe(void) {}
	};

	template<typename T, typename _Mutex>
	bool TQueueSafe<T, _Mutex>::push(const T& value)
	{
		iterator_type node = std::make_shared<TNode<T>>(value);

		node->setNext(nullptr);

		std::lock_guard<_Mutex> _lock(_mutex);

		if (_tail == nullptr)
		{
			_tail = _head = node;
		}
		else
		{
			_tail->setNext(node);
			_tail = node;
		}
		return true;
	}
	
	/*template<typename T, typename _Mutex>
	typename TQueueSafe<T, _Mutex>::iterator_type TQueueSafe<T, _Mutex>::pick(void)
	{
		std::lock_guard<_Mutex> _lock(_mutex);
		return _head;
	}*/

	template<typename T, typename _Mutex>
	typename TQueueSafe<T, _Mutex>::iterator_type TQueueSafe<T, _Mutex>::popQueue(void)
	{
		std::lock_guard<_Mutex> _lock(_mutex);

		iterator_type node = _head;
		_tail = _head = nullptr;

		return node;
	}

	template<typename T>
	using TLockQueue = TQueueSafe<T, std::mutex>;

	template<typename T>
	using TSpinLockQueue = TQueueSafe<T, SpinLock>;
}

#endif // __TQUEUESAFE_HEADER__