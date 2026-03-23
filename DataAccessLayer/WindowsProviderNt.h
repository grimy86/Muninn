#ifndef WINDOWS_PROVIDER_NT_H
#define WINDOWS_PROVIDER_NT_H

#include "apiconfig.h"
#include <phnt_windows.h>
#include <phnt.h>

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteVirtualMemoryNt(
	_In_ const HANDLE processHandle,
	_In_ const uintptr_t address,
	_In_ const void* const value,
	_In_ const SIZE_T size);

MUNINN_API NTSTATUS MUNINN_CALL
DAL_ReadVirtualMemoryNt(
	_In_ const HANDLE processHandle,
	_In_ const uintptr_t address,
	_Out_ void* const out,
	_In_ const SIZE_T size);

/// <summary>
/// Opens a handle to a process.
/// </summary>
/// <param name="processId"> Target process identifier. </param>
/// <param name="accessMask"> Desired access rights. </param>
/// <param name="pHandle"> Receives the process handle. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_OpenProcessHandleNt(
	_In_ const DWORD processId,
	_In_ const ACCESS_MASK accessMask,
	_Out_ HANDLE* const pHandle);

/// <summary>
/// Closes a handle.
/// </summary>
/// <param name="handle"> Handle to close. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_CloseHandleNt(_In_ const HANDLE handle);

/// <summary>
/// Duplicates a handle from another process into the current process.
/// </summary>
/// <param name="sourceHandle"> Handle in the source process. </param>
/// <param name="processId"> Identifier of the process that owns the handle. </param>
/// <param name="pDuplicatedHandle"> Receives the duplicated handle. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_DuplicateHandleNt(
	_In_ const HANDLE sourceHandle,
	_In_ const DWORD processId,
	_Out_ HANDLE* const pDuplicatedHandle);

/// <summary>
/// Opens the access token associated with a process.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="accessMask"> Desired token access rights. </param>
/// <param name="pTokenHandle"> Receives the token handle. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_OpenProcessTokenHandleNt(
	_In_ const HANDLE processHandle,
	_In_ const ACCESS_MASK accessMask,
	_Out_ HANDLE* const pTokenHandle);

/// <summary>
/// Combines the components of a LUID into a 64-bit value.
/// </summary>
/// <param name="luid"> Source LUID structure. </param>
/// <param name="pFullLuid"> Receives the combined value. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetFullLuidNt(
	_In_ const LUID luid,
	_Out_ uint64_t* const pFullLuid);

/// <summary>
/// Retrieves the buffer size required for NtQuerySystemInformation.
/// </summary>
/// <param name="infoClass"> System information class. </param>
/// <param name="pRequiredBufferSize"> Receives the required buffer size. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetQSIBufferSizeNt(
	_In_ const SYSTEM_INFORMATION_CLASS infoClass,
	_Out_ DWORD* const pRequiredBufferSize);

/// <summary>
/// Retrieves the buffer size required for NtQueryObject.
/// </summary>
/// <param name="duplicatedHandle"> Handle to query. </param>
/// <param name="infoClass"> Object information class. </param>
/// <param name="pRequiredBufferSize"> Receives the required buffer size. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetQOBufferSizeNt(
	_In_ const HANDLE duplicatedHandle,
	_In_ const OBJECT_INFORMATION_CLASS infoClass,
	_Out_ DWORD* const pRequiredBufferSize);

/// <summary>
/// Retrieves the buffer size required for NtQueryInformationToken.
/// </summary>
/// <param name="tokenHandle"> Token handle. </param>
/// <param name="infoClass"> Token information class. </param>
/// <param name="pRequiredBufferSize"> Receives the required buffer size. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetQITBufferSizeNt(
	_In_ const HANDLE tokenHandle,
	_In_ const TOKEN_INFORMATION_CLASS infoClass,
	_Out_ DWORD* const pRequiredBufferSize);

/// <summary>
/// Retrieves the name of a kernel object referenced by a handle.
/// </summary>
/// <param name="sourceHandle"> Handle in the source process. </param>
/// <param name="processId"> Process that owns the handle. </param>
/// <param name="pBuffer"> Destination string buffer. </param>
/// <param name="bufferLength"> Buffer length in characters. </param>
/// <param name="pCopiedLength"> Receives number of characters written. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetObjectNameNt(
	_In_ const HANDLE sourceHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	WCHAR* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);

