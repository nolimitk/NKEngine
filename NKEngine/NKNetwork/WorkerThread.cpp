#include "WorkerThread.h"
#include "../NKEngineLog.h"
#include "IOCPManager.h"
#include "Event.h"
#include "EventContext.h"
#include "NetworkCallbacks.h"

using namespace NKNetwork;
using namespace std;
using namespace std::chrono;

const microseconds WorkerThread::UPDATE_UNIT = 50000us;

WorkerThread::WorkerThread(const HANDLE completion_portr)
	: _completion_port(completion_portr)
	, _next_update_time(milliseconds::zero())
{
}

WorkerThread::~WorkerThread(void)
{
}

bool WorkerThread::onStart(void)
{
	timeBeginPeriod(1);

	_next_update_time = _clock.getElapsedTime();
	NKENGINELOG_INFO( L"[WORKERTHREAD],%u,started", getThreadID());
	return true;
}

bool WorkerThread::onRun(void)
{
	// event OnUpdate
	DWORD sleep_duration = 0;
	{
		/*	previous	0
		current		0
		elapsed		current - previous
		next		current + 50
		sleep		next - current	*/
		microseconds current_time = _clock.getElapsedTime();
		microseconds previous_time = _next_update_time - UPDATE_UNIT;
		microseconds elapsed_time = current_time - previous_time;

		//@TODO should apply marin of error
		//@TODO handle delay...?
		if (current_time >= _next_update_time)
		{
			onUpdate(current_time.count());
			_next_update_time = current_time + UPDATE_UNIT;
		}

		sleep_duration = static_cast<DWORD>(duration_cast<milliseconds>(_next_update_time - current_time).count());

		//@TODO need log level
		/*NKENGINELOG_INFO(L"[WORKERTHREAD],%u,update stamp, previous %lld, current %lld, elapsed %lld, next %lld, sleep %u",
			getThreadID(),
			previous_time.count(), current_time.count(), elapsed_time.count(),
			_next_update_time.count(), sleep_duration);*/
	}
	///

	DWORD transferred = 0;
	PULONG_PTR completion_key = nullptr;
	EventContext* pEventContext = nullptr;
	
	BOOL ret = GetQueuedCompletionStatus( _completion_port, &transferred, (PULONG_PTR)&completion_key, (LPOVERLAPPED *)&pEventContext, sleep_duration);

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
	timeEndPeriod(1);

	NKENGINELOG_INFO(L"[WORKERTHREAD],%u,exit", getThreadID());
	return true;
}

bool WorkerThread::onUpdate(int64_t delta)
{
	// TODO : make a log level
	//NKENGINELOG_INFO(L"[WORKERTHREAD],%u,update,delta %lldus", getThreadID(), delta);

	for (auto iter : _callback_list)
	{
		iter->onUpdate(delta);
	}

	return true;
}

void WorkerThread::registerCallback(const std::shared_ptr<WorkerCallback>& callback)
{
	_callback_list.push_back(callback);
}
