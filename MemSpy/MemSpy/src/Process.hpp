#pragma once

#include <windows.h>
#include <functional>

namespace ms
{
	class Process
	{
	public:
		Process(DWORD desiredAccess, DWORD pid);
		~Process();

		HANDLE Handle() const;
		std::wstring FileName() const;

		static void EnumProcesses(std::function<bool(DWORD pid)> enumerator);
		static bool IsProcessRunning(std::wstring fileName, DWORD& rPID);
	private:
		static const int kMaxProcessCount = 1024;

		DWORD m_pid;
		HANDLE m_Handle;
	};
}