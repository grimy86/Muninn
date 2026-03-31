#include "ClientConfig.h"
#include "ProcessPrinter.h"

#ifndef BREAKLINE
#define BREAKLINE std::wcout << '\n'
#endif


int main()
{
	HANDLE hConsole{ GetStdHandle(STD_OUTPUT_HANDLE) };
	PrintTitle(hConsole, L"__Main()__");
	PrintConfig(hConsole, PROCESSNAMEW);

	bool isRunning{ false };
	DWORD processId{ Muninn::Controller::ProcessController::GetProcessId(
			PROCESSNAMEW, isRunning) };

	if (!isRunning)
	{
		PrintError(hConsole, L"Please run the process first.");
		PrintTitle(hConsole, L"__Exiting program__");
		return 0;
	}

	auto* pProcessController{
	new Muninn::Controller::ProcessController(
		processId,
		PROCESS_ALL_ACCESS) };


	PrintProcessId(pProcessController);
	PrintProcessHandle(pProcessController);
	BREAKLINE;

	PrintTitle(hConsole, L"__InitializeProcessEntry()__");
	if (!pProcessController->InitializeProcessEntry())
	{
		PrintError(hConsole, L"Failed to initialize process entry.");
		BREAKLINE;
		delete pProcessController;
		pProcessController = nullptr;
		return 1;
	}

	PrintEntry(pProcessController);
	BREAKLINE;
		
	PrintTitle(hConsole, L"__Cleanup__");
	delete pProcessController;
	pProcessController = nullptr;
	BREAKLINE;
	
	PrintTitle(hConsole, L"__Exiting program__");
	return 0;
};