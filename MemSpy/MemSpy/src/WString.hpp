#pragma once

#include <string>
#include <algorithm> 

namespace ms
{
	class WString
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
	};
}
