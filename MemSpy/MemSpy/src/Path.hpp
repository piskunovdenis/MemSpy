#pragma once

#include <string>

namespace ms
{
	class Path
	{
	public:
		static const wchar_t kPathSeparator = L'\\';

		static std::wstring GetFileName(std::wstring fileName)
		{
			size_t pos = fileName.rfind(kPathSeparator, fileName.length());
			if (pos != std::string::npos)
			{
				return fileName.substr(pos + 1, fileName.length() - pos);
			}
			return L"";
		}
	};
}
