#pragma once
#ifndef __TINDEXEDQUEUE_HEADER__
#define __TINDEXEDQUEUE_HEADER__
// @nolimitk
// 16.05.10
// template indexed queue

#include <memory>
#include "TQueue.h"

namespace NKCore
{
	// T must be derived from TNode
	template<typename T>
	class TIndexedQueue
	{
	public:
		bool push(int index, T* node);
		T* popQueue(int index);
		bool popQueue(int index, NKCore::TQueue<T> &queue);
		T* pickQueue(int index) const;
		bool erase(int index, T* node);

	protected:
		int _size_index;
		std::unique_ptr<TQueue<T>[]> _queue;

	public:
		TIndexedQueue(int size_index)
			:_size_index(size_index)
		{
			if (size_index > 0)
			{
				_queue = std::make_unique<TQueue<T>[]>(size_index);
			}
		}
		virtual ~TIndexedQueue(void) {}
	};

	template<typename T>
	bool TIndexedQueue<T>::push(int index, T* node)
	{
		if (index < 0 || index >= _size_index) return false;
		if (node == nullptr) return false;

		_queue[index].push(node);
		return true;
	}

	template<typename T>
	T* TIndexedQueue<T>::popQueue(int index)
	{
		if (index < 0 || index >= _size_index) return nullptr;

		return _queue[index].popQueue();
	}

	template<typename T>
	bool TIndexedQueue<T>::popQueue(int index, TQueue<T>& queue)
	{
		if (index < 0 || index >= _size_index) return false;

		queue = _queue[index];
		_queue[index].popQueue();
		return true;
	}

	template<typename T>
	T* TIndexedQueue<T>::pickQueue(int index) const
	{
		if (index < 0 || index >= _size_index) return nullptr;

		return _queue[index].pick();
	}

	template<typename T>
	bool TIndexedQueue<T>::erase(int index, T* node)
	{
		if (index < 0 || index >= _size_index) return false;
		if (node == nullptr) return false;

		_queue[index].erase(node);
		return true;
	}
}

#endif // __TINDEXEDQUEUE_HEADER__