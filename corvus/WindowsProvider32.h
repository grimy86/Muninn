#pragma once
#include "apiconfig.h"
#include "WindowsStructures.h"
#include <ProcessSnapshot.h>

namespace Muninn::Data
{
#pragma region WRITE
	MUNINN_API NTSTATUS MUNINN_CALL
		OpenProcessHandle32(
			_In_ const DWORD processId,
			_In_ const ACCESS_MASK accessMask,
			_Out_ HANDLE* const pHandle) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		CloseHandle32(_In_ const HANDLE handle) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		OpenTokenHandle32(
			_In_ const HANDLE processHandle,
			_In_ const ACCESS_MASK accessMask,
			_Out_ HANDLE* const pTokenHandle) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		SetSeDebugPrivilege32() noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		SetRemoteSeDebugPrivilege32(
			_In_ const HANDLE tokenHandle) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		SetThreadPriority32(_In_ const DWORD priorityClass) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		SetThreadSuspended32(_In_ const DWORD threadId) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		SetThreadResumed32(_In_ const DWORD threadId) noexcept;
#pragma endregion

#pragma region READ
	MUNINN_API NTSTATUS MUNINN_CALL
		GetThreadPriority32(
			_In_ const HANDLE threadHandle,
			_Out_ INT* const pThreadPriority) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		GetTokenInfoBufferSize32(
			_In_ const HANDLE tokenHandle,
			_In_ const _TOKEN_INFORMATION_CLASS infoClass,
			_Out_ DWORD* const pRequiredSize) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		GetSeDebugPrivilege32(
			_In_ const HANDLE tokenHandle,
			_Out_ BOOL* const pIsSeDebugPrivilegeEnabled) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		GetProcessInformation32(
			_In_ const DWORD processId,
			_Out_ PROCESSENTRY32W* const pProcessEntry) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		GetImageFileName32(
			_In_ const HANDLE processHandle,
			_Out_writes_(bufferLength)
			WCHAR* const pBuffer,
			_In_ const DWORD bufferLength,
			_Out_ DWORD* const pCopiedLength) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		GetModuleBaseAddress32(
			_In_ const DWORD processId,
			_In_ const wchar_t* const pModuleName,
			_Out_ uintptr_t* const pModuleBaseAddress) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		GetWindowVisibility32(
			_In_ const DWORD processId,
			_Out_ BOOL* const pIsWindowVisible) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		GetProcessArchitecture32(
			_In_ const HANDLE processHandle,
			_Out_ USHORT* const pProcessMachine,
			_Out_ USHORT* const pNativeMachine,
			_Out_ BOOL* const pIsWow64) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		GetProcessModules32(
			_In_ const HANDLE processHandle,
			_In_ const DWORD processId,
			_Out_writes_(bufferLength)
			MODULEENTRY32W* const pBuffer,
			_In_ const DWORD bufferLength,
			_Out_ DWORD* const pCopiedLength) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		GetProcessThreads32(
			_In_ const HANDLE processHandle,
			_In_ const DWORD processId,
			_Out_writes_(bufferLength)
			THREADENTRY32* const pBuffer,
			_In_ const DWORD bufferLength,
			_Out_ DWORD* const pCopiedLength) noexcept;

	MUNINN_API NTSTATUS MUNINN_CALL
		GetProcessHandles32(
			_In_ const HANDLE processHandle,
			_In_ const DWORD processId,
			_Out_writes_(bufferLength)
			PSS_HANDLE_ENTRY* const pBuffer,
			_In_ const DWORD bufferLength,
			_Out_ DWORD* const pCopiedLength) noexcept;
#pragma endregion
}