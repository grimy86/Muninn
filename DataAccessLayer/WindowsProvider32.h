#ifndef WINDOWS_PROVIDER_32_H
#define WINDOWS_PROVIDER_32_H

#include "ApiConfig.h"
#include <phnt_windows.h>
#include <TlHelp32.h>
#include <ProcessSnapshot.h>

#ifndef NTSTATUS
#define NTSTATUS LONG
#endif // !NTSTATUS

MUNINN_API NTSTATUS MUNINN_CALL
DAL_OpenProcessHandle32(
	_In_ const DWORD processId,
	_In_ const ACCESS_MASK accessMask,
	_Out_ HANDLE* const pHandle);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_CloseHandle32(_In_ const HANDLE handle);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_OpenTokenHandle32(
	_In_ const HANDLE processHandle,
	_In_ const ACCESS_MASK accessMask,
	_Out_ HANDLE* const pTokenHandle);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SetSeDebugPrivilege32();

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SetRemoteSeDebugPrivilege32(
	_In_ const HANDLE tokenHandle);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SetThreadPriority32(_In_ const DWORD priorityClass);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SetThreadSuspended32(_In_ const DWORD threadId);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SetThreadResumed32(_In_ const DWORD threadId);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetThreadPriority32(
	_In_ const HANDLE threadHandle,
	_Out_ INT* const pThreadPriority);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetTokenInfoBufferSize32(
	_In_ const HANDLE tokenHandle,
	_In_ const TOKEN_INFORMATION_CLASS infoClass,
	_Out_ DWORD* const pRequiredSize);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetSeDebugPrivilege32(
	_In_ const HANDLE tokenHandle,
	_Out_ BOOL* const pIsSeDebugPrivilegeEnabled);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessInformation32(
	_In_ const DWORD processId,
	_Out_ PROCESSENTRY32W* const pProcessEntry);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetImageFileName32(
	_In_ const HANDLE processHandle,
	_Out_writes_(bufferLength)
	WCHAR* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetModuleBaseAddress32(
	_In_ const DWORD processId,
	_In_ const wchar_t* const pModuleName,
	_Out_ uintptr_t* const pModuleBaseAddress);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetWindowVisibility32(
	_In_ const DWORD processId,
	_Out_ BOOL* const pIsWindowVisible);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessArchitecture32(
	_In_ const HANDLE processHandle,
	_Out_ USHORT* const pProcessMachine,
	_Out_ USHORT* const pNativeMachine,
	_Out_ BOOL* const pIsWow64);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessModules32(
	_In_ const HANDLE processHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	MODULEENTRY32W* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessThreads32(
	_In_ const HANDLE processHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	THREADENTRY32* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessHandles32(
	_In_ const HANDLE processHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	PSS_HANDLE_ENTRY* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);
#endif // !MUNINN_DATA_WINDOWS_PROVIDER_32