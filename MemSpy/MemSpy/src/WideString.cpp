#include "WideString.hpp"

#include <algorithm> 
#include <sstream>

namespace memspy
{
	void WideString::ToLower(std::wstring& rStr)
	{
		std::transform(rStr.begin(), rStr.end(), rStr.begin(), towlower);
	}

	bool WideString::Compare(const std::wstring& left, const std::wstring& rigth)
	{
		std::wstring l(left);
		ToLower(l);

		std::wstring r(rigth);
		ToLower(r);

		return l == r;
	}

	std::string WideString::UTF16ToUTF8(const std::wstring& str)
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
		return {};
	}

	std::wstring WideString::ToHex(DWORD_PTR i)
	{
		std::wstringstream ss;
		ss << std::hex << i;
		return ss.str();
	}
}