/// <summary>
/// Retrieves the type name of a kernel object referenced by a handle.
/// </summary>
/// <param name="sourceHandle"> Handle in the source process. </param>
/// <param name="processId"> Process that owns the handle. </param>
/// <param name="pBuffer"> Destination string buffer. </param>
/// <param name="bufferLength"> Buffer length in characters. </param>
/// <param name="pCopiedLength"> Receives number of characters written. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetObjectTypeNameNt(
	_In_ const HANDLE sourceHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	WCHAR* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);

/// <summary>
/// Reads a UNICODE_STRING structure from a remote process.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pRemoteUnicodeString"> Address of the remote UNICODE_STRING. </param>
/// <param name="pBuffer"> Destination string buffer. </param>
/// <param name="bufferLength"> Buffer length in characters. </param>
/// <param name="pCopiedLength"> Receives number of characters written. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetRemoteUnicodeStringNt(
	_In_ const HANDLE processHandle,
	_In_ const UNICODE_STRING* const pRemoteUnicodeString,
	_Out_writes_(bufferLength)
	WCHAR* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);

/// <summary>
/// Retrieves information about all processes in the system.
/// </summary>
/// <param name="ppBuffer">
/// Receives a pointer to a heap-allocated buffer containing the 
/// full SYSTEM_PROCESS_INFORMATION list. The caller is responsible
/// for freeing this buffer with free().
/// </param>
/// <param name="pSize"> Receives the size, in bytes, of the returned buffer. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetSystemProcessInformationNt(
	_Out_ BYTE** const ppBuffer,
	_Out_ DWORD* const pSize);

/// <summary>
/// Retrieves extended information about a process.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pProcessInfo"> Receives the process information structure. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessInformationNt(
	_In_ const HANDLE processHandle,
	_Out_ PROCESS_EXTENDED_BASIC_INFORMATION* const pProcessInfo);

/// <summary>
/// Retrieves the image file name of a process.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pBuffer"> Destination buffer. </param>
/// <param name="bufferLength"> Buffer length. </param>
/// <param name="pCopiedLength"> Receives number of characters written. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetImageFileNameNt(
	_In_ const HANDLE processHandle,
	_Out_writes_(bufferLength)
	WCHAR* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);

/// <summary>
/// Retrieves the Win32 image path of a process.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pBuffer"> Destination buffer. </param>
/// <param name="bufferLength"> Buffer length. </param>
/// <param name="pCopiedLength"> Receives number of characters written. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetImageFileNameWin32Nt(
	_In_ const HANDLE processHandle,
	_Out_writes_(bufferLength)
	WCHAR* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);

/// <summary>
/// Retrieves the address of the Process Environment Block (PEB).
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pPebBaseAddress"> Receives the PEB address. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetPebBaseAddressNt(
	_In_ const HANDLE processHandle,
	_Out_ uintptr_t* const pPebBaseAddress);

/// <summary>
/// Retrieves the PEB address from process information.
/// </summary>
/// <param name="pProcessInfo"> Process information structure. </param>
/// <param name="pPebBaseAddress"> Receives the PEB address. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetPebBaseAddressFromProcessInfoNt(
	_In_ const PROCESS_EXTENDED_BASIC_INFORMATION* const pProcessInfo,
	_Out_ uintptr_t* const pPebBaseAddress);

/// <summary>
/// Retrieves the PEB address and process information.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pPebBaseAddress"> Receives the PEB address. </param>
/// <param name="pProcessInfo"> Receives process information. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetPebBaseAddressAndProcessInfoNt(
	_In_ const HANDLE processHandle,
	_Out_ uintptr_t* const pPebBaseAddress,
	_Out_ PROCESS_EXTENDED_BASIC_INFORMATION* const pProcessInfo);

/// <summary>
/// Reads the PEB structure of a process.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pPeb"> Receives the PEB structure. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetPebNt(
	_In_ const HANDLE processHandle,
	_Out_ PEB* const pPeb);

/// <summary>
/// Retrieves both the PEB address and PEB structure.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pPebBaseAddress"> Receives the PEB address. </param>
/// <param name="pPeb"> Receives the PEB structure. </param>
/// <returns></returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetPebAndPebBaseAddressNt(
	_In_ const HANDLE processHandle,
	_Out_ uintptr_t* const pPebBaseAddress,
	_Out_ PEB* const pPeb);

