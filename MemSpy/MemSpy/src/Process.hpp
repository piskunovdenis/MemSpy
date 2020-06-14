#pragma once

#include <windows.h>
#include <functional>

namespace ms
{
	class Process
	{
	public:
		Process(DWORD desiredAccess, DWORD processId);
		~Process();

		HANDLE Handle() const;
		std::wstring FileName() const;

		static void EnumProcesses(std::function<bool(DWORD pid)> enumerator);
		static bool IsProcessRunning(std::wstring fileName, DWORD& rProcessId);
	private:
		static const int kMaxProcessCount = 1024;

		DWORD m_ProcessId;
		HANDLE m_Handle;
	};
}