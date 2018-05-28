#include "LogDeviceDetails.h"

#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <sstream>

#include "LogBuilder.h"
#include "LogLayout.h"
#include "LogCategory.h"

using namespace NKLog;
using namespace std;

void NKLog::errorHandling(const NKWString& name, std::wostream& out, const NKWString& log)
{
	wostringstream exception_log;
	wchar_t errMessage[1024];
	_wcserror_s(errMessage, errno);
	exception_log << name << L" exception, " << errMessage << "(" << errno << "), ";
	
	uint32_t i = 0;
	while (log[i] != NULL)
	{
		exception_log << hex << (int32_t)(log[i]) << L" ";
		i++;
	}
	exception_log << endl;
	
	out.clear();
	out << exception_log.str();
}

ConsoleLog::ConsoleLog(void)
{
}

ConsoleLog::~ConsoleLog(void)
{
}

bool ConsoleLog::onWrite(const LogLayout& layout, const LogCategory& category, const NKWString& log)
{
	switch(layout._color )
	{
	case LogColor::RED:
		SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED );
		break;
	case LogColor::YELLOW:
		SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN );
		break;
	case LogColor::WHITE:
		SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
		break;
	default:
		break;
	}
		
	// It doesn't occurs exception in operator << [2014/10/22/ nolimitk]
	// it would be crash than occurs exception.
	// when it be crash, it doen't remain report and dump. [2014/10/16/ nolimitk]
	wcout << log << endl;
	if (wcout.fail() == true)
	{
		errorHandling(L"console log", wcout, log);
	}

	if (layout._color != LogColor::WHITE)
	{
		SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE );
	}
	
	return true;
}

FileLog::FileLog(const NKWString& filename)
{
	wostringstream ext_filename;
	ext_filename << filename << L".log";

	_fpLog.open(ext_filename.str(), ios::out | ios::app);
}

FileLog::~FileLog(void)
{
	close();
}

void FileLog::close(void)
{
	if (_fpLog.is_open() == true)
	{
		_fpLog.flush();
		_fpLog.close();
	}
}

bool FileLog::onWrite(const LogLayout& layout, const LogCategory& category, const NKWString& log)
{
	if (_fpLog.is_open() == false)
	{
		return false;
	}

	_fpLog << log << endl;
	if (_fpLog.fail() == true)
	{
		errorHandling(L"file log", _fpLog, log);
	}

	// It doesn't occurs exception in operator << [2014/10/22/ nolimitk]
	// it would be crash than occurs exception.
	// when it be crash, it doen't remain report and dump. [2014/10/16/ nolimitk]
	/*
	try
	{
	_fpLog << pBuffer << endl;
	}
	catch( wofstream::failure const &ex )
	{
	_fpLog.clear();

	_fpLog << "file log exception, ";
	uint i = 0;
	while( pBuffer[i] != NULL )
	{
	_fpLog << hex << (byte)pBuffer[i];
	i++;
	}
	_fpLog << endl;
	_fpLog << ex.what() << endl;
	}
	*/

	return true;
}

DailyFileLog::DailyFileLog(const NKWString& filename)
	:DailyFileLog(L"", filename)
{
}

NKLog::DailyFileLog::DailyFileLog(const NKWString & rootname, const NKWString & filename)
	:_root_directory(rootname)
	,_filename(filename)
	,FileLog()
{
	if (create(_root_directory, _filename) == false)
	{
		// TODO throw error
	}
}

DailyFileLog::~DailyFileLog(void)
{
	close();
}

bool DailyFileLog::create(const NKWString& rootname, const NKWString& filename)
{
	if (rootname.empty() == false)
	{
		if (CreateDirectory(rootname.c_str(), NULL) == FALSE)
		{
			return false;
		}
	}

	// @TODO feature to create a file per hour
	time_t today;
	wchar_t date[9];
	wostringstream date_filename;
	
	time( &today );
	localtime_s( &_last_creation_time, &today );

	wcsftime( date, sizeof(date), L"%y%m%d", &_last_creation_time);
	if (rootname.empty() == false)
	{
		date_filename << L"./" << rootname << L"/";
	}
	date_filename << _filename << L"_" << date << L".log";

	_fpLog.open( date_filename.str(), ios::out | ios::app );
	if( _fpLog.fail() == true )
	{
		return false;
	}

	return true;
}

bool DailyFileLog::onWrite(const LogLayout & layout, const LogCategory & category, const NKWString & log)
{
	// if date has changed, create new file.
	time_t currentTime;
	struct tm structTime;

	time(&currentTime);
	localtime_s(&structTime, &currentTime);

	if (_last_creation_time.tm_yday != structTime.tm_yday)
	{
		close();
		create(_root_directory, _filename);
	}
	///

	FileLog::onWrite(layout, category, log);

	return true;
}
