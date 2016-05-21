#pragma once
#ifndef __LOGBUILDER_HEADER__
#define __LOGBUILDER_HEADER__
// @nolimitk
// 16.04.10
// log builder

#include "../NKCore.h"

namespace NKLog
{
	class LogCategory;
	class LogLayout;

	class LogBuilder
	{
	public:
		virtual NKWString publish(const LogLayout& layout, const LogCategory& category, const NKWString& log) = 0;
	};

	class BaseLogBuilder final : public LogBuilder
	{
	public:
		NKWString publish(const LogLayout& layout, const LogCategory& category, const NKWString& log) override;
	};
}

#endif // __LOGBUILDER_HEADER__