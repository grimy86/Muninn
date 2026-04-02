#include "WindowsProvider32.h"
#include "DataUtilities.h"

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

#ifndef SE_DEBUG_NAME_W
#define SE_DEBUG_NAME_W L"SeDebugPrivilege"
#endif // !SE_DEBUG_NAME_W

#ifndef SUSPEND_THREAD_ERROR
#define SUSPEND_THREAD_ERROR -1
#endif // !SUSPEND_THREAD_ERROR

#ifndef RESUME_THREAD_ERROR
#define RESUME_THREAD_ERROR -1
#endif // !RESUME_THREAD_ERROR

#ifndef PREALLOC_HANDLES
#define PREALLOC_HANDLES 1000
#endif // !PREALLOC_HANDLES

#ifndef MAX_PATH_LONG
#define MAX_PATH_LONG 32768
#endif // !MAX_PATH_LONG

#ifndef WAIT_TIME
#define WAIT_TIME 5000ul // Expressed in milliseconds
#endif // !WAIT_TIME

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteVirtualMemory32(
	HANDLE processHandle,
	uintptr_t address,
	const void* value,
	SIZE_T size)
{
	SIZE_T bytesWritten = 0ull;
	if (!WriteProcessMemory(
		processHandle,
		(LPVOID)address,
		value,
		size,
		&bytesWritten))
		return STATUS_UNSUCCESSFUL;

	if (bytesWritten != size)
		return STATUS_PARTIAL_COPY;

	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_ReadVirtualMemory32(
	HANDLE processHandle,
	uintptr_t address,
	void* out,
	SIZE_T size)
{
	SIZE_T bytesRead = 0ull;
	if (!ReadProcessMemory(
		processHandle,
		(LPCVOID)address,
		out,
		size,
		&bytesRead))
		return STATUS_UNSUCCESSFUL;

	if (bytesRead != size)
		return STATUS_PARTIAL_COPY;

	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessId32(
	_In_ const WCHAR* const processName,
	_Out_ DWORD* const pProcessId,
	_Out_ BOOL* const pIsRunning)
{
	if (processName == NULL)
		return STATUS_INVALID_PARAMETER_1;
	if (pProcessId == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pIsRunning == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pProcessId = 0ul;
	*pIsRunning = FALSE;

	PROCESSENTRY32W pEntry32W = { 0 };
	pEntry32W.dwSize = sizeof(pEntry32W);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS,
		0ul);

	if (!DAL_IsValidHandle(hSnapshot))
		return STATUS_INVALID_HANDLE;

	if (!Process32First(hSnapshot, &pEntry32W))
	{
		CloseHandle(hSnapshot);
		return STATUS_UNSUCCESSFUL;
	}

	do
	{
		// Case insensitive widestring comparison.
		if (_wcsicmp(pEntry32W.szExeFile, processName) == 0) {
			*pProcessId = pEntry32W.th32ProcessID;
			*pIsRunning = TRUE;
			break;
		}
	} while (Process32Next(hSnapshot, &pEntry32W));

	CloseHandle(hSnapshot);

	return (*pProcessId != 0ul) ?
		STATUS_SUCCESS :
		STATUS_NOT_FOUND;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_OpenProcessHandle32(
	_In_ const DWORD processId,
	_In_ const ACCESS_MASK accessMask,
	_Out_ HANDLE* const pHandle)
{
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_1;
	if (pHandle == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pHandle = OpenProcess(
		accessMask,
		FALSE,
		processId);

	if (!DAL_IsValidHandle(*pHandle))
		return STATUS_INVALID_HANDLE;

	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_CloseHandle32(_In_ const HANDLE handle)
{
	if (!DAL_IsValidHandle(handle))
		return STATUS_INVALID_PARAMETER_1;

	return CloseHandle(handle) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_OpenTokenHandle32(
	_In_ const HANDLE processHandle,
	_In_ const ACCESS_MASK accessMask,
	_Out_ HANDLE* const pTokenHandle)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pTokenHandle == NULL)
		return STATUS_INVALID_PARAMETER_3;

	NTSTATUS status = OpenProcessToken(
		processHandle,
		accessMask,
		pTokenHandle) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (!NT_SUCCESS(status))
		*pTokenHandle = NULL;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SetSeDebugPrivilege32()
{
	HANDLE tokenHandle = NULL;
	NTSTATUS status = DAL_OpenTokenHandle32(
		GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&tokenHandle);

	if (!NT_SUCCESS(status))
		return status;

	if (!DAL_IsValidHandle(tokenHandle))
		return STATUS_INVALID_HANDLE;

	LUID luid = { 0ul, 0l };
	status = LookupPrivilegeValueW(
		NULL,
		SE_DEBUG_NAME_W,
		&luid) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (!NT_SUCCESS(status))
	{
		DAL_CloseHandle32(tokenHandle);
		return status;
	}

	if (!DAL_IsValidLuid(luid))
	{
		DAL_CloseHandle32(tokenHandle);
		return STATUS_UNSUCCESSFUL;
	}

	TOKEN_PRIVILEGES privileges = { 0 };
	privileges.PrivilegeCount = 1;
	privileges.Privileges[0].Luid = luid;
	privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	status = AdjustTokenPrivileges(
		tokenHandle,
		FALSE,
		&privileges,
		sizeof(TOKEN_PRIVILEGES),
		NULL,
		NULL) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (NT_SUCCESS(status))
	{
		DAL_CloseHandle32(tokenHandle);
		return status;
	}

	// privilege missing
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		DAL_CloseHandle32(tokenHandle);
		return STATUS_UNSUCCESSFUL;
	}

	DAL_CloseHandle32(tokenHandle);
	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SetRemoteSeDebugPrivilege32(
	_In_ const HANDLE tokenHandle)
{
	// required: TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
	if (!DAL_IsValidHandle(tokenHandle))
		return STATUS_INVALID_HANDLE;

	LUID luid = { 0ul, 0l };
	NTSTATUS status = LookupPrivilegeValueW(
		NULL,
		SE_DEBUG_NAME_W,
		&luid) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (!NT_SUCCESS(status))
		return status;

	if (!DAL_IsValidLuid(luid))
		return STATUS_UNSUCCESSFUL;

	TOKEN_PRIVILEGES privileges = { 0 };
	privileges.PrivilegeCount = 1;
	privileges.Privileges[0].Luid = luid;
	privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	status = AdjustTokenPrivileges(
		tokenHandle,
		FALSE,
		&privileges,
		sizeof(TOKEN_PRIVILEGES),
		NULL,
		NULL) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (NT_SUCCESS(status))
		return status;

	// privilege missing
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
		return STATUS_UNSUCCESSFUL;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SetThreadPriority32(_In_ const DWORD priorityClass)
{
	return SetPriorityClass(
		GetCurrentProcess(),
		priorityClass) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SetThreadSuspended32(_In_ const DWORD threadId)
{
	HANDLE threadHandle = OpenThread(
		THREAD_SUSPEND_RESUME,
		FALSE,
		threadId);

	if (!DAL_IsValidHandle(threadHandle))
		return STATUS_INVALID_HANDLE;

	DWORD suspendCount =
		SuspendThread(threadHandle);

	if (suspendCount == SUSPEND_THREAD_ERROR)
		return STATUS_UNSUCCESSFUL;

	DAL_CloseHandle32(threadHandle);
	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SetThreadResumed32(_In_ const DWORD threadId)
{
	HANDLE threadHandle = OpenThread(
		THREAD_SUSPEND_RESUME,
		FALSE,
		threadId);

	if (!DAL_IsValidHandle(threadHandle))
		return STATUS_INVALID_HANDLE;

	DWORD suspendCount =
		ResumeThread(threadHandle);

	if (suspendCount == RESUME_THREAD_ERROR)
		return STATUS_UNSUCCESSFUL;

	DAL_CloseHandle32(threadHandle);
	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetThreadPriority32(
	_In_ const HANDLE threadHandle,
	_Out_ INT* const pThreadPriority)
{
	if (!DAL_IsValidHandle(threadHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pThreadPriority == NULL)
		return STATUS_INVALID_PARAMETER_2;

	SetLastError(ERROR_SUCCESS);
	*pThreadPriority
		= GetThreadPriority(threadHandle);

	if (*pThreadPriority == THREAD_PRIORITY_ERROR_RETURN &&
		GetLastError() != ERROR_SUCCESS)
		return STATUS_UNSUCCESSFUL;

	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetTokenInfoBufferSize32(
	_In_ const HANDLE tokenHandle,
	_In_ const TOKEN_INFORMATION_CLASS infoClass,
	_Out_ DWORD* const pRequiredSize)
{
	if (!DAL_IsValidHandle(tokenHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pRequiredSize == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pRequiredSize = 0ul;

	NTSTATUS status = GetTokenInformation(
		tokenHandle,
		infoClass,
		NULL,
		0,
		pRequiredSize) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (!NT_SUCCESS(status))
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			return STATUS_BUFFER_TOO_SMALL;
		else return status;
	}

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetSeDebugPrivilege32(
	_In_ const HANDLE tokenHandle,
	_Out_ BOOL* const pIsSeDebugPrivilegeEnabled)
{
	if (!DAL_IsValidHandle(tokenHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pIsSeDebugPrivilegeEnabled == NULL)
		return STATUS_INVALID_PARAMETER_2;

	*pIsSeDebugPrivilegeEnabled = FALSE;

	DWORD requiredBufferSize = 0ul;
	NTSTATUS status = DAL_GetTokenInfoBufferSize32(
		tokenHandle,
		TokenPrivileges,
		&requiredBufferSize);

	if (!NT_SUCCESS(status))
		return status;

	if (!requiredBufferSize)
		return STATUS_UNSUCCESSFUL;

	BYTE* privilegesBuffer =
		(BYTE*)malloc(requiredBufferSize);
	if (privilegesBuffer == NULL)
		return STATUS_NO_MEMORY;

	status = GetTokenInformation(
		tokenHandle,
		TokenPrivileges,
		privilegesBuffer,
		requiredBufferSize,
		&requiredBufferSize) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	PTOKEN_PRIVILEGES pTokenPrivileges =
		(PTOKEN_PRIVILEGES)(privilegesBuffer);

	LUID debugLuid = { 0ul, 0l };
	status = LookupPrivilegeValueW(
		NULL,
		SE_DEBUG_NAME_W,
		&debugLuid) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (!NT_SUCCESS(status))
	{
		free(privilegesBuffer);
		return status;
	}

	if (!DAL_IsValidLuid(debugLuid))
	{
		free(privilegesBuffer);
		return STATUS_UNSUCCESSFUL;
	}

	status = STATUS_NOT_FOUND;
	for (DWORD i = 0ul; i < pTokenPrivileges->PrivilegeCount; ++i)
	{
		LUID_AND_ATTRIBUTES* pLaa =
			&pTokenPrivileges->Privileges[i];

		if (pLaa->Luid.LowPart == debugLuid.LowPart &&
			pLaa->Luid.HighPart == debugLuid.HighPart)
		{
			*pIsSeDebugPrivilegeEnabled =
				(pLaa->Attributes & SE_PRIVILEGE_ENABLED) != FALSE;

			status = STATUS_SUCCESS;
			break;
		}
	}

	free(privilegesBuffer);
	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessInformation32(
	_In_ const DWORD processId,
	_Out_ PROCESSENTRY32W* const pProcessEntry)
{
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_1;
	if (pProcessEntry == NULL)
		return STATUS_INVALID_PARAMETER_2;

	memset(pProcessEntry, 0, sizeof(*pProcessEntry));
	pProcessEntry->dwSize = sizeof(PROCESSENTRY32W);

	HANDLE snapshotHandle = CreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS,
		0);

	if (!DAL_IsValidHandle(snapshotHandle))
		return STATUS_INVALID_HANDLE;

	if (!Process32FirstW(snapshotHandle, pProcessEntry))
	{
		DAL_CloseHandle32(snapshotHandle);
		return STATUS_UNSUCCESSFUL;
	}

	do
	{
		if (pProcessEntry->th32ProcessID == processId)
		{
			DAL_CloseHandle32(snapshotHandle);
			return STATUS_SUCCESS;
		}
	} while (Process32NextW(snapshotHandle, pProcessEntry));

	DAL_CloseHandle32(snapshotHandle);
	return STATUS_NOT_FOUND;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetImageFileName32(
	_In_ const HANDLE processHandle,
	_Out_writes_(bufferLength)
	WCHAR* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pBuffer == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_4;

	memset(pBuffer, 0, bufferLength * sizeof(WCHAR));
	*pCopiedLength = 0ul;

	DWORD length = bufferLength;
	NTSTATUS status = QueryFullProcessImageNameW(
		processHandle,
		0ul,
		pBuffer,
		&length) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (!NT_SUCCESS(status))
		return status;

	*pCopiedLength = length;
	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetModuleBaseAddress32(
	_In_ const DWORD processId,
	_In_ const wchar_t* const pModuleName,
	_Out_ uintptr_t* const pModuleBaseAddress)
{
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_1;
	if (pModuleName == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pModuleBaseAddress == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pModuleBaseAddress = 0ull;

	MODULEENTRY32W moduleEntry = { 0 };
	moduleEntry.dwSize = sizeof(MODULEENTRY32W);

	HANDLE snapshotHandle = CreateToolhelp32Snapshot(
		TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
		processId);

	if (!DAL_IsValidHandle(snapshotHandle))
		return STATUS_INVALID_HANDLE;

	if (!Module32FirstW(snapshotHandle, &moduleEntry))
	{
		DAL_CloseHandle32(snapshotHandle);
		return STATUS_UNSUCCESSFUL;
	}

	do
	{
		if (_wcsicmp(moduleEntry.szModule, pModuleName) == 0)
		{
			*pModuleBaseAddress =
				(uintptr_t)(moduleEntry.modBaseAddr);

			DAL_CloseHandle32(snapshotHandle);
			return STATUS_SUCCESS;
		}
	} while (Module32NextW(snapshotHandle, &moduleEntry));

	DAL_CloseHandle32(snapshotHandle);
	return STATUS_NOT_FOUND;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetWindowVisibility32(
	_In_ const DWORD processId,
	_Out_ BOOL* const pIsWindowVisible)
{
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_1;
	if (pIsWindowVisible == NULL)
		return STATUS_INVALID_PARAMETER_2;

	*pIsWindowVisible = FALSE;

	for (HWND hwnd = GetTopWindow(NULL); hwnd; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT))
	{
		DWORD windowThreadProcessId = 0ul;
		GetWindowThreadProcessId(hwnd, &windowThreadProcessId);

		if (windowThreadProcessId == processId && IsWindowVisible(hwnd))
		{
			*pIsWindowVisible = TRUE;
			return STATUS_SUCCESS;
		}
	}

	return STATUS_NOT_FOUND;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessArchitecture32(
	_In_ const HANDLE processHandle,
	_Out_ USHORT* const pProcessMachine,
	_Out_ USHORT* const pNativeMachine,
	_Out_ BOOL* const pIsWow64)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pProcessMachine == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pNativeMachine == NULL)
		return STATUS_INVALID_PARAMETER_3;
	if (pIsWow64 == NULL)
		return STATUS_INVALID_PARAMETER_4;

	*pProcessMachine = IMAGE_FILE_MACHINE_UNKNOWN;
	*pNativeMachine = IMAGE_FILE_MACHINE_UNKNOWN;
	*pIsWow64 = FALSE;

	NTSTATUS status = IsWow64Process2(
		processHandle,
		// IMAGE_FILE_MACHINE_UNKNOWN if not a WOW64 process
		pProcessMachine,
		// Native architecture of host system
		pNativeMachine) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (!NT_SUCCESS(status))
		return status;

	*pIsWow64 =
		*pProcessMachine != IMAGE_FILE_MACHINE_UNKNOWN;

	// If running under WOW64, processMachine already contains the guest architecture.
	// Otherwise processMachine is IMAGE_FILE_MACHINE_UNKNOWN, so use the native machine.
	*pProcessMachine = *pIsWow64 ?
		*pProcessMachine :
		*pNativeMachine;

	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessModules32(
	_In_ const HANDLE processHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	MODULEENTRY32W* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_2;
	if (pBuffer == NULL)
		return STATUS_INVALID_PARAMETER_3;
	if (bufferLength == 0ul)
		return STATUS_BUFFER_TOO_SMALL;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_5;

	memset(pBuffer, 0, bufferLength * sizeof(MODULEENTRY32W));
	for (DWORD i = 0ul; i < bufferLength; ++i)
		pBuffer[i].dwSize = sizeof(MODULEENTRY32W);

	*pCopiedLength = 0ul;

	HANDLE snapshotHandle = CreateToolhelp32Snapshot(
		TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
		processId);

	if (!DAL_IsValidHandle(snapshotHandle))
		return STATUS_INVALID_HANDLE;

	MODULEENTRY32W moduleEntry = { 0 };
	moduleEntry.dwSize = sizeof(MODULEENTRY32W);

	NTSTATUS status = Module32FirstW(
		snapshotHandle,
		&moduleEntry) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (!NT_SUCCESS(status))
	{
		DAL_CloseHandle32(snapshotHandle);
		return status;
	}

	DWORD count = 0ul;
	do
	{
		if (count >= bufferLength)
		{
			DAL_CloseHandle32(snapshotHandle);
			*pCopiedLength = count;
			return STATUS_BUFFER_TOO_SMALL;
		}

		pBuffer[count++] = moduleEntry;

	} while (Module32NextW(snapshotHandle, &moduleEntry));

	DAL_CloseHandle32(snapshotHandle);
	*pCopiedLength = count;
	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessThreads32(
	_In_ const HANDLE processHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	THREADENTRY32* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_2;
	if (pBuffer == NULL)
		return STATUS_INVALID_PARAMETER_3;
	if (bufferLength == 0ul)
		return STATUS_BUFFER_TOO_SMALL;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_5;

	memset(pBuffer, 0, bufferLength * sizeof(THREADENTRY32));
	for (DWORD i = 0ul; i < bufferLength; ++i)
		pBuffer[i].dwSize = sizeof(THREADENTRY32);

	*pCopiedLength = 0ul;

	// The processId parameter is technically ignored for TH32CS_SNAPTHREAD.
	HANDLE snapshotHandle = CreateToolhelp32Snapshot(
		TH32CS_SNAPTHREAD,
		processId);

	if (!DAL_IsValidHandle(snapshotHandle))
		return STATUS_INVALID_HANDLE;

	THREADENTRY32 threadEntry = { 0 };
	threadEntry.dwSize = sizeof(THREADENTRY32);
	NTSTATUS status = Thread32First(
		snapshotHandle,
		&threadEntry) ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (!NT_SUCCESS(status))
	{
		DAL_CloseHandle32(snapshotHandle);
		return status;
	}

	DWORD count = 0ul;
	do
	{
		if (threadEntry.th32OwnerProcessID == processId)
		{
			if (count >= bufferLength)
			{
				DAL_CloseHandle32(snapshotHandle);
				*pCopiedLength = count;
				return STATUS_BUFFER_TOO_SMALL;
			}
		}

		pBuffer[count++] = threadEntry;

	} while (Thread32Next(snapshotHandle, &threadEntry));

	DAL_CloseHandle32(snapshotHandle);
	*pCopiedLength = count;
	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessHandles32(
	_In_ const HANDLE processHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	PSS_HANDLE_ENTRY* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_2;
	if (pBuffer == NULL)
		return STATUS_INVALID_PARAMETER_3;
	if (bufferLength == 0ul)
		return STATUS_BUFFER_TOO_SMALL;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_5;

	memset(pBuffer, 0, bufferLength * sizeof(PSS_HANDLE_ENTRY));
	*pCopiedLength = 0ul;

	HPSS pssSnapshotHandle = { 0 };
	if (PssCaptureSnapshot(
		processHandle,
		PSS_CAPTURE_HANDLES |
		PSS_CAPTURE_HANDLE_NAME_INFORMATION |
		PSS_CAPTURE_HANDLE_BASIC_INFORMATION |
		PSS_CAPTURE_HANDLE_TYPE_SPECIFIC_INFORMATION |
		PSS_CAPTURE_HANDLE_TRACE,
		0,
		&pssSnapshotHandle)
		!= ERROR_SUCCESS)
		return STATUS_UNSUCCESSFUL;

	HPSSWALK walkMarkerHandle = { 0 };
	if (PssWalkMarkerCreate(NULL, &walkMarkerHandle)
		!= ERROR_SUCCESS)
	{
		PssFreeSnapshot(GetCurrentProcess(), pssSnapshotHandle);
		return STATUS_UNSUCCESSFUL;
	}

	DWORD count = 0ul;
	while (true)
	{
		PSS_HANDLE_ENTRY handleEntry = { 0 };
		DWORD walkStatus = PssWalkSnapshot(
			pssSnapshotHandle,
			PSS_WALK_HANDLES,
			walkMarkerHandle,
			&handleEntry,
			sizeof(handleEntry));

		if (walkStatus == ERROR_NO_MORE_ITEMS)
			break;
		if (walkStatus != ERROR_SUCCESS)
			break;

		if (count >= bufferLength)
		{
			*pCopiedLength = count;
			PssWalkMarkerFree(walkMarkerHandle);
			PssFreeSnapshot(GetCurrentProcess(), pssSnapshotHandle);
			return STATUS_BUFFER_TOO_SMALL;
		}

		pBuffer[count++] = handleEntry;
	}

	*pCopiedLength = count;
	PssWalkMarkerFree(walkMarkerHandle);
	PssFreeSnapshot(GetCurrentProcess(), pssSnapshotHandle);
	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SimpleDLLInjectA32(
	_In_ const HANDLE processHandle,
	_In_ const CHAR* const dllPath,
	_Out_ HMODULE* pModuleHandle)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (dllPath == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pModuleHandle == NULL)
		return STATUS_INVALID_PARAMETER_3;

	SIZE_T size =
		(strlen(dllPath) + 1) * // Ensures null-termination
		sizeof(CHAR);

	// Allocate memory inside the remote process
	void* location = VirtualAllocEx(
		processHandle,
		NULL,
		size,
		MEM_COMMIT | MEM_RESERVE, // Commit and reserve memory.
		PAGE_READWRITE);

	if (location == NULL)
		return STATUS_NO_MEMORY;

	NTSTATUS status = DAL_WriteVirtualMemory32(
		processHandle,
		(uintptr_t)location,
		dllPath,
		size);

	if (!NT_SUCCESS(status))
	{
		VirtualFreeEx(
			processHandle,
			location,
			0ull,
			MEM_RELEASE);

		return status;
	}

	HANDLE threadHandle = CreateRemoteThread(
		processHandle,
		NULL,
		0ull,
		(LPTHREAD_START_ROUTINE)LoadLibraryA, // StartAddress
		location, // Pointer to the remote DLL path string
		0ul,
		NULL);

	if (!DAL_IsValidHandle(threadHandle))
	{
		VirtualFreeEx(
			processHandle,
			location,
			0ull,
			MEM_RELEASE);

		return STATUS_INVALID_HANDLE;
	}

	// Wait for injection
	WaitForSingleObject(threadHandle, WAIT_TIME);

	DWORD exitCode = 0ul;

	status = GetExitCodeThread(
		threadHandle,
		&exitCode) == TRUE ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (!NT_SUCCESS(status))
	{
		DAL_CloseHandle32(threadHandle);

		VirtualFreeEx(
			processHandle,
			location,
			0ull,
			MEM_RELEASE);

		return status;
	}

	// Cleanup
	DAL_CloseHandle32(threadHandle);
	VirtualFreeEx(
		processHandle,
		location,
		0ull,
		MEM_RELEASE);

	if (exitCode == 0ul)
	{
		*pModuleHandle = NULL;
		return STATUS_UNSUCCESSFUL;
	}

	*pModuleHandle = (HMODULE)exitCode;
	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_SimpleDLLInjectW32(
	_In_ const HANDLE processHandle,
	_In_ const WCHAR* const dllPath,
	_Out_ HMODULE* pModuleHandle)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (dllPath == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pModuleHandle == NULL)
		return STATUS_INVALID_PARAMETER_3;

	SIZE_T size =
		(wcslen(dllPath) + 1) * // Ensures null-termination
		sizeof(WCHAR);

	// Allocate memory inside the remote process
	void* location = VirtualAllocEx(
		processHandle,
		NULL,
		size,
		MEM_COMMIT | MEM_RESERVE, // Commit and reserve memory.
		PAGE_READWRITE);

	if (location == NULL)
		return STATUS_NO_MEMORY;

	NTSTATUS status = DAL_WriteVirtualMemory32(
		processHandle,
		(uintptr_t)location,
		dllPath,
		size);

	if (!NT_SUCCESS(status))
	{
		VirtualFreeEx(
			processHandle,
			location,
			0ull,
			MEM_RELEASE);

		return status;
	}

	HANDLE threadHandle = CreateRemoteThread(
		processHandle,
		NULL,
		0ull,
		(LPTHREAD_START_ROUTINE)LoadLibraryA, // StartAddress
		location, // Pointer to the remote DLL path string
		0ul,
		NULL);

	if (!DAL_IsValidHandle(threadHandle))
	{
		VirtualFreeEx(
			processHandle,
			location,
			0ull,
			MEM_RELEASE);

		return STATUS_INVALID_HANDLE;
	}

	// Wait for injection
	WaitForSingleObject(threadHandle, WAIT_TIME);

	DWORD exitCode = 0ul;

	status = GetExitCodeThread(
		threadHandle,
		&exitCode) == TRUE ?
		STATUS_SUCCESS :
		STATUS_UNSUCCESSFUL;

	if (!NT_SUCCESS(status))
	{
		DAL_CloseHandle32(threadHandle);

		VirtualFreeEx(
			processHandle,
			location,
			0ull,
			MEM_RELEASE);

		return status;
	}

	// Cleanup
	DAL_CloseHandle32(threadHandle);
	VirtualFreeEx(
		processHandle,
		location,
		0ull,
		MEM_RELEASE);

	if (exitCode == 0ul)
	{
		*pModuleHandle = NULL;
		return STATUS_UNSUCCESSFUL;
	}

	*pModuleHandle = (HMODULE)exitCode;
	return STATUS_SUCCESS;
}