#pragma once

#include <string>
#include <vector>
#include <tchar.h>

namespace memspy
{
	class CommandLine
	{
		static const std::vector<wchar_t> kParamSeparators;
		static const std::vector<wchar_t> kNameValueSeparators;

	public:
		CommandLine(int argc, _TCHAR* argv[]);

		bool FindParam(const std::wstring& searchingParamName) const;
		std::wstring GetParamValue(const std::wstring& searchingParamName) const;

	private:
		void ParseParam(const std::wstring& param, std::wstring& rParamName, std::wstring& rParamValue) const;

		std::vector<std::wstring> m_Arguments;
	};
}