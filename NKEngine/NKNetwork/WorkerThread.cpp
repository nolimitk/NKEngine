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
	, _update_tick(0)
{
}

WorkerThread::~WorkerThread(void)
{
}

bool WorkerThread::onStart(void)
{
	_update_tick = _clock.getElapsedMicroSec();
	NKENGINELOG_INFO( L"[WORKERTHREAD],%u,started", getThreadID() );
	return true;
}

bool WorkerThread::onRun(void)
{
	// event OnUpdate
	uint64_t current_tick = _clock.getElapsedMicroSec();
	int64_t update_elapsed_time = current_tick - _update_tick;
	DWORD sleep_gap = 0;
	
	if (update_elapsed_time >= 0)
	{
		onUpdate(current_tick);
		_update_tick = current_tick + UPDATE_UNIT;
		sleep_gap = UPDATE_UNIT / 1000;
	}
	else
	{
		sleep_gap = static_cast<DWORD>((_update_tick - current_tick) / 1000);
		//NKENGINELOG_INFO(L"[WORKERTHREAD],%u,idle,sleep %d, %llu - %llu", getThreadID(), sleep_gap, _update_tick, current_tick);
	}
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
			NKENGINELOG_INFO( L"[WORKERTHREAD],%u,end event", getThreadID());
			keep_run = false;
		}
		else if (event_context == nullptr)
		{
			NKENGINELOG_ERROR( L"[WORKERTHREAD],%u,[CRITICAL] event context is null", getThreadID() );
			_ASSERT(0);
		}
		else
		{
			shared_ptr<EventObject> event_object = event_context->_event_object;
			if (event_object == nullptr)
			{
				NKENGINELOG_ERROR(L"[WORKERTHREAD],%u,[CRITICAL] event object is null", getThreadID());
				_ASSERT(0);
			}
			event_object->onProcess(*event_context,transferred);
		}
	}
	else
	{
		if (event_context == nullptr)
		{
			// GQCS timeout, worker thread debugging log, do not remove
			//NKENGINELOG_INFO(L"[WORKERTHREAD],%u,timeout,sleep %u", getThreadID(), sleep_gap);
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
	
	return keep_run;
}

bool WorkerThread::onEnd(void)
{
	NKENGINELOG_INFO(L"[WORKERTHREAD],%u,exit", getThreadID());
	return true;
}

bool WorkerThread::onUpdate(int64_t delta)
{
	NKENGINELOG_INFO(L"[WORKERTHREAD],%u,update,delta %lldus", getThreadID(), delta);
	return true;
}