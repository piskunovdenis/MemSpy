#pragma once

#include <string>

namespace memspy
{
	class Path
	{
	public:
		static const wchar_t kPathSeparator = L'\\';

		static std::wstring GetFileName(const std::wstring& fileName);
	};
}
