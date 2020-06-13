#pragma once

#include <string>
#include <vector>
#include <tchar.h>

namespace ms
{
	class CommandLine
	{
	public:
		CommandLine(int argc, _TCHAR* argv[]);

		bool FindParam(std::wstring searchingParamName) const;
		std::wstring GetParamValue(std::wstring searchingParamName) const;
	private:
		static const std::vector<wchar_t> kParamSeparators;
		static const std::vector<wchar_t> kNameValueSeparators;

		std::vector<std::wstring> m_Arguments;

		void ParseParam(std::wstring param, std::wstring& rParamName, std::wstring& rParamValue) const;
	};
}