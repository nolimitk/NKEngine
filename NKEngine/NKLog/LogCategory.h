#pragma once
#ifndef __LOGCATEGORY_HEADER__
#define __LOGCATEGORY_HEADER__
// @nolimitk
// 16.04.10
// log category

#include <string>
#include "../NKCore.h"

namespace NKLog
{
	class LogCategory
	{
	public:
		NKCore::UniqueID _id;
		NKWString _name;

	public:
		LogCategory(const NKWString& name)
			:_name(name)
		{
		}
	};
}

#endif // __LOGCATEGORY_HEADER__