/// <summary>
/// Retrieves the base address of the main module of a process.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pModuleBaseAddress"> Receives the module base address. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetModuleBaseAddressNt(
	_In_ const HANDLE processHandle,
	_Out_ uintptr_t* const pModuleBaseAddress);

/// <summary>
/// Retrieves the base address of the main module using process information.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="processInfo"> Process information containing the PEB address. </param>
/// <param name="pModuleBaseAddress"> Receives the module base address. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetModuleBaseAddressFromProcessInfoNt(
	_In_ const HANDLE processHandle,
	_In_ const PROCESS_EXTENDED_BASIC_INFORMATION* const processInfo,
	_Out_ uintptr_t* const pModuleBaseAddress);

/// <summary>
/// Retrieves the base address of the main module using a PEB address.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pPebBaseAddress"> PEB address. </param>
/// <param name="pModuleBaseAddress"> Receives the module base address. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetModuleBaseAddressFromPebBaseAddressNt(
	_In_ const HANDLE processHandle,
	_In_ const uintptr_t* const pPebBaseAddress,
	_Out_ uintptr_t* const pModuleBaseAddress);

/// <summary>
/// Retrieves the base address of the main module using a PEB structure.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pPeb"> PEB structure. </param>
/// <param name="pModuleBaseAddress"> Receives the module base address. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetModuleBaseAddressFromPebNt(
	_In_ const HANDLE processHandle,
	_In_ const PEB* const pPeb,
	_Out_ uintptr_t* const pModuleBaseAddress);

/// <summary>
/// Retrieves the WOW64 information for a process.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pWow64Info"> Receives the WOW64 information pointer. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetWow64InfoNt(
	_In_ const HANDLE processHandle,
	_Out_ ULONG_PTR* const pWow64Info);

/// <summary>
/// Retrieves modules loaded in a process.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="pPeb"> PEB structure. </param>
/// <param name="pBuffer"> Destination buffer. </param>
/// <param name="bufferLength"> Buffer capacity. </param>
/// <param name="pCopiedLength"> Receives module count. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessModulesNt(
	_In_ const HANDLE processHandle,
	_In_ const PEB* const pPeb,
	_Out_writes_(bufferLength)
	LDR_DATA_TABLE_ENTRY* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);

/// <summary>
/// Retrieves threads belonging to a process.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="processId"> Process identifier. </param>
/// <param name="pBuffer"> Destination buffer. </param>
/// <param name="bufferLength"> Buffer capacity. </param>
/// <param name="pCopiedLength"> Receives thread count. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessThreadsNt(
	_In_ const HANDLE processHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	SYSTEM_THREAD_INFORMATION* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);

/// <summary>
/// Retrieves handles owned by a process.
/// </summary>
/// <param name="processHandle"> Handle to the process. </param>
/// <param name="processId"> Process identifier. </param>
/// <param name="pBuffer"> Destination buffer. </param>
/// <param name="bufferLength"> Buffer capacity. </param>
/// <param name="pCopiedLength"> Receives handle count. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessHandlesNt(
	_In_ const HANDLE processHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	SYSTEM_HANDLE_TABLE_ENTRY_INFO* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);


/// <summary>
/// Retrieves statistics for a process token.
/// </summary>
/// <param name="tokenHandle"> Token handle. </param>
/// <param name="pTokenStatistics"> Receives token statistics. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessTokenStatisticsNt(
	_In_ const HANDLE tokenHandle,
	_Out_ TOKEN_STATISTICS* const pTokenStatistics);

/// <summary>
/// Retrieves privileges associated with a token.
/// </summary>
/// <param name="tokenHandle"> Token handle. </param>
/// <param name="pBuffer"> Destination buffer. </param>
/// <param name="bufferLength"> Buffer capacity. </param>
/// <param name="pCopiedLength"> Receives privilege count. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessTokenPriviligesNt(
	_In_ const HANDLE tokenHandle,
	_Out_writes_(bufferLength)
	LUID_AND_ATTRIBUTES* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength);

/// <summary>
/// Retrieves the session identifier associated with a token.
/// </summary>
/// <param name="tokenHandle"> Token handle. </param>
/// <param name="pSessionId"> Receives the session identifier. </param>
/// <returns> NTSTATUS indicating the result of the operation. </returns>
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessTokenSessionIdNt(
	_In_ const HANDLE tokenHandle,
	_Out_ DWORD* const pSessionId);

#endif // !WINDOWS_PROVIDER_NT_H