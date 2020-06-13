#include <algorithm> 
#include <windows.h>
#include <stdexcept>

#include "CommandLine.hpp"
#include "WString.hpp"

namespace ms
{
	const std::vector<wchar_t> CommandLine::kParamSeparators = { L'-', L'/' };
	const std::vector<wchar_t> CommandLine::kNameValueSeparators = { L':', L'=' };

	CommandLine::CommandLine(int argc, _TCHAR* argv[]) :
		m_Arguments(argv + 1, argv + argc)
	{
		for (auto& i : m_Arguments)
		{
			ms::WString::ToLower(i);
		}
	}

	bool CommandLine::FindParam(std::wstring searchingParamName) const
	{
		if (searchingParamName.empty())
		{
			throw std::invalid_argument("searchingParamName is empty");
		}

		for (auto i : m_Arguments)
		{
			std::wstring paramName;
			std::wstring paramValue;
			ParseParam(i, paramName, paramValue);

			if (ms::WString::Compare(paramName, searchingParamName) ||
				ms::WString::Compare(L"-" + paramName, searchingParamName) ||
				ms::WString::Compare(L"/" + paramName, searchingParamName))
			{
				return true;
			}
		}
		return false;
	}

	std::wstring CommandLine::GetParamValue(std::wstring searchingParamName) const
	{
		if (searchingParamName.empty())
		{
			throw std::invalid_argument("searchingParamName is empty");
		}

		for (auto i : m_Arguments)
		{
			std::wstring paramName;
			std::wstring paramValue;
			ParseParam(i, paramName, paramValue);

			if (ms::WString::Compare(paramName, searchingParamName))
			{
				return paramValue;
			}
		}
		return L"";
	}

	void CommandLine::ParseParam(std::wstring param, std::wstring& rParamName, std::wstring& rParamValue) const
	{
		if (param.empty())
		{
			throw std::invalid_argument("param is empty");
		}

		rParamName = L"";
		rParamValue = L"";
		if (param.empty())
			return;

		for (auto i : kParamSeparators)
		{
			if (param[0] == i)
			{
				rParamName = param.substr(1, param.length());
				break;
			}
		}

		size_t firstSeparatorPos{ std::wstring::npos };
		for (auto i : kNameValueSeparators)
		{
			size_t separatorPos = rParamName.find(i);
			if (separatorPos != std::wstring::npos)
			{
				firstSeparatorPos = min(separatorPos, firstSeparatorPos);
			}
		}
		if (firstSeparatorPos != std::wstring::npos)
		{
			rParamValue = rParamName.substr(firstSeparatorPos + 1, std::wstring::npos);
		}
		rParamName = rParamName.substr(0, firstSeparatorPos);
	}
}