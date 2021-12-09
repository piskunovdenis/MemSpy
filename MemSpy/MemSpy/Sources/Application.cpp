#include <windows.h>
#include <string.h>
#include <limits>

#include "Application.hpp"
#include "CommandLine.hpp"
#include "Process.hpp"
#include "MemoryBlock.hpp"
#include "Console.hpp"
#include "WideString.hpp"

namespace memspy
{
	DWORD Application::Run(memspy::CommandLine cmd)
	{
		if (cmd.FindParam(L"?"))
		{
			PrintHelp();
			return 0;
		}

		std::wstring secret{ cmd.GetParamValue(L"s") };
		if (secret.empty())
		{
			return -1;
		}

		m_UTF16Secret = secret;
		m_UTF16SecretByteCount = m_UTF16Secret.length() * sizeof(wchar_t);

		m_UTF8Secret = WideString::UTF16ToUTF8(m_UTF16Secret);
		m_UTF8SecretByteCount = m_UTF8Secret.length() * sizeof(char);

		m_ProcessFileName = cmd.GetParamValue(L"f");
		if (m_ProcessFileName.empty())
		{
			return -2;
		}

		if (m_UseLoop = cmd.FindParam(L"l"))
		{
			std::wstring loopIterations{ cmd.GetParamValue(L"i") };
			if (!loopIterations.empty())
			{
				m_LoopIteration = std::stoul(loopIterations);
				if (m_LoopIteration < 1)
				{
					m_LoopIteration = 1;
				}
			}
			else
			{
				m_LoopIteration = (std::numeric_limits<DWORD>::max)();
			}

			std::wstring timeout{ cmd.GetParamValue(L"t") };
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
		while (m_LoopIteration > 0)
		{
			foundUTF16Addresses.clear();
			foundUTF8Addresses.clear();

			DWORD pid;
			if (!memspy::Process::IsProcessRunning(m_ProcessFileName, pid))
			{
				PrintSearchingInfo();
				Console::WriteLine(L"Can not find process with file name: " + m_ProcessFileName);

				Wait(-3);
				continue;
			}

			memspy::Process process(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ, pid);
			if (process.Handle() == INVALID_HANDLE_VALUE)
			{
				PrintSearchingInfo();
				Console::WriteLine(L"Can not open process #" + std::to_wstring(pid) + L" with file name: " + m_ProcessFileName);

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
					memspy::MemoryBlock dump(mbi.RegionSize);
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
								foundUTF16Addresses.push_back(DWORD_PTR(mbi.BaseAddress) + DWORD_PTR(i));
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
				Console::WriteLine(L"Found secrets count: " + std::to_wstring(foundUTF16Addresses.size() + foundUTF8Addresses.size()));
				for (auto i : foundUTF16Addresses)
				{
					Console::WriteLine(L"Found UTF16 secret at: 0x" + WideString::ToHex(i));
				}
				for (auto i : foundUTF8Addresses)
				{
					Console::WriteLine(L"Found UTF8 secret at: 0x" + WideString::ToHex(i));
				}
			}
			else
			{
				Console::WriteLine(L"Secret is not found!");
			}
			Wait(static_cast<int>(foundUTF16Addresses.size() + foundUTF8Addresses.size()));
		}
		return m_ExitCode;
	}

	void Application::PrintHelp()
	{
		Console::WriteLine(L"Usage: MemSpy.exe [/s=...] [/f=...] [/l [/i=...] [/t=...]]");
		Console::WriteLine(L"  /s - searching secret");
		Console::WriteLine(L"  /f - process file name");
		Console::WriteLine(L"  /l - search in loop");
		Console::WriteLine(L"	 /i - number of loop iterations");
		Console::WriteLine(L"	 /t - sleep timeout in milliseconds after each loop");
		std::getchar();
	}

	void Application::PrintSearchingInfo()
	{
		memspy::Console::ClearScreen();
		Console::WriteLine(std::wstring(L"Searching secret: ") + m_UTF16Secret);
		Console::WriteLine(L"Process file name: " + m_ProcessFileName);
		if (m_UseLoop)
		{
			Console::WriteLine(L"Attempt timeout: " + std::to_wstring(m_LoopTimeout) + L" ms");

			GetConsoleScreenBufferInfo(memspy::Console::StdOutput(), &m_csbi);
			m_SearchingAttemptCursorPos = m_csbi.dwCursorPosition;

			Console::WriteLine(L"Searching attempt #" + std::to_wstring(m_LoopIteration) + L"...");
		}
		Console::WriteLine();
	}

	void Application::PrintSearchingAttempt()
	{
		if (m_UseLoop)
		{
			GetConsoleScreenBufferInfo(memspy::Console::StdOutput(), &m_csbi);
			SetConsoleCursorPosition(memspy::Console::StdOutput(), m_SearchingAttemptCursorPos);
			Console::WriteLine(L"Searching attempt #" + std::to_wstring(m_LoopIteration) + L"...");
			Console::WriteLine();
			Console::WriteLine();
			SetConsoleCursorPosition(memspy::Console::StdOutput(), m_csbi.dwCursorPosition);
		}
	}

	void Application::Wait(int exitCode)
	{
		m_LoopIteration--;
		if (m_UseLoop)
		{
			Sleep(m_LoopTimeout);
		}
		m_ExitCode = exitCode;
	}
}