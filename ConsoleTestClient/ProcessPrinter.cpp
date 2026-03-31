#include "ProcessPrinter.h"

#ifndef FOREGROUND_WHITE
#define FOREGROUND_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#endif // !FOREGROUND_WHITE

void PrintTitle(HANDLE hConsole, const wchar_t* title) noexcept
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::wcout << title << '\n';
	SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
}

void PrintError(HANDLE hConsole, const wchar_t* error) noexcept
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
	std::wcout << error << '\n';
	SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
}

void PrintConfig(HANDLE hConsole, const wchar_t* config) noexcept
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	std::wcout << config << '\n';
	SetConsoleTextAttribute(hConsole, FOREGROUND_WHITE);
}

void PrintProcessId(Muninn::Controller::ProcessController* pProcessController) noexcept
{
	std::wcout << std::dec << "processId: " <<
		pProcessController->GetProcessId() << '\n';
}

void PrintProcessHandle(Muninn::Controller::ProcessController* pProcessController) noexcept
{
	std::wcout << std::hex << "processHandle: 0x" <<
		reinterpret_cast<uintptr_t>(
			pProcessController->GetProcessHandle()) << '\n';
}

void PrintEntry(Muninn::Controller::ProcessController* pProcessController) noexcept
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

void PrintModules(Muninn::Controller::ProcessController* pProcessController) noexcept
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