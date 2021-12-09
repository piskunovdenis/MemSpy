#include <windows.h>
#include <string>

namespace memspy
{
	class Console
	{
	public:
		static HANDLE StdOutput();

		static void ClearScreen();

		static void WriteLine();
		static void WriteLine(const std::wstring& str);
	};
}
