#include "Log.h"
#include <ProcessController.h>

#ifndef BREAKLINE
#define BREAKLINE std::cout << '\n'
#endif // !BREAKLINE

const wchar_t* processName{ L"ac_client.exe" };
bool isRunning{ false };
bool success{ false };
DWORD processId{ 0ul };

void ShowEntry(Muninn::Controller::ProcessController* pProcessController) noexcept;
void ShowModules(Muninn::Controller::ProcessController* pProcessController) noexcept;

int main()
{
	processId = Muninn::Controller::ProcessController::FindProcessId(
		processName,
		isRunning);

	if (!isRunning)
	{
		Log(ConsoleColor::Red, "Please run the process first.");
		return 1;
	}

	Muninn::Controller::ProcessController* pProcessController{
	new Muninn::Controller::ProcessController(
		processId,
		PROCESS_ALL_ACCESS) };
		
	uintptr_t processHandle{
		reinterpret_cast<uintptr_t>(
			pProcessController->GetProcess().processHandle) };

	success = pProcessController->PopulateProcessEntry();
	
	if (!success)
	{
		Log(ConsoleColor::Red, "Failed to initialize process entry.");
		delete pProcessController;
		pProcessController = nullptr;
		return 1;
	}

	success = pProcessController->PopulateProcessModuleList();

	if (!success)
	{
		Log(ConsoleColor::Red, "Failed to initialize module list.");
		delete pProcessController;
		pProcessController = nullptr;
		return 1;
	}

	ShowEntry(pProcessController);
	BREAKLINE;
	ShowModules(pProcessController);

	delete pProcessController;
	pProcessController = nullptr;
	return 0;
};

void ShowEntry(Muninn::Controller::ProcessController* pProcessController) noexcept
{
	LogW(ConsoleColor::BrightBlue, L"processEntry:");
	LogW(ConsoleColor::Yellow, L"ParentPID: ", std::dec, pProcessController->GetProcess().processEntry.parentProcessId);
	LogW(ConsoleColor::Yellow, L"PID: ", std::dec, pProcessController->GetProcess().processEntry.processId);
	LogW(ConsoleColor::Yellow, L"Name: ", std::dec, pProcessController->GetProcess().processEntry.processName);
	LogW(ConsoleColor::Yellow, L"ImageName32: ", std::dec, pProcessController->GetProcess().processEntry.userFullProcessImageName);
	LogW(ConsoleColor::Yellow, L"ImageNameNt: ", std::dec, pProcessController->GetProcess().processEntry.NativeImageFileName);
	LogW(ConsoleColor::Yellow, L"ModuleBase: 0x", std::hex, pProcessController->GetProcess().processEntry.moduleBaseAddress);
	LogW(ConsoleColor::Yellow, L"PEB: 0x", std::hex, pProcessController->GetProcess().processEntry.pebBaseAddress);
	LogW(ConsoleColor::Yellow, L"Protected: ", std::dec, pProcessController->GetProcess().processEntry.isProtectedProcess);
	LogW(ConsoleColor::Yellow, L"WoW64: ", std::dec, pProcessController->GetProcess().processEntry.isWow64Process);
	LogW(ConsoleColor::Yellow, L"Background: ", std::dec, pProcessController->GetProcess().processEntry.isBackgroundProcess);
	LogW(ConsoleColor::Yellow, L"Secure: ", std::dec, pProcessController->GetProcess().processEntry.isSecureProcess);
	LogW(ConsoleColor::Yellow, L"Subsystem: ", std::dec, pProcessController->GetProcess().processEntry.isSubsystemProcess);
	LogW(ConsoleColor::Yellow, L"VisibleWindow: ", std::dec, pProcessController->GetProcess().processEntry.hasVisibleWindow);

	switch (pProcessController->GetProcess().processEntry.architectureType)
	{
	case(Muninn::Model::ArchitectureType::x86):
	{
		LogW(ConsoleColor::Yellow, L"Architecture: x86");
		break;
	}
	case(Muninn::Model::ArchitectureType::x64):
	{
		LogW(ConsoleColor::Yellow, L"Architecture: x64");
		break;
	}
	case(Muninn::Model::ArchitectureType::Unknown):
	{
		LogW(ConsoleColor::Yellow, L"Architecture: Unknown");
		break;
	}
	default:
	{
		LogW(ConsoleColor::Yellow, L"Architecture: Unknown");
		break;
	}
	}
}

void ShowModules(Muninn::Controller::ProcessController* pProcessController) noexcept
{
	for (DWORD i{ 0ul }; i < pProcessController->GetProcess().moduleList.size(); ++i)
	{
		LogW(ConsoleColor::BrightBlue, L"Module: ", std::dec, pProcessController->GetProcess().moduleList[i].moduleName);
		LogW(ConsoleColor::Yellow, L"PID: ", std::dec, pProcessController->GetProcess().moduleList[i].processId);
		LogW(ConsoleColor::Yellow, L"modulePath: ", std::dec, pProcessController->GetProcess().moduleList[i].modulePath);
		LogW(ConsoleColor::Yellow, L"moduleBaseAddress: 0x", std::hex , pProcessController->GetProcess().moduleList[i].moduleBaseAddress);
		LogW(ConsoleColor::Yellow, L"moduleLoadAddress: 0x", std::hex, pProcessController->GetProcess().moduleList[i].moduleLoadAddress);
		LogW(ConsoleColor::Yellow, L"moduleEntryPoint: 0x", std::hex , pProcessController->GetProcess().moduleList[i].moduleEntryPoint);
		LogW(ConsoleColor::Yellow, L"parentDllBaseAddress: 0x", std::hex , pProcessController->GetProcess().moduleList[i].parentDllBaseAddress);
		LogW(ConsoleColor::Yellow, L"kernelModuleFlags: ", std::dec , pProcessController->GetProcess().moduleList[i].kernelModuleFlags);
		LogW(ConsoleColor::Yellow, L"moduleImageSize: ", std::dec , pProcessController->GetProcess().moduleList[i].moduleImageSize);
		LogW(ConsoleColor::Yellow, L"tlsIndex: ", std::dec , pProcessController->GetProcess().moduleList[i].tlsIndex);
		BREAKLINE;
	}
}