#pragma once
#ifndef __LOGLAYOUT_HEADER__
#define __LOGLAYOUT_HEADER__
// @nolimitk
// 16.04.06
// log layout

#include <string>
#include "../NKCore.h"

namespace NKLog
{
	enum class LogColor
	{
		WHITE,
		RED,
		YELLOW,
	};

	class LogLayout
	{
	public:
		NKCore::UniqueID _id;
		NKWString _name;
		LogColor _color;

	public:
		LogLayout(NKWString& name, LogColor color)
			:_name(name)
			,_color(color)
		{
		}

		LogLayout(NKWString&& name, LogColor color)
			:_name(name)
			,_color(color)
		{
		}
	};
}

#endif // __LOGDEVICE_HEADER__
