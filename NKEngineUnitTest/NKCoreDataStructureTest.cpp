#pragma once
#ifndef __NKCOREDATASTRUCTURETEST_HEADER__
#define __NKCOREDATASTRUCTURETEST_HEADER__
// @nolimitk
// 16.04.22
// core data structure test

#include <iostream>
#include <queue>
#include <string>

#include "NKEngine.h"
#include "NKUnittest.h"

class MockNode : public NKCore::TNode<MockNode>
{
public:
	inline int64_t getKey(void) { return _key;  }

public:
	int _v;

public:
	MockNode(uint64_t key)
		:TNode<MockNode>(key)
	{
	}
};

typedef std::vector<std::unique_ptr<MockNode>> MockNodeVector;

template<typename Q>
void queueTest(Q& queue, int count)
{
	_ASSERT(count != 0);

	// push
	for( int i = 0 ; i < count ; ++i )
	{
		MockNode* mock = new MockNode(i);
		mock->_v = 10 * i;
		_ASSERT(queue.push(mock) == true);
	}

	// pick
	{
		MockNode* trace = queue.pick();
		_ASSERT(trace != nullptr);
		
		int j = 0;
		while (trace != nullptr)
		{
			_ASSERT(trace != nullptr);
			_ASSERT(trace->_v == 10 * j++);
			// queue dose not have to use prev node
			_ASSERT(trace->getPrev() == nullptr);

			trace = trace->getNext();
		}
		_ASSERT(j == count);
	}
	
	// pop
	for (int i = 0; i < count; ++i)
	{
		MockNode* mock = queue.pop();
		_ASSERT(mock != nullptr);
		_ASSERT(mock->_v == 10*i);
		_ASSERT(mock->getNext() == nullptr);
		_ASSERT(mock->getPrev() == nullptr);
		SAFE_DELETE(mock);
	}
}

template<typename Q>
void popQueueTest(Q& queue, int count)
{
	_ASSERT(count != 0);

	// push
	for (int i = 0; i < count; ++i)
	{
		MockNode* mock = new MockNode(i);
		mock->_v = 10 * i;
		_ASSERT(queue.push(mock) == true);
	}

	// pop queue
	MockNode* trace = queue.popQueue();
	_ASSERT(trace != nullptr);
	MockNode* pDelete = nullptr;

	int j = 0;
	while (trace != nullptr)
	{
		_ASSERT(trace != nullptr);
		_ASSERT(trace->_v == 10 * j++);
		// queue dose not have to use prev node
		_ASSERT(trace->getPrev() == nullptr);

		pDelete = trace;
		trace = trace->getNext();
		SAFE_DELETE(pDelete);
	}
	_ASSERT(j == count);
}

NKTEST(TQueue_Test)
{	
	NKCore::TQueue<MockNode> queue;

	queueTest<NKCore::TQueue<MockNode>>(queue, 1);
	queueTest<NKCore::TQueue<MockNode>>(queue, 2);
	queueTest<NKCore::TQueue<MockNode>>(queue, 3);

	popQueueTest<NKCore::TQueue<MockNode>>(queue, 1);
	popQueueTest<NKCore::TQueue<MockNode>>(queue, 2);
	popQueueTest<NKCore::TQueue<MockNode>>(queue, 3);
	
	return true;
}

NKTEST(TLockFreeQueue_Test)
{
	NKCore::TLockFreeQueue<MockNode> queue;

	queueTest<NKCore::TLockFreeQueue<MockNode>>(queue, 1);
	queueTest<NKCore::TLockFreeQueue<MockNode>>(queue, 2);
	queueTest<NKCore::TLockFreeQueue<MockNode>>(queue, 3);

	popQueueTest<NKCore::TLockFreeQueue<MockNode>>(queue, 1);
	popQueueTest<NKCore::TLockFreeQueue<MockNode>>(queue, 2);
	popQueueTest<NKCore::TLockFreeQueue<MockNode>>(queue, 3);

	return true;
}

NKTEST(TLockQueue_Test)
{
	NKCore::TLockQueue<MockNode> queue;

	queueTest<NKCore::TLockQueue<MockNode>>(queue, 1);
	queueTest<NKCore::TLockQueue<MockNode>>(queue, 2);
	queueTest<NKCore::TLockQueue<MockNode>>(queue, 3);

	popQueueTest<NKCore::TLockQueue<MockNode>>(queue, 1);
	popQueueTest<NKCore::TLockQueue<MockNode>>(queue, 2);
	popQueueTest<NKCore::TLockQueue<MockNode>>(queue, 3);

	return true;
}

