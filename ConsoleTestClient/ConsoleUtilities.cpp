#include "Tests.h"
#include "ConsoleUtilities.h"

void ShowEntry(Muninn::Controller::ProcessController* pProcessController) noexcept
{
	LogW(ConsoleColor::Yellow, L"ParentPID: ", std::dec, pProcessController->GetProcess().processEntry.parentProcessId);
	LogW(ConsoleColor::Yellow, L"PID: ", std::dec, pProcessController->GetProcess().processEntry.processId);
	LogW(ConsoleColor::Yellow, L"Name: ", std::dec, pProcessController->GetProcess().processEntry.processName);
	LogW(ConsoleColor::Yellow, L"ImageName32: ", std::dec, pProcessController->GetProcess().processEntry.userFullProcessImageName);
	LogW(ConsoleColor::Yellow, L"ImageNameNt: ", std::dec, pProcessController->GetProcess().processEntry.NativeImageFileName);
	LogW(ConsoleColor::Yellow, L"ModuleBase: ", std::hex, pProcessController->GetProcess().processEntry.moduleBaseAddress);
	LogW(ConsoleColor::Yellow, L"PEB: ", std::hex, pProcessController->GetProcess().processEntry.pebBaseAddress);
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
		std::wcout << std::dec << "moduleList[" << i << "].moduleName:" <<
			pProcessController->GetProcess().moduleList[i].moduleName << '\n';

		std::wcout << "moduleList[" << i << "].moduleName:" <<
			pProcessController->GetProcess().moduleList[i].moduleName << '\n';

		std::wcout << "moduleList[" << i << "].modulePath:" <<
			pProcessController->GetProcess().moduleList[i].modulePath << '\n';

		std::wcout << std::hex << "moduleList[" << i << "].moduleLoadAddress: 0x" <<
			pProcessController->GetProcess().moduleList[i].moduleLoadAddress << '\n';

		std::wcout << "moduleList[" << i << "].moduleEntryPoint:  0x" <<
			pProcessController->GetProcess().moduleList[i].moduleEntryPoint << '\n';

		std::wcout << "moduleList[" << i << "].moduleBaseAddress:  0x" <<
			pProcessController->GetProcess().moduleList[i].moduleBaseAddress << '\n';

		std::wcout << "moduleList[" << i << "].parentDllBaseAddress:  0x" <<
			pProcessController->GetProcess().moduleList[i].parentDllBaseAddress << '\n';

		std::wcout << std::dec << "moduleList[" << i << "].kernelModuleFlags: " <<
			pProcessController->GetProcess().moduleList[i].kernelModuleFlags << '\n';

		std::wcout << "moduleList[" << i << "].moduleImageSize: " <<
			pProcessController->GetProcess().moduleList[i].moduleImageSize << '\n';

		std::wcout << "moduleList[" << i << "].processId: " <<
			pProcessController->GetProcess().moduleList[i].processId << '\n';

		std::wcout << "moduleList[" << i << "].tlsIndex: " <<
			pProcessController->GetProcess().moduleList[i].tlsIndex << '\n';

		BREAKLINE;
	}
}

void TrySimpleDllInjtect(Muninn::Controller::ProcessController* pProcessController) noexcept
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