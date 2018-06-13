#include "AsyncLog.h"
#include "LogBuilder.h"

using namespace std;
using namespace NKLog;

const std::chrono::milliseconds LogThread::DEFAULT_GAP_LOGWRITE(250);

bool LogThread::onRun(void)
{
	LogDataSP log_data = AsyncLogSingleton::getInstance()->popLogdataQueue();
	
	if (log_data == false)
	{
		this_thread::sleep_for(DEFAULT_GAP_LOGWRITE);
		return true;
	}

	while (log_data != nullptr)
	{
		AsyncLogSingleton::getInstance()->writeDeviceDetails(log_data->_log_layout, log_data->_log_category, log_data->_log);

		log_data = log_data->getNext();
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

LogDataSP NKLog::AsyncLog::popLogdataQueue(void)
{
	return _logdata_queue.popQueue();
}

bool NKLog::AsyncLog::write(const LogLayout & layout, const LogCategory & category, const NKWString & log)
{
	LogDataSP log_data = make_shared<LogData>(layout, category, log);
	
	write(log_data);
	return true;
}

bool NKLog::AsyncLog::write(const LogDataSP& log_data)
{
	if (log_data == false) return false;

	_logdata_queue.push(log_data);

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