#include <windows.h>
#include <string.h>
#include <iostream>

#include "Application.hpp"
#include "CommandLine.hpp"
#include "Process.hpp"
#include "MemoryBlock.hpp"
#include "Console.hpp"

namespace ms
{
	int Application::Run(ms::CommandLine cmd)
	{
		if (cmd.FindParam(L"help"))
		{
			PrintHelp();
			return 0;
		}

		std::wstring secret{ cmd.GetParamValue(L"secret") };
		if (secret.empty())
		{
			return -1;
		}

		m_UTF16Secret = secret;
		m_UTF16SecretByteCount = m_UTF16Secret.length() * sizeof(wchar_t);

		m_UTF8Secret = "";// !! UTF16ToUTF8(SearchingString);
		m_UTF8SecretByteCount = m_UTF8Secret.length() * sizeof(char);

		std::wstring filename{ cmd.GetParamValue(L"filename") };
		if (filename.empty())
		{
			return -2;
		}

		m_ProcessFileName = filename;

		if (m_UseLoop = cmd.FindParam(L"loop"))
		{
			std::wstring iter{ cmd.GetParamValue(L"iter") };
			if (!iter.empty())
			{
				m_LoopIter = std::stoul(iter);
				if (m_LoopIter < 1)
				{
					m_LoopIter = 1;
				}
			}
			else
			{
				m_LoopIter = INT32_MAX;
			}

			std::wstring timeout{ cmd.GetParamValue(L"timeout") };
			if (!timeout.empty())
			{
				m_LoopTimeout = std::stoul(timeout);
				if (m_LoopTimeout < 100)
				{
					m_LoopTimeout = 100;
				}
			}
		}

		std::vector<DWORD_PTR> foundUTF16Addresses;
		std::vector<DWORD_PTR> foundUTF8Addresses;

		PrintSearchingInfo();
		while (m_LoopIter > 0)
		{
			foundUTF16Addresses.clear();
			foundUTF8Addresses.clear();

			DWORD pid;
			if (!ms::Process::IsProcessRunning(m_ProcessFileName, pid))
			{
				PrintSearchingInfo();
				std::wcout << L"Can not find process with file name: " << m_ProcessFileName << std::endl;

				Wait(-3);
				continue;
			}

			ms::Process process(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ, pid);
			if (process.Handle() == INVALID_HANDLE_VALUE)
			{
				PrintSearchingInfo();
				std::wcout << L"Can not open process #" << std::to_wstring(pid) << L" with file name: " << m_ProcessFileName << std::endl;

				Wait(-4);
				continue;
			}

			PrintSearchingAttempt();

			SYSTEM_INFO si;
			ZeroMemory(&si, sizeof(si));
			GetSystemInfo(&si);

			MEMORY_BASIC_INFORMATION mbi;
			ZeroMemory(&mbi, sizeof(mbi));
			void* lpStartAddr = si.lpMinimumApplicationAddress;

			while (VirtualQueryEx(process.Handle(), lpStartAddr, &mbi, sizeof(mbi)) > 0)
			{
				if ((mbi.Protect & PAGE_EXECUTE_READ) == PAGE_EXECUTE_READ ||
					(mbi.Protect & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE ||
					(mbi.Protect & PAGE_READONLY) == PAGE_READONLY ||
					(mbi.Protect & PAGE_READWRITE) == PAGE_READWRITE)
				{
					ms::MemoryBlock dump(mbi.RegionSize);
					SIZE_T bytesRead{ 0 };
					ReadProcessMemory(process.Handle(), mbi.BaseAddress, dump.Memory(), mbi.RegionSize, &bytesRead);
					if (mbi.RegionSize == bytesRead)
					{
						// utf16 secret
						for (DWORD_PTR i = 0; i <= mbi.RegionSize - m_UTF16SecretByteCount - 1; i++)
						{
							void* lpSourceDump = (void*)((DWORD_PTR)dump.Memory() + i);
							if (memcmp(lpSourceDump, m_UTF16Secret.c_str(), m_UTF16SecretByteCount) == 0)
							{
								foundUTF8Addresses.push_back(DWORD_PTR(mbi.BaseAddress) + DWORD_PTR(i));
							}
						}
						// utf8 secret
						for (DWORD_PTR i = 0; i <= mbi.RegionSize - m_UTF8SecretByteCount - 1; i++)
						{
							void* lpSourceDump = (void*)((DWORD_PTR)dump.Memory() + i);
							if (memcmp(lpSourceDump, m_UTF8Secret.c_str(), m_UTF8SecretByteCount) == 0)
							{
								foundUTF8Addresses.push_back(DWORD_PTR(mbi.BaseAddress) + DWORD_PTR(i));
							}
						}
					}
				}
				lpStartAddr = (void*)((DWORD_PTR)mbi.BaseAddress + mbi.RegionSize);
			}

			PrintSearchingInfo();
			if (!foundUTF16Addresses.empty() || !foundUTF8Addresses.empty())
			{
				std::wcout << L"Found secrets count: " << foundUTF16Addresses.size() + foundUTF8Addresses.size() << std::endl;
				for (auto i : foundUTF16Addresses)
				{
					std::wcout << L"Found UTF16 secret at: 0x" << std::hex << i << std::endl;
				}
				for (auto i : foundUTF8Addresses)
				{
					std::wcout << L"Found UTF8 secret at: 0x" << std::hex << i << std::endl;
				}
			}
			else
			{
				std::wcout << L"Secret is not found!" << std::endl;
			}
			Wait(static_cast<DWORD>(foundUTF16Addresses.size() + foundUTF8Addresses.size()));
		}
		return m_ExitCode;
	}

	void Application::PrintHelp()
	{
		std::wcout << L"Usage: " << L"MemFind.exe [/secret=...] [/filename=...] [/loop [/iter=...] [/timeout=...]]" << std::endl;
		std::wcout << L"  /secret - searching secret" << std::endl;
		std::wcout << L"  /filename - process file name" << std::endl;
		std::wcout << L"  /loop - search in loop" << std::endl;
		std::wcout << L"	 /iter - number of loop iterations" << std::endl;
		std::wcout << L"	 /timeout - sleep timeout in milliseconds after each loop" << std::endl;
		std::getchar();
	}

	void Application::PrintSearchingInfo()
	{
		ms::Console::ClearConsoleScreen();
		std::wcout << L"Searching secret: " << m_UTF16Secret << std::endl;
		std::wcout << L"Process file name: " << m_ProcessFileName << std::endl;
		if (m_UseLoop)
		{
			std::wcout << L"Attempt timeout: " + std::to_wstring(m_LoopTimeout) << L" ms" << std::endl;

			GetConsoleScreenBufferInfo(ms::Console::ConsoleOutput(), &m_csbi);
			m_SearchingAttemptCursorPos = m_csbi.dwCursorPosition;

			std::wcout << L"Searching attempt #" << std::to_wstring(m_LoopIter) << L"..." << std::endl;
		}
		std::wcout << std::endl;
	}

	void Application::PrintSearchingAttempt()
	{
		if (m_UseLoop)
		{
			GetConsoleScreenBufferInfo(ms::Console::ConsoleOutput(), &m_csbi);
			SetConsoleCursorPosition(ms::Console::ConsoleOutput(), m_SearchingAttemptCursorPos);
			std::wcout << L"Searching attempt #" << std::to_wstring(m_LoopIter) << L"..." << std::endl << std::endl;
			SetConsoleCursorPosition(ms::Console::ConsoleOutput(), m_csbi.dwCursorPosition);
		}
	}

	void Application::Wait(DWORD exitCode)
	{
		m_LoopIter--;
		if (m_UseLoop)
		{
			Sleep(m_LoopTimeout);
		}
		m_ExitCode = exitCode;
	}
}