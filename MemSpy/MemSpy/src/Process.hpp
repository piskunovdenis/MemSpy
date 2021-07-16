#pragma once

#include <windows.h>
#include <functional>

namespace memspy
{
	class Process
	{
		using FnProcessEnumerator = std::function<bool(DWORD pid)>;

		static const int kMaxProcessCount = 1024;

	public:
		Process(DWORD desiredAccess, DWORD processId);
		~Process();

		static void EnumProcesses(const FnProcessEnumerator enumerator);
		static bool IsProcessRunning(const std::wstring& fileName, DWORD& rProcessId);

		HANDLE Handle() const;
		std::wstring FileName() const;
	private:
		DWORD m_ProcessId;
		HANDLE m_Handle;
	};
}