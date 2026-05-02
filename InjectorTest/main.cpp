#include "Log.h"
#include <ProcessController.h>

#ifndef BREAKLINE
#define BREAKLINE std::cout << '\n'
#endif // !BREAKLINE

const wchar_t* processName{ L"ac_client.exe" };
const wchar_t* dllPath{ L"C:\\Users\\Amend\\source\\repos\\grimy86\\Muninn\\Debug\\InternalTest.dll" };
bool isRunning{ false };
bool success{ false };
DWORD processId{ 0ul };

int main()
{
	std::cout << "Hello World!\n";

	return 0;
}