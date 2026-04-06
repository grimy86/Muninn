#include "ClientConfig.h"
#include "ConsoleUtilities.h"

int main()
{
	PrintTitle(L"__Main()__");

#ifdef PRINT_PROCESSNAME
	PrintConfig(PRINT_PROCESSNAME);
#endif

	bool isRunning{ false };
	DWORD processId{ Muninn::Controller::ProcessController::FindProcessId(
			PROCESSNAME_W,isRunning) };

	if (!isRunning)
	{
		PrintError(L"Please run the process first.");
		PrintTitle(L"__Exiting program__");
		return 0;
	}

	auto* pProcessController{
	new Muninn::Controller::ProcessController(
		processId,
		PROCESS_ALL_ACCESS) };

	PrintProcessId(pProcessController);
	PrintProcessHandle(pProcessController);
	BREAKLINE;

	PrintTitle(L"__InitializeProcessEntry()__");
	bool isInitialized = pProcessController->PopulateProcessEntry();
	if (!isInitialized)
	{
		PrintError(L"Failed to initialize process entry.");
		BREAKLINE;
		delete pProcessController;
		pProcessController = nullptr;
		return 1;
	}

#ifdef PRINT_ENTRY
	PrintConfig(PRINT_ENTRY);
	PrintEntry(pProcessController);
	BREAKLINE;
#endif
		
#ifdef PRINT_MODULES
	PrintConfig(PRINT_MODULES);
	pProcessController->InitializeModuleList();
	PrintModules(pProcessController);
	BREAKLINE;
#endif

#ifdef PRINT_THREADS
	PrintConfig(PRINT_MODULES);
	pProcessController->InitializeThreadList();
	PrintThreads(pProcessController);
	BREAKLINE;
#endif

#ifdef PRINT_HANDLES
	PrintConfig(PRINT_MODULES);
	pProcessController->InitializeHandleList();
	PrintHandles(pProcessController);
	BREAKLINE;
#endif

#ifdef PRINT_SIMPLE_INJECTION_A
	PrintTitle(L"__PrintSimpleInject()__");
	PrintConfig(PRINT_DLLPATH_A);
	PrintConfig(PRINT_SIMPLE_INJECTION_A);
	pProcessController->SetInjectorDllPathA(DLLPATH_A);
	PrintSimpleInject(pProcessController);
#endif

#ifdef PRINT_SIMPLE_INJECTION_W
	PrintTitle(L"__PrintSimpleInject()__");
	PrintConfig(PRINT_DLLPATH_W);
	PrintConfig(PRINT_SIMPLE_INJECTION_W);
	pProcessController->SetInjectorDllPathW(DLLPATH_W);
	PrintSimpleInject(pProcessController);
#endif

	PrintTitle(L"__Cleanup__");
	delete pProcessController;
	pProcessController = nullptr;
	BREAKLINE;
	
	PrintTitle(L"__Exiting program__");
	return 0;
};