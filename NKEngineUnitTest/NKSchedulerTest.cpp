// @nolimitk
// 16.05.28
// scheduler test

#include "NKEngine.h"
#include "NKUnittest.h"
#include "NKUnittestLog.h"

using namespace std;

/*
NKTEST(EventSlot_Test)
{
	class MockSlot : public NKScheduler::JobSlot
	{
	public:
		bool _onProcess;

		virtual bool onProcess(NKNetwork::EventContext& event_context, uint32_t transferred) override
		{
			_onProcess = true;
			return true;
		}

		MockSlot(void) :JobSlot(10),_onProcess(false) {}
	};

	shared_ptr<MockSlot> test_event = make_shared<MockSlot>();

	_ASSERT(NKNetwork::IOCPManager::getInstance()->postEvent(dynamic_pointer_cast<NKNetwork::EventObject>(test_event), 0) == true);
	WAITFOR(test_event, onProcess);

	return true;
}
*/

NKTEST(Scheduler_Test)
{
	{
		class MockNode2 : public NKCore::TNode2<MockNode2>
		{
		public:
			int _v;

		public:
			MockNode2(void) :_v(0) {}
			virtual ~MockNode2(void) { _v = 11; }
		};
		std::shared_ptr<MockNode2> node = make_shared<MockNode2>();
		std::shared_ptr<MockNode2> next_node = make_shared<MockNode2>();
		NKCore::TWaitFreeQueue<MockNode2> queue;

		queue.push(node);
		queue.push(next_node);
	}
	
	class MockJob : public NKScheduler::RealTimeJob
	{
	public:
		bool _onExecute;

	public:
		virtual bool onExecute(uint64_t execute_index)
		{
			_onExecute = true;
			return true;
		}

		MockJob(void) :_onExecute(false) {}
	};

	{
		NKScheduler::SerializerSP serializer = std::make_shared<NKScheduler::Serializer>();
		_ASSERT(serializer);
		shared_ptr<MockJob> job = std::make_shared<MockJob>();
		_ASSERT(job);

		uint64_t reserve_execution_index = NKScheduler::Scheduler::getInstance()->convertToExecutionIndex(50ms);
		_ASSERT(serializer->addJob(job, reserve_execution_index) == true);
		_ASSERT(NKScheduler::Scheduler::getInstance()->addSerializer(serializer, reserve_execution_index) == true);

		WAITFOR(job, onExecute);
	}

	{
		NKScheduler::SerializerSP serializer = std::make_shared<NKScheduler::Serializer>();
		_ASSERT(serializer);
		shared_ptr<MockJob> job = std::make_shared<MockJob>();
		_ASSERT(job);

		_ASSERT(NKScheduler::addJob(job, serializer, 50ms) == true);
		_ASSERT(NKScheduler::addJob(job, serializer, 50ms) == true);

		WAITFOR(job, onExecute);
	}

	return true;
}