#include "Log.h"
#include "LogDevice.h"
#include "LogBuilder.h"
#include <algorithm>

using namespace NKLog;
using namespace std;

Log::Log(void)
	:_turnon(true)
{
}

Log::~Log(void)
{
}

bool Log::registerLogDevice(const std::shared_ptr<LogDevice>& logDevice)
{
	if (logDevice == nullptr)
	{
		_ASSERT(0);
		return false;
	}

	std::lock_guard<std::mutex> _lock(__mutex_logDeviceList);
	_logDeviceList.push_back(logDevice);
	return true;
}

bool Log::write(const LogLayout& layout, const LogCategory& category, const NKWString& log)
{
	if (log.empty() == true) return false;
	if ( _turnon == false) return false;
	if (emptyDeviceList() == true) return false;
		
	return writeDeviceDetails(layout, category, log);
}

bool Log::writeDeviceDetails(const LogLayout& layout, const LogCategory& category, const NKWString& log)
{
	if (log.empty() == true) return false;
	if (_turnon == false) return false;

	std::lock_guard<std::mutex> _lock(__mutex_logDeviceList);

	for_each(_logDeviceList.begin(), _logDeviceList.end(),
		[&layout, &category, &log]
		(const std::shared_ptr<LogDevice>& logDevice)
		{
			logDevice->write(layout, category, log);
		}
	);

	return true;
}

void Log::clearLogDevices(void)
{
	std::lock_guard<std::mutex> _lock(__mutex_logDeviceList);
	_logDeviceList.clear();
}