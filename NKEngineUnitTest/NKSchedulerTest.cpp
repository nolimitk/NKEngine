#pragma once

#ifndef __NKSCHEDULERTEST_HEADER__
#define __NKSCHEDULERTEST_HEADER__
// @nolimitk
// 16.05.28
// scheduler test

#include "NKEngine.h"
#include "NKUnittest.h"
#include "NKUnittestLog.h"

using namespace std;

NKTEST(EventSlot_Test)
{
	_ASSERT(NKNetwork::IOCPManager::getInstance()->create() == true);

	class MockSlot : public NKScheduler::EventSlot
	{
		virtual bool onProcess(NKNetwork::EventContext& event_context, uint32_t transferred) override
		{
			NKUNITTESTLOG_INFO(L"%s", __FUNCTIONW__);
			return true;
		}
	};

	shared_ptr<MockSlot> test_event = make_shared<MockSlot>();

	_ASSERT(NKNetwork::IOCPManager::getInstance()->postEvent(dynamic_pointer_cast<NKNetwork::EventObject>(test_event), 0) == true);

	NKNetwork::IOCPManager::getInstance()->close();

	return true;
}

#endif // __NKSCHEDULERTEST_HEADER__
