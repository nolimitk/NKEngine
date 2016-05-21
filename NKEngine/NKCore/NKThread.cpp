#include "NKThread.h"
#include <Windows.h>
#include <time.h>
#include <locale>

using namespace NKCore;
using namespace std;

NKThread::NKThread(void)
	: _run(false)
{
}

NKThread::~NKThread(void)
{
}

bool NKThread::create(void)
{
	_run = true;
	_t = thread(NKThread::run,this);
	return true;
}

bool NKThread::join(void)
{
	_run = false;
	if (_t.joinable() == false)
	{
		return false;
	}
	_t.join();
	return true;
}

bool NKThread::join(std::chrono::milliseconds tick)
{
	_run = false;
	if (_t.joinable() == false)
	{
		return true;
	}
	
	DWORD ret = WaitForSingleObjectEx(_t.native_handle(), (DWORD)tick.count(), FALSE);
	switch (ret)
	{
	case WAIT_OBJECT_0:
	{
		_t.detach();
		return true;
	}
	break;
	case WAIT_TIMEOUT:
	{
		return false;
	}
	break;
	}
	return true;
}

bool NKThread::run(void *pParam)
{
	NKThread *pThread = static_cast<NKThread*>(pParam);

	// srand must be called by each thread.
	srand( (uint32_t)time(nullptr) );
	pThread->_id = this_thread::get_id();

	if (pThread->onStart() == false)
	{
		return false;
	}

	while( pThread->_run == true )
	{
		if (pThread->onRun() == false)
		{
			break;
		}
	}

	if (pThread->onEnd() == false)
	{
		return false;
	}

	return 0;
}