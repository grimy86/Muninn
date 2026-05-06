#ifndef WINDOWS_MEMORY_PROVIDER_32_H
#define WINDOWS_MEMORY_PROVIDER_32_H

#include "DalConfig.h"

#ifndef NTSTATUS
#define NTSTATUS LONG
#endif // !NTSTATUS

MUNINN_API NTSTATUS MUNINN_CALL
DAL_PatchMemory32(
	_In_ LPVOID const targetAddress,
	_In_ const BYTE* const patchBytes,
	_In_ const SIZE_T size);

// x86 relative detour (5 bytes) for 32-bit processes
MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteRelativeDetour32(
	_In_ LPVOID const targetAddress,
	_In_ LPVOID const detourAddress,
	_In_ const SIZE_T size);

// x86 relative trampoline (5 bytes) for 32-bit processes
MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteRelativeTrampoline32(
	_In_  LPVOID const targetAddress,
	_In_  LPVOID const detourAddress,
	_In_  SIZE_T const size,
	_Out_ LPVOID* const pGatewayOut);

// x86 relative trampoline restore (5 bytes) for 32-bit processes
MUNINN_API NTSTATUS MUNINN_CALL
DAL_RestoreRelativeTrampoline32(
	_In_ LPVOID const targetAddress,
	_In_ LPVOID const gatewayAddress,
	_In_ SIZE_T const size);

// x86 absolute detour (14 bytes) for 32-bit processes
MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteAbsoluteDetour32(
	_In_ LPVOID const targetAddress,
	_In_ LPVOID const detourAddress,
	_In_ const SIZE_T size);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteVTableHook32(
	_In_  LPVOID const  pObject,
	_In_  DWORD const   methodIndex,
	_In_  LPVOID const  hookAddress,
	_Out_ LPVOID* const pOriginalOut);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_RestoreVTableHook32(
	_In_ LPVOID const pObject,
	_In_ DWORD const  methodIndex,
	_In_ LPVOID const originalAddress);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteVTableInlineHook32(
	_In_  LPVOID const  vTableAddress,
	_In_  DWORD const   methodIndex,
	_In_  LPVOID const  hookAddress,
	_Out_ LPVOID* const pOriginalOut);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_RestoreVTableInlineHook32(
	_In_ LPVOID const vTableAddress,
	_In_ DWORD const methodIndex,
	_In_ LPVOID const originalAddress);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteIATHook32(
	_In_  const WCHAR* const moduleName,
	_In_  const WCHAR* const functionName,
	_In_  LPVOID const hookAddress,
	_Out_ LPVOID* const pOriginalOut);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_RestoreIATHook32(
	_In_ LPCWSTR const moduleName,
	_In_ LPCWSTR const functionName,
	_In_ LPVOID const originalAddress);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteHWBP32(
	_In_ LPVOID const targetAddress,
	_In_ DWORD const condition,
	_In_ PVECTORED_EXCEPTION_HANDLER const veHandler);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_RemoveHWBP32(
	_In_ LPVOID const targetAddress,
	_In_ PVECTORED_EXCEPTION_HANDLER const veHandler);

#endif // !WINDOWS_MEMORY_PROVIDER_32_H