#include "Log.h"
#include "LogDevice.h"
#include "LogBuilder.h"
#include <algorithm>

using namespace NKLog;
using namespace std;

Log::Log(void)
	:_turnon(true)
	,_builder(make_unique<BaseLogBuilder>())
{
}

Log::~Log(void)
{
}

bool Log::registerLogDevice(std::unique_ptr<LogDevice>&& logDevice)
{
	if (logDevice == nullptr) return false;
	logDevice->setBuilder(nullptr);
	_logDeviceQueue.push_back(std::move(logDevice));
	return true;
}

bool Log::write(const LogLayout& layout, const LogCategory& category, const NKWString& log)
{
	if (log.empty() == true) return false;
	if ( _turnon == false) return false;
	if (_logDeviceQueue.empty() == true) return false;

	NKWString build_log = (_builder != nullptr) ? _builder->publish(layout, category, log) : log;
	
	return writeDeviceDetails(layout, category, build_log);
}

bool Log::writeDeviceDetails(const LogLayout& layout, const LogCategory& category, const NKWString& log)
{
	if (log.empty() == true) return false;
	if (_turnon == false) return false;
	if (_logDeviceQueue.empty() == true) return false;

	for_each(_logDeviceQueue.begin(), _logDeviceQueue.end(),
		[&layout, &category, &log]
		(std::unique_ptr<LogDevice>& logDevice)
		{
			logDevice->write(layout, category, log);
		}
	);

	return true;
}

void Log::clearLogDevices(void)
{
	_logDeviceQueue.clear();
}