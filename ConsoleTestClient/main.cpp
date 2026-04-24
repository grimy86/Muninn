#include "ClientConfig.h"
#include "ConsoleUtilities.h"

int main()
{
	bool isRunning{ false };
	DWORD processId{ Muninn::Controller::ProcessController::FindProcessId(
			PROCESSNAME_W,isRunning) };

	if (!isRunning)
	{
		Log(ConsoleColor::Red, "Please run the process first.");
		return 1;
	}

	Muninn::Controller::ProcessController* pProcessController{
	new Muninn::Controller::ProcessController(
		processId,
		PROCESS_ALL_ACCESS) };
		

	uintptr_t processHandle{ reinterpret_cast<uintptr_t>(pProcessController->GetProcess().processHandle) };
	Log(ConsoleColor::Yellow, "ProcessHandle: 0x", std::hex, processHandle);
	BREAKLINE;

	bool isInitialized { 
		pProcessController->PopulateProcessEntry() };

	if (!isInitialized)
	{
		Log(ConsoleColor::Red, "Failed to initialize process entry.");
		delete pProcessController;
		pProcessController = nullptr;
		return 1;
	}

	PrintEntry(pProcessController);
	BREAKLINE;

#ifdef PRINT_SIMPLE_INJECTION_W
	pProcessController->SetInjectorDllPathW(DLLPATH_W);
	PrintSimpleInject(pProcessController);
#endif

	Log(ConsoleColor::Yellow, "Cleanup");
	delete pProcessController;
	pProcessController = nullptr;
	return 0;
};