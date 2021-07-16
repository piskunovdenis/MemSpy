#include "Path.hpp"

#include <string>

namespace memspy
{
	std::wstring Path::GetFileName(const std::wstring& fileName)
	{
		size_t pos = fileName.rfind(kPathSeparator, fileName.length());
		if (pos != std::string::npos)
		{
			return fileName.substr(pos + 1, fileName.length() - pos);
		}
		return L"";
	}
}
