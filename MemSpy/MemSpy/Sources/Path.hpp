#pragma once

#include <string>

namespace memspy
{
	class Path
	{
		static const wchar_t kPathSeparator = L'\\';

	public:
		static std::wstring GetFileName(const std::wstring& fileName);
	};
}
