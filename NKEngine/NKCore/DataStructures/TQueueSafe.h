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
	// this is lock-free guaranteed spinlock
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

	// T must be derived from TNode
	template<typename T, typename _Mutex>
	class TQueueSafe
	{
	public:
		bool push(T* node);
		T* pop(void);
		T* popQueue(void);
		inline T* pick(void) { return _head; }

	protected:
		_Mutex _mutex;
		T* _head;
		T* _tail;

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
	bool TQueueSafe<T, _Mutex>::push(T* node)
	{
		if (node == nullptr) return false;

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

	template<typename T, typename _Mutex>
	T* TQueueSafe<T, _Mutex>::pop(void)
	{
		std::lock_guard<_Mutex> _lock(_mutex);

		if (_head == nullptr) return nullptr;

		T* node = _head;

		if (node == _tail) { _tail = nullptr; }

		_head = _head->getNext();
		node->setNext(nullptr);

		return node;
	}

	template<typename T, typename _Mutex>
	T* TQueueSafe<T, _Mutex>::popQueue(void)
	{
		std::lock_guard<_Mutex> _lock(_mutex);

		T* node = _head;
		_tail = _head = nullptr;

		return node;
	}

	template<typename T>
	using TLockQueue = TQueueSafe<T, std::mutex>;

	template<typename T>
	using TLockFreeQueue = TQueueSafe<T, SpinLock>;
}

#endif // __TQUEUESAFE_HEADER__