#include <windows.h>

namespace ms
{
	class Console
	{
	public:
		static HANDLE StdOutput()
		{
			return GetStdHandle(STD_OUTPUT_HANDLE);
		}

		static void ClearScreen()
		{
			if (StdOutput() == INVALID_HANDLE_VALUE)
				return;

			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (!GetConsoleScreenBufferInfo(StdOutput(), &csbi))
				return;

			COORD coord{ 0,0 };
			DWORD consoleSize = csbi.dwSize.X * csbi.dwSize.Y;
			DWORD charsWritten;

			if (!FillConsoleOutputCharacter(StdOutput(), L' ', consoleSize, coord, &charsWritten))
				return;
			if (!GetConsoleScreenBufferInfo(StdOutput(), &csbi))
				return;

			FillConsoleOutputAttribute(StdOutput(), csbi.wAttributes, consoleSize, coord, &charsWritten);
			SetConsoleCursorPosition(StdOutput(), coord);
		}

		static void WriteLine()
		{
			DWORD charsWritten;
			WriteConsoleW(StdOutput(), L"\n", 1, &charsWritten, nullptr);
		}

		static void WriteLine(std::wstring str)
		{
			DWORD charsWritten;
			WriteConsoleW(StdOutput(), str.c_str(), static_cast<DWORD>(str.length()), &charsWritten, nullptr);
			WriteLine();
		}
	};
}
