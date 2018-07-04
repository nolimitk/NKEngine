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
	class TNode3
	{
	private:
		using iterator_type = std::shared_ptr<TNode3<T>>;

	public:
		inline void setNext(const iterator_type& node) { _next = node; }
		inline iterator_type getNext(void) const { return _next; }
		inline T getValue(void) const { return _value; }

	protected:
		T _value;
		iterator_type _next;
		
	public:
		TNode3(void)
			: _next(nullptr)
		{
		}

		TNode3(const T& value)
			: _value(value)
			, _next(nullptr)
		{
		}

		virtual ~TNode3(void) {}
	};

	template<class T>
	class TWaitFreeQueue2
	{
	public:
		using iterator_type = std::shared_ptr<TNode3<T>>;

	public:
		bool push(const T& node);
		iterator_type popQueue(void);

	protected:
		iterator_type _head;
		iterator_type _tail;

	public:
		TWaitFreeQueue2(void)
			:_head(nullptr), _tail(nullptr)
		{

		}
		virtual ~TWaitFreeQueue2(void)
		{
		}
	};
	
	template<class T>
	inline bool TWaitFreeQueue2<T>::push(const T& value)
	{
		iterator_type node = std::make_shared<TNode3<T>>(value);

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
	inline typename TWaitFreeQueue2<T>::iterator_type TWaitFreeQueue2<T>::popQueue(void)
	{
		iterator_type pos = std::atomic_exchange(&_tail, iterator_type(nullptr));
		if (pos == nullptr)
		{
			return nullptr;
		}
		return _head;
	}

	template<class T>
	class TNode2
	{
	public:
		inline void setNext(const std::shared_ptr<T>& node) { _next = node; }
		inline std::shared_ptr<T> getNext(void) const { return _next; }

	protected:
		std::shared_ptr<T> _next;

	public:
		std::atomic<bool> _inserted;

	public:
		TNode2(void)
			: _next(nullptr)
			, _inserted(false)
		{
		}

		virtual ~TNode2(void) {}
	};

	template<class T>
	class TWaitFreeQueue
	{
	public:
		bool push(const std::shared_ptr<T>& node);
		bool pushBack(const std::shared_ptr<T>& node);
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