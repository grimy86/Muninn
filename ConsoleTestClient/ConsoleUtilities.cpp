#include "ConsoleUtilities.h"

void PrintEntry(Muninn::Controller::ProcessController* pProcessController) noexcept
{
	std::wcout << std::dec << "processEntry.processId: " <<
		pProcessController->GetProcess().processEntry.processId << '\n';

	std::wcout << L"processEntry.processName: "
		<< pProcessController->GetProcess().processEntry.processName << L'\n';

	std::wcout << L"processEntry.userFullProcessImageName: "
		<< pProcessController->GetProcess().processEntry.userFullProcessImageName << L'\n';

	std::wcout << L"processEntry.NativeImageFileName: "
		<< pProcessController->GetProcess().processEntry.NativeImageFileName << L'\n';

	std::wcout << std::hex << L"processEntry.pebBaseAddress: 0x"
		<< pProcessController->GetProcess().processEntry.pebBaseAddress << L'\n';

	std::wcout << L"processEntry.moduleBaseAddress: 0x"
		<< pProcessController->GetProcess().processEntry.moduleBaseAddress << L'\n';

	std::wcout << std::dec << L"processEntry.parentProcessId: "
		<< pProcessController->GetProcess().processEntry.parentProcessId << L'\n';

	std::wcout << L"processEntry.isProtectedProcess: "
		<< pProcessController->GetProcess().processEntry.isProtectedProcess << L'\n';

	std::wcout << L"processEntry.isWow64Process: "
		<< pProcessController->GetProcess().processEntry.isWow64Process << L'\n';

	std::wcout << L"processEntry.isBackgroundProcess: "
		<< pProcessController->GetProcess().processEntry.isBackgroundProcess << L'\n';

	std::wcout << L"processEntry.isSecureProcess: "
		<< pProcessController->GetProcess().processEntry.isSecureProcess << L'\n';

	std::wcout << L"processEntry.isSubsystemProcess: "
		<< pProcessController->GetProcess().processEntry.isSubsystemProcess << L'\n';

	std::wcout << L"processEntry.hasVisibleWindow: "
		<< pProcessController->GetProcess().processEntry.hasVisibleWindow << L'\n';

	switch (pProcessController->GetProcess().processEntry.architectureType)
	{
	case(Muninn::Model::ArchitectureType::x86):
	{
		std::wcout << L"processEntry.architectureType: " <<
			L"x86" << L'\n';
		break;
	}
	case(Muninn::Model::ArchitectureType::x64):
	{
		std::wcout << L"Controller..process.processEntry.architectureType: " <<
			L"x64" << L'\n';
		break;
	}
	case(Muninn::Model::ArchitectureType::Unknown):
	{
		std::wcout << L"Controller..process.processEntry.architectureType: " <<
			L"Unknown" << L'\n';
		break;
	}
	default:
	{
		std::wcout << L"Controller..process.processEntry.architectureType: " <<
			L"Unknown" << L'\n';
		break;
	}
	}
}

void PrintModules(Muninn::Controller::ProcessController* pProcessController) noexcept
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

void PrintSimpleInject(Muninn::Controller::ProcessController* pProcessController) noexcept
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