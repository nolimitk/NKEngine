#pragma once
#ifndef __TQUEUE_HEADER__
#define __TQUEUE_HEADER__
// @nolimitk
// 16.05.10
// template queue
// @todo think about the cache hit

#include "TNode.h"

namespace NKCore
{
	template<typename T>
	class TQueue
	{
	public:
		using iterator_type = std::shared_ptr<TNode<T>>;

	public:
		bool push(const T& value);
		//iterator_type pop(void);
		iterator_type popQueue(void);
		inline iterator_type pick(void) const { return _head; };
		//bool erase(T* node);

	protected:
		iterator_type _head;
		iterator_type _tail;

	public:
		TQueue(void)
			:_head(nullptr)
			, _tail(nullptr)
		{
		}
		
		TQueue(const TQueue& queue)
			:_head(queue._head)
			, _tail(queue._tail)
		{
		}

		virtual ~TQueue(void) {}
	};

	template<typename T>
	bool TQueue<T>::push(const T& value)
	{
		iterator_type node = std::make_shared<TNode<T>>(value);

		node->setNext(nullptr);

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

	/*template<typename T>
	T* TQueue<T>::pop(void)
	{
		if (_head == nullptr) return nullptr;

		T* node = _head;

		if (node == _tail) { _tail = nullptr; }

		_head = _head->getNext();
		node->setNext(nullptr);

		return node;
	}*/

	template<typename T>
	typename TQueue<T>::iterator_type TQueue<T>::popQueue(void)
	{
		iterator_type node = _head;

		_tail = _head = nullptr;

		return node;
	}

	/*template<typename T>
	bool TQueue<T>::erase(T* node)
	{
		if (node == nullptr) return false;

		if (node == _head)
		{
			_head = node->getNext();
		}
		if (node == _tail)
		{
			_tail = node->getPrev();
		}

		if (node->getNext() != nullptr)
		{
			node->getNext()->setPrev(node->getPrev());
		}

		if (node->getPrev() != nullptr)
		{
			node->getPrev()->setNext(node->getNext());
		}
		return true;
	}*/
}

#endif // __TQUEUE_HEADER__