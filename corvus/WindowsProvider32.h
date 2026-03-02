#pragma once
#include "WindowsStructures.h"

namespace Corvus::Data
{
#pragma region WRITE
	HANDLE OpenTokenHandle32(const HANDLE processHandle, const ACCESS_MASK accessMask);
	BOOL SetSeDebugPrivilege32();
	BOOL SetSeDebugPrivilege32(const HANDLE tokenHandle);
	BOOL SetThreadPriority32(const DWORD priorityClass);
	BOOL SetThreadSuspended32(const DWORD threadId);
	BOOL SetThreadResumed32(const DWORD threadId);
#pragma endregion

#pragma region READ
	int GetThreadPriority32(HANDLE threadHandle);

	DWORD GetTokenInfoBufferSize32(
		const HANDLE tokenHandle,
		const TOKEN_INFORMATION_CLASS infoClass);

	BOOL GetSeDebugPrivilege32(const HANDLE tokenHandle);

	PROCESSENTRY32W GetProcessInformation32(const DWORD processId);

	BOOL GetProcessInformationObject32(
		const DWORD processId,
		Corvus::Object::ProcessEntry& processEntry);

	std::wstring GetImageFileName32(const HANDLE hProcess);

	uintptr_t GetModuleBaseAddress32(const DWORD processId, const std::wstring& processName);

	BOOL GetWindowVisibility32(const DWORD processId);

	BOOL GetProcessArchitecture32(
		const HANDLE processHandle,
		Corvus::Object::ArchitectureType& architectureType,
		BOOL& isWow64);

	std::vector<std::pair<MODULEENTRY32W, MODULEINFO>> GetProcessModules32(
		const HANDLE processHandle,
		const DWORD processId);

	BOOL GetProcessModuleObjects32(
		const HANDLE processHandle,
		const DWORD processId,
		std::vector<Corvus::Object::ModuleEntry>& modules);

	std::vector<THREADENTRY32> GetProcessThreads32(
		const HANDLE processHandle,
		const DWORD processId);

	BOOL GetProcessThreadObjects32(
		const HANDLE processHandle,
		const DWORD processId,
		std::vector<Corvus::Object::ThreadEntry>& threads);

	std::vector<PSS_HANDLE_ENTRY> GetProcessHandles32(
		const HANDLE processHandle,
		const DWORD processId);

	BOOL GetProcessHandleObjects32(
		const HANDLE processHandle,
		const DWORD processId,
		std::vector<Corvus::Object::HandleEntry>& handles);
#pragma endregion
}