template<typename T>
void sortedListInsertTest(NKCore::TSortedList<T>& sorted_list, int count)
{
	_ASSERT(count != 0);

	// insert
	for (int i = 0; i < count; ++i)
	{
		MockNode* mock = new MockNode(i);
		mock->_v = 10 * i;
		_ASSERT(sorted_list.insert(mock) == true);
	}

	// pick
	{
		MockNode* trace = sorted_list.pick();
		_ASSERT(trace != nullptr);
		_ASSERT(trace->getPrev() == nullptr);

		int j = 0;
		while (trace != nullptr)
		{
			_ASSERT(trace != nullptr);
			_ASSERT(trace->_v == 10 * j++);
			trace = trace->getNext();
		}
		_ASSERT(j == count);
	}

	// pop
	for (int i = 0; i < count; ++i)
	{
		MockNode* mock = sorted_list.pop();
		_ASSERT(mock != nullptr);
		_ASSERT(mock->_v == 10 * i);
		_ASSERT(mock->getNext() == nullptr);
		_ASSERT(mock->getPrev() == nullptr);
		SAFE_DELETE(mock);
	}
}

template<typename T>
void sortedListPopTest(NKCore::TSortedList<T>& sorted_list, int count)
{
	_ASSERT(count != 0);

	// insert
	for (int i = 0; i < count; ++i)
	{
		MockNode* mock = new MockNode(i);
		mock->_v = 10 * i;
		_ASSERT(sorted_list.insert(mock) == true);
	}
	
	// pop by key
	int j = 0;
	{
		int64_t pop_key = count / 2;
		MockNode* trace = sorted_list.pop(pop_key);
		_ASSERT(trace != nullptr);
		_ASSERT(trace->getPrev() == nullptr);

		MockNode* pDelete = nullptr;
		while (trace != nullptr)
		{
			_ASSERT(trace != nullptr);
			_ASSERT(trace->_v == 10 * j++);
			_ASSERT(trace->getKey() <= pop_key);

			pDelete = trace;
			trace = trace->getNext();
			SAFE_DELETE(pDelete);
		}
		if (j == count) return;
	}

	// pick
	{
		int k = j;
		MockNode* trace = sorted_list.pick();
		_ASSERT(trace != nullptr);

		while (trace != nullptr)
		{
			_ASSERT(trace != nullptr);
			_ASSERT(trace->_v == 10 * k++);
			
			trace = trace->getNext();
		}
	}

	// pop
	{
		MockNode* trace = sorted_list.pop();
		_ASSERT(trace != nullptr);
		while (trace != nullptr)
		{
			_ASSERT(trace != nullptr);
			_ASSERT(trace->_v == 10 * j++);

			SAFE_DELETE(trace);

			trace = sorted_list.pop();
		}
		_ASSERT(j == count);
	}
}

template<typename T>
void sortedListEraseTest(NKCore::TSortedList<T>& sorted_list, int count)
{;
	_ASSERT(count != 0);

	// insert
	MockNodeVector list;

	for (int i = 0; i < count; ++i)
	{
		std::unique_ptr<MockNode> mock = std::make_unique<MockNode>(i);
		mock->_v = 10 * i;
		_ASSERT(sorted_list.insert(mock.get()) == true);

		list.push_back(std::move(mock));
	}

	// shuffle
	std::random_shuffle(list.begin(), list.end());

	// erase
	for_each(list.begin(), list.end(),
		[&sorted_list](std::unique_ptr<MockNode>& mock)
		{
			_ASSERT(mock != nullptr);
			MockNode* pMock = mock.get();
			_ASSERT(pMock != nullptr);
			_ASSERT(sorted_list.erase(pMock) == true);
		}
	);
}

NKTEST(TSortedList_Test)
{
	NKCore::TSortedList<MockNode> sorted_list;

	sortedListInsertTest(sorted_list, 1);
	sortedListInsertTest(sorted_list, 2);
	sortedListInsertTest(sorted_list, 3);

	sortedListPopTest(sorted_list, 1);
	sortedListPopTest(sorted_list, 2);
	sortedListPopTest(sorted_list, 3);
	sortedListPopTest(sorted_list, 10);

	sortedListEraseTest(sorted_list, 1);
	sortedListEraseTest(sorted_list, 2);
	sortedListEraseTest(sorted_list, 3);
	sortedListEraseTest(sorted_list, 10);

	return true;
}

