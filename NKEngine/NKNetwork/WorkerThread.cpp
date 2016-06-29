#include "WorkerThread.h"
#include "../NKEngineLog.h"
#include "IOCPManager.h"
#include "Event.h"
#include "EventContext.h"

using namespace NKNetwork;
using namespace std;

//TimerNode::TimerNode(void)
//	:_reserved(false)
//{
//}
//
//TimerNode::~TimerNode(void)
//{
//}
//
//bool TimerNode::tryReserve(void)
//{
//	bool init = _reserved.exchange(true);
//	if (init == true) return false;
//	return true;
//}
//
//bool TimerNode::releaseReserve(void)
//{
//	bool init = _reserved.exchange(false);
//	if (init == false) return false;
//	return true;
//}
//
//NKTimer::NKTimer(void)
//	:NKTimer(DEFAULT_SHORTTERM_SIZE)
//{
//
//}
//
//NKTimer::NKTimer(int size)
//	: _shortTermJob(size)
//{
//
//}
//
//NKTimer::~NKTimer(void)
//{
//}
//
//bool NKTimer::onExpired(void)
//{
//	return true;
//}

WorkerThread::WorkerThread(const IOCPManager& iocp_manager)
	:_completion_port(iocp_manager.getCompletionPort())
{
}

WorkerThread::~WorkerThread(void)
{
}

bool WorkerThread::onStart(void)
{
	_last_tick = _clock.getElapsedMicroSec();
	NKENGINELOG_INFO( L"worker thread started,thread %u", getThreadID() );
	return true;
}

bool WorkerThread::onRun(void)
{
	// event OnUpdate
	uint64_t current_tick = _clock.getElapsedMicroSec();

	NKENGINELOG_INFO(L"worker thread wait,thread %u, time %I64d", getThreadID(), current_tick);
	onUpdate(current_tick);
	int64_t update_elapsed_time = _clock.getElapsedMicroSec() - current_tick;
	if (update_elapsed_time > UPDATE_UNIT)
	{
		NKENGINELOG_ERROR(L"worker thread update, it takes too much times, thread %u, time %I64d", getThreadID(), update_elapsed_time);
		_ASSERT(0);
		update_elapsed_time = UPDATE_UNIT;
	}
	else
	{
		NKENGINELOG_INFO(L"worker thread update, thread %u, time %I64dus", getThreadID(), update_elapsed_time);
	}
	
	DWORD sleep_gap = static_cast<DWORD>((UPDATE_UNIT - update_elapsed_time) / 1000);
	///

	DWORD transferred = 0;
	PULONG_PTR completion_key = nullptr;
	EventContext* pEventContext = nullptr;
			
	BOOL ret = GetQueuedCompletionStatus( _completion_port, &transferred, (PULONG_PTR)&completion_key, (LPOVERLAPPED *)&pEventContext, sleep_gap);

	unique_ptr<EventContext> event_context(pEventContext);
	bool keep_run = true;
	// @TODO event process count와 process time을 기록해서 monitoring을 제공한다.
		
	if (ret == TRUE)
	{
		if (completion_key == (PULONG_PTR)IOCPManager::COMPLETION_KEY::END_EVENT)
		{
			NKENGINELOG_INFO( L"worker thread end event,thread %u", getThreadID());
			keep_run = false;
		}
		else if (event_context == nullptr)
		{
			NKENGINELOG_ERROR( L"[CRITICAL],event context is null, thread %u", getThreadID() );
			_ASSERT(0);
		}
		else
		{
			shared_ptr<EventObject> event_object = event_context->_event_object;
			if (event_object == nullptr)
			{
				NKENGINELOG_ERROR(L"[CRITICAL],event object is null, thread %u", getThreadID());
				_ASSERT(0);
			}
			event_object->onProcess(*event_context,transferred);
		}
	}
	else
	{
		if (event_context == nullptr)
		{
			NKENGINELOG_SOCKETERROR( GetLastError(), L"process failed with no event context,thread %u", getThreadID());
			_ASSERT(0);
		}
		else
		{
			shared_ptr<EventObject> event_object = event_context->_event_object;
			if (event_object == nullptr)
			{
				NKENGINELOG_ERROR(L"[CRITICAL],event object is null, thread %u", getThreadID());
				_ASSERT(0);
			}
			event_object->onProcessFailed(*event_context,transferred);
		}
	}

	int64_t last_tick = _clock.getElapsedMicroSec() - current_tick;
	
	return keep_run;
}

bool WorkerThread::onEnd(void)
{
	NKENGINELOG_INFO(L"worker thread to exit,thread %u", getThreadID());
	return true;
}

bool WorkerThread::onUpdate(int64_t delta)
{
	NKENGINELOG_INFO(L"worker thread to update,thread %u, delta %d", getThreadID(), delta);
	return true;
}