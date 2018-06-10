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
	_ASSERT(NKNetwork::IOCPManager::getInstance()->create() == true);

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

	NKNetwork::IOCPManager::getInstance()->close();

	return true;
}
*/

NKTEST(Scheduler_Test)
{
	_ASSERT(NKNetwork::IOCPManager::getInstance()->create() == true);
	_ASSERT(NKScheduler::Scheduler::getInstance()->start() == true);
	_ASSERT(NKScheduler::Scheduler::getInstance()->stop() == true);

	NKScheduler::Scheduler::getInstance()->destroy();
	NKNetwork::IOCPManager::getInstance()->close();

	return true;
}