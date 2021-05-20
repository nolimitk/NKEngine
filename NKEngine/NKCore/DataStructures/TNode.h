#pragma once
#ifndef __TNODE_HEADER__
#define __TNODE_HEADER__
// @nolimitk
// 16.05.10
// template node

#include <memory>

namespace NKCore
{
	template<class T>
	class TNode
	{
	private:
		using iterator_type = std::shared_ptr<TNode<T>>;

	public:
		inline void setNext(const iterator_type& node) { _next = node; }
		inline iterator_type getNext(void) const { return _next; }
		inline T getValue(void) const { return _value; }

	protected:
		T _value;
		iterator_type _next;

	public:
		TNode(void)
			: _next(nullptr)
		{
		}

		TNode(const T& value)
			: _value(value)
			, _next(nullptr)
		{
		}

		virtual ~TNode(void) {}
	};
}

#endif // __TNODE_HEADER__