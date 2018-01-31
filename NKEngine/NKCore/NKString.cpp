#include "NKString.h"
#include <windows.h>

NKWString MultibyteToUnicode(const NKString& source)
{
	if (source.empty() == true) return nullptr;

	int required_size = MultiByteToWideChar(CP_ACP, 0, source.c_str(), (int)source.size(), nullptr, 0);

	NKWString destination(required_size,NULL);

	int ret = MultiByteToWideChar(CP_ACP, 0, source.c_str(), (int)source.size(), const_cast<wchar_t*>(destination.c_str()), (int)destination.size());
	if (ret == 0)
	{
		return false;
	}
	return std::move(destination);
}

NKString UnicodeToMultibyte(const NKWString& source)
{
	if (source.empty() == true) return nullptr;

	int required_size = WideCharToMultiByte(CP_ACP, 0, source.c_str(), (int)source.size(), nullptr, 0, nullptr, nullptr);

	NKString destination(required_size, NULL);

	int ret = WideCharToMultiByte(CP_ACP, 0, source.c_str(), (int)source.size(), const_cast<char*>(destination.c_str()), (int)destination.size(), nullptr, nullptr);
	if (ret == 0)
	{
		return false;
	}
	return std::move(destination);
}

NKWString UTF8ToUnicode(const NKString& source)
{
	if (source.empty() == true) return nullptr;

	int required_size = MultiByteToWideChar(CP_UTF8, 0, source.c_str(), (int)source.size(), nullptr, 0);

	NKWString destination(required_size, NULL);

	int ret = MultiByteToWideChar(CP_UTF8, 0, source.c_str(), (int)source.size(), const_cast<wchar_t*>(destination.c_str()), (int)destination.size());
	if (ret == 0)
	{
		return false;
	}
	return std::move(destination);
}

NKString UnicodeToUTF8(const NKWString& source)
{
	if (source.empty() == true) return nullptr;

	int required_size = WideCharToMultiByte(CP_UTF8, 0, source.c_str(), (int)source.size(), nullptr, 0, nullptr, nullptr);

	NKString destination(required_size, NULL);

	int ret = WideCharToMultiByte(CP_UTF8, 0, source.c_str(), (int)source.size(), const_cast<char*>(destination.c_str()), (int)destination.size(), nullptr, nullptr);
	if (ret == 0)
	{
		return false;
	}
	return std::move(destination);
}