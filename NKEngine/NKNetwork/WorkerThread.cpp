#include "WorkerThread.h"
#include "../NKEngineLog.h"
#include "IOCPManager.h"
#include "Event.h"
#include "EventContext.h"

using namespace NKNetwork;
using namespace std;

WorkerThread::WorkerThread(const IOCPManager& iocp_manager)
	:_completion_port(iocp_manager.getCompletionPort())
{
}

WorkerThread::~WorkerThread(void)
{
}

bool WorkerThread::onStart(void)
{
	NKENGINELOG_INFO( L"worker thread started,thread %u", getThreadID() );
	return true;
}

bool WorkerThread::onRun(void)
{
	DWORD transferred = 0;
	PULONG_PTR completion_key = nullptr;
	EventContext* pEventContext = nullptr;
	
	NKENGINELOG_INFO( L"worker thread wait,thread %u", getThreadID() );

	BOOL ret = GetQueuedCompletionStatus( _completion_port, &transferred, (PULONG_PTR)&completion_key, (LPOVERLAPPED *)&pEventContext, INFINITE );

	unique_ptr<EventContext> event_context(pEventContext);
		
	// @TODO event process count와 process time을 기록해서 monitoring을 제공한다.
		
	if (ret == TRUE)
	{
		if (completion_key == (PULONG_PTR)IOCPManager::COMPLETION_KEY::END_EVENT)
		{
			NKENGINELOG_INFO( L"worker thread end event,thread %u", getThreadID());
			return false;
		}
		else if (event_context == nullptr)
		{
			NKENGINELOG_ERROR( L"[CRITICAL],event context is null, thread %u", getThreadID() );
			_ASSERT(0);
			return true;
		}
		else
		{
			shared_ptr<EventObject> event_object = event_context->_event_object;
			if (event_object == nullptr)
			{
				NKENGINELOG_ERROR(L"[CRITICAL],event object is null, thread %u", getThreadID());
				_ASSERT(0);
				return true;
			}
			event_object->onProcess(*event_context,transferred);
		}
	}
	else
	{
		if (event_context == nullptr)
		{
			// @TODO 이런 경우가 발생하는지 조사 후 대처한다.
			NKENGINELOG_SOCKETERROR( GetLastError(), L"process failed with no event context,thread %u", getThreadID());
		}
		else
		{
			shared_ptr<EventObject> event_object = event_context->_event_object;
			if (event_object == nullptr)
			{
				NKENGINELOG_ERROR(L"[CRITICAL],event object is null, thread %u", getThreadID());
				_ASSERT(0);
				return true;
			}
			event_object->onProcessFailed(*event_context,transferred);
		}
	}
	
	return true;
}

bool WorkerThread::onEnd(void)
{
	NKENGINELOG_INFO(L"worker thread to exit,thread %u", getThreadID());
	return true;
}