#include <iostream>
#include <ProcessController.h>

#ifndef PROCESSNAMEW
#define PROCESSNAMEW L"ac_client.exe"
#endif // !PROCESSNAMEW

void ParseProcessEntry(Muninn::Controller::ProcessController* pProcessController);
void ParseProcessModules(Muninn::Controller::ProcessController* pProcessController);
int main()
{
	std::wcout << L"___MAIN()___" << '\n';
	DWORD processId{
		Muninn::Controller::ProcessController::GetProcessId(
			PROCESSNAMEW) };

	auto* pProcessController{
		new Muninn::Controller::ProcessController(
			processId,
			PROCESS_ALL_ACCESS) };

	std::wcout << std::dec << "processId: " <<
		processId << '\n';

	std::wcout << std::hex << "processHandle: 0x" <<
		reinterpret_cast<uintptr_t>(
			pProcessController->GetProcessHandle()) << '\n';
	std::wcout << '\n';

	std::wcout << L"___PROCESSENTRY___" << '\n';
	if (pProcessController->InitializeProcessEntry())
		ParseProcessEntry(pProcessController);
	std::wcout << '\n';

	std::wcout << L"___MODULELIST___" << '\n';
	if (pProcessController->InitializeModuleList())
		ParseProcessModules(pProcessController);
	std::wcout << '\n';
};

void ParseProcessEntry(Muninn::Controller::ProcessController* pProcessController)
{
	std::wcout << std::dec << "processEntry.processId: " <<
		pProcessController->GetProcessObject().processEntry.processId << '\n';

	std::wcout << L"processEntry.processName: "
		<< pProcessController->GetProcessObject().processEntry.processName << L'\n';

	std::wcout << L"processEntry.userFullProcessImageName: "
		<< pProcessController->GetProcessObject().processEntry.userFullProcessImageName << L'\n';

	std::wcout << L"processEntry.NativeImageFileName: "
		<< pProcessController->GetProcessObject().processEntry.NativeImageFileName << L'\n';

	std::wcout << std::hex << L"processEntry.pebBaseAddress: 0x"
		<< pProcessController->GetProcessObject().processEntry.pebBaseAddress << L'\n';

	std::wcout << L"processEntry.pebBaseAddress: 0x"
		<< pProcessController->GetProcessObject().processEntry.moduleBaseAddress << L'\n';

	std::wcout << std::dec << L"processEntry.parentProcessId: "
		<< pProcessController->GetProcessObject().processEntry.parentProcessId << L'\n';

	std::wcout << L"processEntry.isProtectedProcess: "
		<< pProcessController->GetProcessObject().processEntry.isProtectedProcess << L'\n';

	std::wcout << L"processEntry.isWow64Process: "
		<< pProcessController->GetProcessObject().processEntry.isWow64Process << L'\n';

	std::wcout << L"processEntry.isBackgroundProcess: "
		<< pProcessController->GetProcessObject().processEntry.isBackgroundProcess << L'\n';

	std::wcout << L"processEntry.isSecureProcess: "
		<< pProcessController->GetProcessObject().processEntry.isSecureProcess << L'\n';

	std::wcout << L"processEntry.isSubsystemProcess: "
		<< pProcessController->GetProcessObject().processEntry.isSubsystemProcess << L'\n';

	std::wcout << L"processEntry.hasVisibleWindow: "
		<< pProcessController->GetProcessObject().processEntry.hasVisibleWindow << L'\n';

	switch (pProcessController->GetProcessObject().processEntry.architectureType)
	{
	case(Muninn::Models::ArchitectureType::x86):
	{
		std::wcout << L"processEntry.architectureType: " <<
			L"x86" << L'\n';
		break;
	}
	case(Muninn::Models::ArchitectureType::x64):
	{
		std::wcout << L"Controller..process.processEntry.architectureType: " <<
			L"x64" << L'\n';
		break;
	}
	case(Muninn::Models::ArchitectureType::Unknown):
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

void ParseProcessModules(Muninn::Controller::ProcessController* pProcessController)
{
	for (DWORD i{ 0ul }; i < pProcessController->GetProcessObject().moduleList.size(); ++i)
	{
		std::wcout << std::dec << "moduleList[" << i << "].moduleName:" <<
			pProcessController->GetProcessObject().moduleList[i].moduleName << '\n';

		std::wcout << "moduleList[" << i << "].moduleName:" <<
			pProcessController->GetProcessObject().moduleList[i].moduleName << '\n';

		std::wcout << "moduleList[" << i << "].modulePath:" <<
			pProcessController->GetProcessObject().moduleList[i].modulePath << '\n';

		std::wcout << std::hex << "moduleList[" << i << "].moduleLoadAddress: 0x" <<
			pProcessController->GetProcessObject().moduleList[i].moduleLoadAddress << '\n';

		std::wcout << "moduleList[" << i << "].moduleEntryPoint:  0x" <<
			pProcessController->GetProcessObject().moduleList[i].moduleEntryPoint << '\n';

		std::wcout << "moduleList[" << i << "].moduleBaseAddress:  0x" <<
			pProcessController->GetProcessObject().moduleList[i].moduleBaseAddress << '\n';

		std::wcout << "moduleList[" << i << "].parentDllBaseAddress:  0x" <<
			pProcessController->GetProcessObject().moduleList[i].parentDllBaseAddress << '\n';

		std::wcout << std::dec << "moduleList[" << i << "].kernelModuleFlags: " <<
			pProcessController->GetProcessObject().moduleList[i].kernelModuleFlags << '\n';

		std::wcout << "moduleList[" << i << "].moduleImageSize: " <<
			pProcessController->GetProcessObject().moduleList[i].moduleImageSize << '\n';

		std::wcout << "moduleList[" << i << "].processId: " <<
			pProcessController->GetProcessObject().moduleList[i].processId << '\n';

		std::wcout << "moduleList[" << i << "].tlsIndex: " <<
			pProcessController->GetProcessObject().moduleList[i].tlsIndex << '\n';
	}
}