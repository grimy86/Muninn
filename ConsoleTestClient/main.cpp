#include <iostream>
#include <ProcessController.h>

#ifndef PROCESSNAMEW
#define PROCESSNAMEW L"ac_client.exe"
#endif // !PROCESSNAMEW

int main()
{
	DWORD processId{
		Muninn::Controller::ProcessController::GetProcessId(PROCESSNAMEW) };

	std::wcout << "processId: " << processId << '\n';

	auto* pProcessController{
		new Muninn::Controller::ProcessController(
			processId,
			PROCESS_ALL_ACCESS) };

	std::wcout << "Controller.process.processEntry.processId: " <<
		pProcessController->GetProcessObject().processEntry.processId << '\n';

	std::wcout << std::hex << "Controller.ProcessHandle : 0x" <<
		reinterpret_cast<uintptr_t>(
			pProcessController->GetProcessHandle()) << '\n';

	pProcessController->InitializeProcessEntry();

	std::wcout << std::dec << L"Controller..process.processEntry.processName: "
		<< pProcessController->GetProcessObject().processEntry.processName << L'\n';

	std::wcout << L"Controller..process.processEntry.userFullProcessImageName: "
		<< pProcessController->GetProcessObject().processEntry.userFullProcessImageName << L'\n';

	std::wcout << L"Controller..process.processEntry.NativeImageFileName: "
		<< pProcessController->GetProcessObject().processEntry.NativeImageFileName << L'\n';

	std::wcout << std::hex << L"Controller..process.processEntry.pebBaseAddress: 0x"
		<< pProcessController->GetProcessObject().processEntry.pebBaseAddress << L'\n';

	std::wcout << L"Controller..process.processEntry.pebBaseAddress: 0x"
		<< pProcessController->GetProcessObject().processEntry.moduleBaseAddress << L'\n';

	std::wcout << std::dec << L"Controller..process.processEntry.parentProcessId: "
		<< pProcessController->GetProcessObject().processEntry.parentProcessId << L'\n';

	std::wcout << L"Controller..process.processEntry.isProtectedProcess: "
		<< pProcessController->GetProcessObject().processEntry.isProtectedProcess << L'\n';

	std::wcout << L"Controller..process.processEntry.isWow64Process: "
		<< pProcessController->GetProcessObject().processEntry.isWow64Process << L'\n';

	std::wcout << L"Controller..process.processEntry.isBackgroundProcess: "
		<< pProcessController->GetProcessObject().processEntry.isBackgroundProcess << L'\n';

	std::wcout << L"Controller..process.processEntry.isSecureProcess: "
		<< pProcessController->GetProcessObject().processEntry.isSecureProcess << L'\n';

	std::wcout << L"Controller..process.processEntry.isSubsystemProcess: "
		<< pProcessController->GetProcessObject().processEntry.isSubsystemProcess << L'\n';

	std::wcout << L"Controller..process.processEntry.hasVisibleWindow: "
		<< pProcessController->GetProcessObject().processEntry.hasVisibleWindow << L'\n';

	switch (pProcessController->GetProcessObject().processEntry.architectureType)
	{
	case(Muninn::Models::ArchitectureType::x86):
	{
		std::wcout << L"Controller..process.processEntry.architectureType: " <<
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
};