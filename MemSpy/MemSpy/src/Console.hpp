#include <windows.h>

namespace ms
{
	class Console
	{
	public:
		static HANDLE ConsoleOutput()
		{
			return GetStdHandle(STD_OUTPUT_HANDLE);
		}

		static void ClearConsoleScreen()
		{
			if (ConsoleOutput() == INVALID_HANDLE_VALUE)
				return;

			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (!GetConsoleScreenBufferInfo(ConsoleOutput(), &csbi))
				return;

			COORD coord{ 0,0 };
			DWORD consoleSize = csbi.dwSize.X * csbi.dwSize.Y;
			DWORD charsWritten;

			if (!FillConsoleOutputCharacter(ConsoleOutput(), L' ', consoleSize, coord, &charsWritten))
				return;
			if (!GetConsoleScreenBufferInfo(ConsoleOutput(), &csbi))
				return;

			FillConsoleOutputAttribute(ConsoleOutput(), csbi.wAttributes, consoleSize, coord, &charsWritten);
			SetConsoleCursorPosition(ConsoleOutput(), coord);
		}
	};
}
