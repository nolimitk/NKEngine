#pragma once
#ifndef __TSORTEDLIST_HEADER__
#define __TSORTEDLIST_HEADER__
// @nolimitk
// 16.05.10
// template sorted list

namespace NKCore
{
	// T must be derived from TNode
	template<typename T>
	class TSortedList
	{
	public:
		bool insert(T* node);
		T* pop(void);
		T* pop(uint64 key);
		T* pick(void) const { return _head; }
		bool erase(T* node);

	protected:
		T* _find(uint64 key);

	protected:
		T* _head;

	public:
		TSortedList(void)
			:_head(nullptr)
		{
		}
		virtual ~TSortedList(void) {}
	};

	template<typename T>
	bool TSortedList<T>::insert(T* node)
	{
		if (node == nullptr) return false;

		node->setNext(nullptr);

		if (_head == nullptr)
		{
			_head = node;
		}
		else
		{
			T* traverse;
			traverse = _head;
			while (traverse != nullptr)
			{
				if (*node < *traverse)
				{
					if (traverse == _head)
					{
						_head = node;
						node->setNext(traverse);
						traverse->setPrev(node);
						break;
					}
					else
					{
						node->setNext(traverse);
						node->setPrev(traverse->getPrev());
						traverse->getPrev()->setNext(node);
						traverse->setPrev(node);
						break;
					}
				}

				if (traverse->getNext() == nullptr)
				{
					traverse->setNext(node);
					node->setPrev(traverse);
					break;
				}

				traverse = traverse->getNext();
			}
		}

		return true;
	}

	template<typename T>
	T* TSortedList<T>::pop(void)
	{
		if (_head == nullptr) return nullptr;

		T* node = _head;

		if (_head->getNext() != nullptr)
		{
			_head->getNext()->setPrev(nullptr);
		}

		_head = _head->getNext();
		
		node->setNext(nullptr);
		node->setPrev(nullptr);

		return node;
	}

	template<typename T>
	T* TSortedList<T>::pop(uint64 key)
	{
		if (_head == nullptr) return nullptr;

		T* node = _find(key);

		if (node != nullptr)
		{
			T* ret = _head;

			_head = node->getNext();

			if (_head != nullptr)
			{
				_head->setPrev(nullptr);
			}
			node->setNext(nullptr);

			return ret;
		}

		return nullptr;
	}

	template<typename T>
	bool TSortedList<T>::erase(T* node)
	{
		if (node == nullptr) return false;

		if (node == _head)
		{
			_head = node->getNext();
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
	}

	template<typename T>
	T* TSortedList<T>::_find(uint64 key)
	{
		if (_head == nullptr) return nullptr;

		// return node which is less than index.
		T* current = _head;
		T* prev = nullptr;

		while (current != nullptr)
		{
			if (*current > key)
			{
				return prev;
			}

			prev = current;
			current = current->getNext();
		}

		return prev;
	}
}

#endif // __TSORTEDLIST_HEADER__