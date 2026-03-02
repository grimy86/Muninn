#pragma once
#include "WindowsStructures.h"

/* NOT WIN32 SUPPORTED:
	Opening, duplicating or closing handles
	Writing or reading processMemory
	ProcessAccessToken Functions
*/

namespace Corvus::Data
{
#pragma region WRITE
	BOOL SetThreadSuspended32(const DWORD threadId);
	BOOL SetThreadResumed32(const DWORD threadId);
	BOOL SetSeDebugPrivilege32();
	BOOL SetSeDebugPrivilege32(const DWORD processId);
	BOOL SetThreadPriority32(int priorityMask);
#pragma endregion

#pragma region READ
	BOOL GetSeDebugPrivilege32(HANDLE hProcess);
	BOOL GetThreadPriority32(HANDLE hThread);

	// PROCESS_EXTENDED_BASIC_INFORMATION GetProcessInformationNt(HANDLE hProcess);
	BOOL GetProcessInformationObject32(Corvus::Object::ProcessEntry& processEntry);
	std::wstring GetImageFileName32(HANDLE hProcess);
	uintptr_t GetModuleBaseAddress32(DWORD processId, const std::wstring& processName);
	BOOL GetWindowVisibility32(DWORD processId);
	Corvus::Object::ArchitectureType GetProcessArchitecture32(HANDLE hProcess, BOOL& isWow64);

	std::vector<Corvus::Object::ModuleEntry> GetProcessModules32(Corvus::Object::ProcessObject& process);
	std::vector<Corvus::Object::ThreadEntry> GetProcessThreads32(Corvus::Object::ProcessObject& process);
	std::vector<Corvus::Object::HandleEntry> GetProcessHandles32(Corvus::Object::ProcessObject& process);
#pragma endregion
}