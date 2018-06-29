#include "WorkerThread.h"
#include "../NKEngineLog.h"
#include "IOCPManager.h"
#include "Event.h"
#include "EventContext.h"

using namespace NKNetwork;
using namespace std;

const std::chrono::microseconds WorkerThread::UPDATE_UNIT = 50000us;

WorkerThread::WorkerThread(const HANDLE completion_portr)
	:_completion_port(completion_portr)
	, _update_tick(0)
{
}

WorkerThread::~WorkerThread(void)
{
}

bool WorkerThread::onStart(void)
{
	_update_tick = _clock.getElapsedTime();
	NKENGINELOG_INFO( L"[WORKERTHREAD],%u,started", getThreadID() );
	return true;
}

bool WorkerThread::onRun(void)
{
	// event OnUpdate
	std::chrono::microseconds current_tick = _clock.getElapsedTime();
	std::chrono::microseconds update_elapsed_time = current_tick - _update_tick;
	DWORD sleep_gap = 0;
	
	if (update_elapsed_time >= 0us)
	{
		onUpdate(current_tick.count());
		_update_tick = current_tick + UPDATE_UNIT;
		sleep_gap = static_cast<DWORD>((UPDATE_UNIT / 1000).count());
	}
	else
	{
		sleep_gap = static_cast<DWORD>((_update_tick - current_tick).count() / 1000);
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
			NKENGINELOG_ERROR_ASSERT( L"[WORKERTHREAD],%u,[CRITICAL] event context is null", getThreadID() );
		}
		else
		{
			shared_ptr<EventObject>& event_object = event_context->_event_object;
			if (event_object == nullptr)
			{
				NKENGINELOG_ERROR_ASSERT(L"[WORKERTHREAD],%u,[CRITICAL] event object is null", getThreadID());
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
			shared_ptr<EventObject>& event_object = event_context->_event_object;
			if (event_object == nullptr)
			{
				NKENGINELOG_ERROR_ASSERT(L"[WORKERTHREAD],%u,[CRITICAL] event object is null", getThreadID());
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
	// TODO : make a log level
	//NKENGINELOG_INFO(L"[WORKERTHREAD],%u,update,delta %lldus", getThreadID(), delta);
	return true;
}