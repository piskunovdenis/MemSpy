#pragma once

#include <windows.h>
#include <string>
#include <algorithm> 
#include <sstream>

namespace ms
{
	class WideString
	{
	public:
		static void ToLower(std::wstring& rStr)
		{
			std::transform(rStr.begin(), rStr.end(), rStr.begin(), towlower);
		}

		static bool Compare(std::wstring left, std::wstring rigth)
		{
			ToLower(left);
			ToLower(rigth);
			return left == rigth;
		}

		static std::string UTF16ToUTF8(std::wstring str)
		{
			if (!str.empty())
			{
				int requiredByteCount = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), 0, 0, 0, 0);
				if (requiredByteCount > 0)
				{
					std::string ret(requiredByteCount, 0);
					if (::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), &ret[0], requiredByteCount, 0, 0) > 0)
						return ret;
				}
			}
			return std::string();
		}

		static std::wstring ToHex(DWORD_PTR i)
		{
			std::wstringstream ss;
			ss << std::hex << i;
			return ss.str();
		}
	};
}
