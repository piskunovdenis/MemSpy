#include <windows.h>
#include <psapi.h>
#include <stdexcept>
#include <vector>

#include "Process.hpp"
#include "WideString.hpp"
#include "Path.hpp"

namespace memspy
{
	Process::Process(DWORD desiredAccess, DWORD processId) :
		m_ProcessId(processId)
	{
		m_Handle = OpenProcess(desiredAccess, FALSE, m_ProcessId);
	}

	Process::~Process()
	{
		CloseHandle(m_Handle);
	}

	void Process::EnumProcesses(std::function<bool(DWORD pid)> enumerator)
	{
		if (!enumerator)
		{
			throw std::invalid_argument("enumerator is null");
		}

		DWORD processCount;
		std::vector<DWORD> processIds(kMaxProcessCount);
		if (!::EnumProcesses(&processIds[0], kMaxProcessCount * sizeof(DWORD), &processCount))
			return;

		processCount = processCount / sizeof(DWORD);
		processIds.resize(processCount);

		for (auto processId : processIds)
		{
			if (enumerator(processId))
				return;
		}
	}

	bool Process::IsProcessRunning(std::wstring fileName, DWORD& rProcessId)
	{
		if (fileName.empty())
		{
			throw std::invalid_argument("fileName is empty");
		}

		rProcessId = 0;
		Process::EnumProcesses(
			[fileName, &rProcessId](DWORD pid) -> bool
			{
				if (pid != 0)
				{
					Process process(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, pid);
					std::wstring processFileName = memspy::Path::GetFileName(process.FileName());
					{
						if (memspy::WideString::Compare(processFileName, fileName))
						{
							rProcessId = pid;
							return true;
						}
					}
				}
				return false;
			}
		);
		return rProcessId != 0;
	}

	HANDLE Process::Handle() const
	{
		return m_Handle;
	}

	std::wstring Process::FileName() const
	{
		DWORD bufferLength{ MAX_PATH };
		std::vector<wchar_t> buffer(bufferLength);
		if (QueryFullProcessImageNameW(m_Handle, 0, &buffer[0], &bufferLength))
		{
			return &buffer[0];
		}
		return L"";
	}
}