#pragma once
#ifndef __NKUNITTESTUTILS_HEADER__
#define __NKUNITTESTUTILS_HEADER__
// @nolimitk
// 16.05.18
// unit test utils

#include "NKEngine.h"

namespace NKUnitTest
{
	class ElapsedTime
	{
	protected:
		NKCore::NKClock _clock;
		const NKWString _name;

	public:
		ElapsedTime(NKWString&& name);
		virtual ~ElapsedTime(void);
	};
}

#endif // __NKUNITTESTUTILS_HEADER__
