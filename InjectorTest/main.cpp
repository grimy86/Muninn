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

void SimpleInjectDll(Muninn::Controller::ProcessController* pProcessController) noexcept
{
	if (pProcessController->GetInjector().IsInjected == true)
	{
		Log(ConsoleColor::Red, "You've already injected.");
		return;
	}

	bool isDllPathASet{
		pProcessController->GetInjector().DllPathA != nullptr };

	bool isDllPathWSet{
		pProcessController->GetInjector().DllPathW != nullptr };

	if (!isDllPathASet && !isDllPathWSet)
	{
		Log(ConsoleColor::Red, "Please set the injector DLL path first.");
		return;
	}

	if (isDllPathASet)
		pProcessController->SimpleDLLInjectA();

	if (isDllPathWSet)
		pProcessController->SimpleDLLInjectW();

	if (!pProcessController->GetInjector().IsInjected)
		Log(ConsoleColor::Red, "Failed to inject DLL using the path.");

	if (pProcessController->GetInjector().IsInjected)
		Log(ConsoleColor::Yellow, "DLL injected successfully using the path.");

	return;
}