#pragma once

#include <windows.h>
#include <string.h>

#include "CommandLine.hpp"

namespace ms
{
	class Application
	{
	public:
		DWORD Run(ms::CommandLine cmd);
	private:
		std::wstring m_UTF16Secret = L"";
		DWORD_PTR m_UTF16SecretByteCount = 0;

		std::string m_UTF8Secret = "";
		DWORD_PTR m_UTF8SecretByteCount = 0;

		std::wstring m_ProcessFileName = L"";

		bool m_UseLoop = false;
		DWORD m_LoopIter = 1;
		DWORD m_LoopTimeout = 500;

		int m_ExitCode = 0;

		CONSOLE_SCREEN_BUFFER_INFO m_csbi;
		COORD m_SearchingAttemptCursorPos;

		void PrintHelp();
		void PrintSearchingInfo();
		void PrintSearchingAttempt();

		void Wait(int exitCode);
	};
}