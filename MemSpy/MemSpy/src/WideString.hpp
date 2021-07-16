#pragma once

#include <windows.h>
#include <string>

namespace memspy
{
	class WideString
	{
	public:
		static void ToLower(std::wstring& rStr);

		static bool Compare(const std::wstring& left, const std::wstring& rigth);

		static std::string UTF16ToUTF8(const std::wstring& str);

		static std::wstring ToHex(DWORD_PTR i);
	};
}
