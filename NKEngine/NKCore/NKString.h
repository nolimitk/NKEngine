#pragma once
#ifndef __NKSTRING_HEADER__
#define __NKSTRING_HEADER__
// @nolimitk
// 16.05.10
// string

#include "NKMemory.h"
#include <string>

namespace NKCore
{
	template<typename T>
	class NKBasicString : public std::basic_string<T>
	{
	public:
		NKBasicString(void)
			:std::basic_string<T>()
		{
		}

		NKBasicString(const std::basic_string<T>& str)
			:std::basic_string<T>(str)
		{
		}

		NKBasicString(const T* ptr)
		{
			if (ptr == nullptr) { return; }
			assign(ptr);
		}

		NKBasicString(size_t size, T ch)
		{
			assign(size, ch);
		}
	};
}

typedef NKCore::NKBasicString<wchar_t> NKWString;
typedef NKCore::NKBasicString<char> NKString;

NKWString MultibyteToUnicode(const NKString& source);
NKString UnicodeToMultibyte(const NKWString& source);
NKWString UTF8ToUnicode(const NKString& source);
NKString UnicodeToUTF8(const NKWString& source);

#endif // __NKSTRING_HEADER__