void indexedQueueTest(int size_index, int node_count)
{
	_ASSERT(size_index != 0);
	_ASSERT(node_count != 0);

	std::unique_ptr<byte[]> _counts_index = std::make_unique<byte[]>(size_index);
	NKCore::TIndexedQueue<MockNode> indexed_queue(size_index);

	// push
	for (int i = 0; i < node_count; ++i)
	{
		int param = i + 1;
		MockNode* mock = new MockNode(param);
		mock->_v = 10 * (param);
		int index = rand() % size_index;
		_ASSERT(indexed_queue.push(index, mock) == true);
		_counts_index[index]++;
	}

	// pick queue
	{
		for (int i = 0; i < size_index; i++)
		{
			int count = 0;
			if (_counts_index[i] > 0)
			{
				MockNode* trace = indexed_queue.pickQueue(i);
				_ASSERT(trace != nullptr);
				while (trace != nullptr)
				{
					_ASSERT(trace != nullptr);
					count++;
					trace = trace->getNext();
				}
				_ASSERT(_counts_index[i] == count);
			}
		}
	}

	// pop queue
	{
		for (int i = 0; i < size_index; i++)
		{
			MockNode* trace = indexed_queue.popQueue(i);
			MockNode* pDelete = nullptr;
			if (_counts_index[i] > 0)
			{
				_ASSERT(trace != nullptr);
				while (trace != nullptr)
				{
					_ASSERT(trace != nullptr);

					pDelete = trace;
					trace = trace->getNext();
					SAFE_DELETE(pDelete);
				}
			}
		}
	}
}

void indexedQueueEraseTest(int size_index, int node_count)
{
	_ASSERT(size_index != 0);
	_ASSERT(node_count != 0);

	std::unique_ptr<MockNodeVector[]> _list_array = std::make_unique<MockNodeVector[]>(size_index);
	NKCore::TIndexedQueue<MockNode> indexed_queue(size_index);

	// push
	for (int i = 0; i < node_count; ++i)
	{
		int param = i + 1;
		std::unique_ptr<MockNode> mock = std::make_unique<MockNode>(param);
		mock->_v = 10 * (param);
		int index = rand() % size_index;
		_ASSERT(indexed_queue.push(index, mock.get()) == true);
		_list_array[index].push_back(std::move(mock));
	}
	
	// shuffle
	for (int i = 0; i < size_index; ++i)
	{
		std::random_shuffle(_list_array[i].begin(), _list_array[i].end());
	}

	// erase
	for (int i = 0; i < size_index; ++i)
	{
		for_each(_list_array[i].begin(), _list_array[i].end(),
			[&indexed_queue,&i](std::unique_ptr<MockNode>& mock)
			{
				_ASSERT(mock != nullptr);
				MockNode* pMock = mock.get();
				_ASSERT(pMock != nullptr);
				_ASSERT(indexed_queue.erase(i,pMock) == true);
			}
		);
	}
}

NKTEST(TIndexedQueue_Test)
{
	for (int i = 1; i < 10; i++)
	{
		for (int j = 1; j < 10; j++)
		{
			indexedQueueTest(i, j);
			indexedQueueEraseTest(i, j);
		}
	}

	return true;
}

NKTEST_BENCHMARK(TQueue_Benchmark)
{
	{
		NKUnitTest::ElapsedTime elapsed_time(L"queue");
		NKCore::TQueue<MockNode> queue;
		queueTest<NKCore::TQueue<MockNode>>(queue, 100000);
	}

	{
		NKUnitTest::ElapsedTime elapsed_time(L"lock-free queue");
		NKCore::TLockFreeQueue<MockNode> queue;
		queueTest<NKCore::TLockFreeQueue<MockNode>>(queue, 100000);
	}

	{
		NKUnitTest::ElapsedTime elapsed_time(L"lock queue");
		NKCore::TLockQueue<MockNode> queue;
		queueTest<NKCore::TLockQueue<MockNode>>(queue, 100000);
	}

	{
		const int thread_count = 10;
		NKUnitTest::thread_test(thread_count,
			[]()
		{
			NKUnitTest::ElapsedTime elapsed_time(L"lock-free queue");
			NKCore::TLockFreeQueue<MockNode> queue;
			queueTest<NKCore::TLockFreeQueue<MockNode>>(queue, 100000);
		}
		);
	}

	{
		const int thread_count = 10;
		NKUnitTest::thread_test(thread_count,
			[]()
		{
			NKUnitTest::ElapsedTime elapsed_time(L"lock queue");
			NKCore::TLockQueue<MockNode> queue;
			queueTest<NKCore::TLockQueue<MockNode>>(queue, 100000);
		}
		);
	}

	return true;
}

#endif // __NKCOREDATASTRUCTURETEST_HEADER__