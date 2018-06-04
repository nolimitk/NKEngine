#include "AsyncLog.h"
#include "LogBuilder.h"

using namespace std;
using namespace NKLog;

const std::chrono::milliseconds LogThread::DEFAULT_GAP_LOGWRITE(250);

bool LogThread::onRun(void)
{
	LogData* pLogData = AsyncLogSingleton::getInstance()->getLogdataQueue();
	LogData *pDelete = nullptr;

	if (pLogData == nullptr)
	{
		this_thread::sleep_for(DEFAULT_GAP_LOGWRITE);
		return true;
	}

	while (pLogData != nullptr)
	{
		AsyncLogSingleton::getInstance()->writeDeviceDetails(pLogData->_log_layout, pLogData->_log_category, pLogData->_log);

		pDelete = pLogData;
		pLogData = pLogData->getNext();
		SAFE_DELETE(pDelete);
	}

	this_thread::yield();
		
	return true;
}

AsyncLog::AsyncLog(void)
	:_start_thread(false)
{
}


AsyncLog::~AsyncLog(void)
{
	_log_thread.join();
	flush();
}

LogData* NKLog::AsyncLog::getLogdataQueue(void)
{
	return _logdata_queue.popQueue();
}

bool NKLog::AsyncLog::write(const LogLayout & layout, const LogCategory & category, const NKWString & log)
{
	LogData* pLogData = new LogData(layout, category, log);
	
	write(pLogData);
	return true;
}

bool NKLog::AsyncLog::write(LogData* pLogData)
{
	if (pLogData == nullptr) return false;

	_logdata_queue.push(pLogData);

	bool expected = false;
	bool ret = _start_thread.compare_exchange_strong(expected, true);
	if (ret == true)
	{
		_log_thread.create();
	}
	return true;
}

bool AsyncLog::flush(void)
{
	return _log_thread.onRun();
}