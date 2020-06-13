#include <windows.h>
#include <psapi.h>
#include <stdexcept>
#include <vector>

#include "Process.hpp"
#include "WString.hpp"
#include "Path.hpp"

namespace ms
{
	Process::Process(DWORD desiredAccess, DWORD pid)
	{
		m_pid = m_pid;
		m_Handle = OpenProcess(desiredAccess, FALSE, m_pid);
		if (m_Handle == nullptr)
		{
			throw std::runtime_error("OpenProcess failed");
		}
	}

	Process::~Process()
	{
		CloseHandle(m_Handle);
	}

	HANDLE Process::Handle() const
	{
		return m_Handle;
	}

	std::wstring Process::FileName() const
	{
		DWORD bufferLength = MAX_PATH;
		std::vector<wchar_t> buffer(bufferLength);
		if (QueryFullProcessImageNameW(m_Handle, 0, &buffer[0], &bufferLength))
		{
			return &buffer[0];
		}
		return L"";
	}

	void Process::EnumProcesses(std::function<bool(DWORD pid)> enumerator)
	{
		if (!enumerator)
		{
			throw std::invalid_argument("enumerator is null");
		}

		DWORD processCount;
		std::vector<DWORD> pids(kMaxProcessCount);
		if (!::EnumProcesses(&pids[0], pids.size() * sizeof(DWORD), &processCount)) // !! size()
			return;

		processCount = processCount / sizeof(DWORD);
		pids.resize(processCount);

		for (auto pid : pids)
		{
			if (enumerator(pid))
				return;
		}
	}

	bool Process::IsProcessRunning(std::wstring fileName, DWORD& rPID)
	{
		if (fileName.empty())
		{
			throw std::invalid_argument("fileName is empty");
		}

		rPID = 0;
		Process::EnumProcesses(
			[fileName, &rPID](DWORD pid) -> bool
			{
				if (pid != 0)
				{
					Process process(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, pid);
					std::wstring processFileName = ms::Path::GetFileName(process.FileName());
					{
						if (ms::WString::Compare(processFileName, fileName))
						{
							rPID = pid;
							return true;
						}
					}
				}
				return false;
			}
		);
		return rPID != 0;
	}
}