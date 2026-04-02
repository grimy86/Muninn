#include "ClientConfig.h"
#include "ProcessPrinter.h"

#ifndef BREAKLINE
#define BREAKLINE std::wcout << '\n'
#endif

int main()
{
	HANDLE hConsole{ GetStdHandle(STD_OUTPUT_HANDLE) };
	DWORD processId{ 0ul };
	bool isRunning{ false };
	bool isInitialized{ false };
	bool isInjected{ false };

	PrintTitle(hConsole, L"__Main()__");
#ifdef PRINT_PROCESSNAME
	PrintConfig(hConsole, PRINT_PROCESSNAME);
#endif

	processId = Muninn::Controller::ProcessController::GetProcessId(
			PROCESSNAMEW,isRunning);
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
	isInitialized = pProcessController->InitializeProcessEntry();
	if (!isInitialized)
	{
		PrintError(hConsole, L"Failed to initialize process entry.");
		BREAKLINE;
		delete pProcessController;
		pProcessController = nullptr;
		return 1;
	}

#ifdef PRINT_ENTRY
	PrintConfig(hConsole, PRINT_ENTRY);
	PrintEntry(pProcessController);
	BREAKLINE;
#endif
		
#ifdef PRINT_MODULES
	PrintConfig(hConsole, PRINT_MODULES);
	pProcessController->InitializeModuleList();
	PrintModules(pProcessController);
	BREAKLINE;
#endif

#undef PRINT_THREADS
#ifdef PRINT_THREADS
	PrintConfig(hConsole, PRINT_MODULES);
	pProcessController->InitializeThreadList();
	PrintThreads(pProcessController);
	BREAKLINE;
#endif

#undef PRINT_HANDLES
#ifdef PRINT_HANDLES
	PrintConfig(hConsole, PRINT_MODULES);
	pProcessController->InitializeHandleList();
	PrintHandles(pProcessController);
	BREAKLINE;
#endif

#ifdef PRINT_DLLPATH
	PrintConfig(hConsole, PRINT_DLLPATH);
#endif

#ifdef PRINT_SIMPLE_INJECTION
	PrintConfig(hConsole, PRINT_SIMPLE_INJECTION);
	PrintSimpleInject();
#endif

	PrintTitle(hConsole, L"__Cleanup__");
	delete pProcessController;
	pProcessController = nullptr;
	BREAKLINE;
	
	PrintTitle(hConsole, L"__Exiting program__");
	return 0;
};