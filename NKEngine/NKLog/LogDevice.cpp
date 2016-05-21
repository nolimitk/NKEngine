#include "LogDevice.h"

#include "LogBuilder.h"
#include "LogLayout.h"
#include "LogCategory.h"

using namespace NKLog;
using namespace std;

LogDevice::LogDevice(void)
	:_builder(nullptr)
{
}

LogDevice::~LogDevice(void)
{
}

bool LogDevice::replaceInvalid(wchar_t *pBuffer)
{
	if (pBuffer == nullptr) return false;

	for( int i = 0 ; pBuffer[i] != NULL ; i++ )
	{
		if( pBuffer[i] == 0xFFFD )
		{
			pBuffer[i] = 0;
			return true;
		}
	}
	return false;
}

bool LogDevice::writeSafe(const LogLayout& layout, const LogCategory& category, const NKWString& log)
{
	lock_guard<mutex> lock(_mutex);
	return write(layout, category, log);
}

bool LogDevice::write(const LogLayout& layout, const LogCategory& category, const NKWString& log)
{
	if (log.empty() == true)
	{
		return false;
	}

	if (_layout_set.isRegisteredID(layout._id) == false)
	{
		return false;
	}

	if (_category_set.isRegisteredID(category._id) == false)
	{
		return false;
	}

	NKWString build_log = (_builder != nullptr) ? _builder->publish(layout, category, log) : log;

	return onWrite(layout, category, build_log);
}