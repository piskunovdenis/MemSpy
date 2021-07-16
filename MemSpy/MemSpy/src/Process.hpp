#pragma once

#include <windows.h>
#include <functional>

namespace memspy
{
	class Process
	{
	public:
		Process(DWORD desiredAccess, DWORD processId);
		~Process();

		static void EnumProcesses(std::function<bool(DWORD pid)> enumerator);
		static bool IsProcessRunning(std::wstring fileName, DWORD& rProcessId);

		HANDLE Handle() const;
		std::wstring FileName() const;
	private:
		static const int kMaxProcessCount = 1024;

		DWORD m_ProcessId;
		HANDLE m_Handle;
	};
}