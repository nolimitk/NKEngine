#pragma once
#ifndef __NKLOGGER_HEADER__
#define __NKLOGGER_HEADER__
// @nolimitk
// 16.04.23
// logger

#include "LogLayout.h"
#include "LogCategory.h"
#include "AsyncLog.h"

namespace NKLog
{
	class NKLogger
	{
	public:
		enum LAYOUT
		{
			LAYOUT_ERROR,
			LAYOUT_WARNING,
			LAYOUT_INFO,
			LAYOUT_MAX
		};

	public:
		bool write(LAYOUT layout, const wchar_t* file, int line, const wchar_t* format, ...);
		bool write(LAYOUT layout, const wchar_t* format, ...);

	protected:
		const NKLog::LogLayout _layout[LAYOUT_MAX];
		const NKLog::LogCategory _category;
		
	public:
		NKLogger(void);
		NKLogger(const NKWString& name);
		virtual ~NKLogger(void);
	};
}

#endif // __NKLOGGER_HEADER__