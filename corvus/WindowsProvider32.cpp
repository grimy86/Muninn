#include "MemoryService.h"
#include <Psapi.h>
#include <TlHelp32.h>
#include "WindowsProvider32.h"

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

namespace Muninn::Data
{
#pragma region WRITE
	MUNINN_API NTSTATUS MUNINN_CALL
		OpenProcessHandle32(
			_In_ const DWORD processId,
			_In_ const ACCESS_MASK accessMask,
			_Out_ HANDLE* const pHandle) noexcept
	{
		if (!IsValidProcessId(processId))
			return STATUS_INVALID_PARAMETER_1;
		if (pHandle == nullptr)
			return STATUS_INVALID_PARAMETER_3;

		*pHandle = OpenProcess(
			accessMask,
			FALSE,
			processId);

		if (!IsValidHandle(*pHandle))
			return STATUS_INVALID_HANDLE;

		return STATUS_SUCCESS;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		CloseHandle32(_In_ const HANDLE handle) noexcept
	{
		if (!IsValidHandle(handle))
			return STATUS_INVALID_PARAMETER_1;

		return CloseHandle(handle) ?
			STATUS_SUCCESS :
			STATUS_UNSUCCESSFUL;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		OpenTokenHandle32(
			_In_ const HANDLE processHandle,
			_In_ const ACCESS_MASK accessMask,
			_Out_ HANDLE* const pTokenHandle) noexcept
	{
		if (!IsValidHandle(processHandle))
			return STATUS_INVALID_PARAMETER_1;
		if (pTokenHandle == nullptr)
			return STATUS_INVALID_PARAMETER_3;

		NTSTATUS status{ OpenProcessToken(
			processHandle,
			accessMask,
			pTokenHandle) ?
			STATUS_SUCCESS :
			 STATUS_UNSUCCESSFUL };

		if (!NT_SUCCESS(status))
			*pTokenHandle = nullptr;

		return status;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		SetSeDebugPrivilege32() noexcept
	{
		HANDLE tokenHandle{};
		NTSTATUS status{ OpenTokenHandle32(
			GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
			&tokenHandle) };

		if (!NT_SUCCESS(status))
			return status;

		if (!IsValidHandle(tokenHandle))
			return STATUS_INVALID_HANDLE;

		LUID luid{};
		status = LookupPrivilegeValueW(
			nullptr,
			SE_DEBUG_NAME_W,
			&luid) ?
			STATUS_SUCCESS :
			STATUS_UNSUCCESSFUL;

		if (!NT_SUCCESS(status))
		{
			CloseHandle32(tokenHandle);
			return status;
		}

		if (!IsValidLuid(luid))
		{
			CloseHandle32(tokenHandle);
			return STATUS_UNSUCCESSFUL;
		}

		TOKEN_PRIVILEGES privileges{};
		privileges.PrivilegeCount = 1;
		privileges.Privileges[0].Luid = luid;
		privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		status = AdjustTokenPrivileges(
			tokenHandle,
			FALSE,
			&privileges,
			sizeof(TOKEN_PRIVILEGES),
			nullptr,
			nullptr) ?
			STATUS_SUCCESS :
			STATUS_UNSUCCESSFUL;

		if (NT_SUCCESS(status))
		{
			CloseHandle32(tokenHandle);
			return status;
		}

		// privilege missing
		if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
		{
			CloseHandle32(tokenHandle);
			return STATUS_UNSUCCESSFUL;
		}

		CloseHandle32(tokenHandle);
		return status;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		SetRemoteSeDebugPrivilege32(
			_In_ const HANDLE tokenHandle) noexcept
	{
		// required: TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY
		if (!IsValidHandle(tokenHandle))
			return STATUS_INVALID_HANDLE;

		LUID luid{};
		NTSTATUS status{ LookupPrivilegeValueW(
			nullptr,
			SE_DEBUG_NAME_W,
			&luid) ?
			STATUS_SUCCESS :
			STATUS_UNSUCCESSFUL };

		if (!NT_SUCCESS(status))
			return status;

		if (!IsValidLuid(luid))
			return STATUS_UNSUCCESSFUL;

		TOKEN_PRIVILEGES privileges{};
		privileges.PrivilegeCount = 1;
		privileges.Privileges[0].Luid = luid;
		privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		status = AdjustTokenPrivileges(
			tokenHandle,
			FALSE,
			&privileges,
			sizeof(TOKEN_PRIVILEGES),
			nullptr,
			nullptr) ?
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
		SetThreadPriority32(_In_ const DWORD priorityClass) noexcept
	{
		return SetPriorityClass(
			GetCurrentProcess(),
			priorityClass) ?
			STATUS_SUCCESS :
			STATUS_UNSUCCESSFUL;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		SetThreadSuspended32(_In_ const DWORD threadId) noexcept
	{
		HANDLE threadHandle{ OpenThread(
			THREAD_SUSPEND_RESUME,
			FALSE,
			threadId) };

		if (!IsValidHandle(threadHandle))
			return STATUS_INVALID_HANDLE;

		DWORD suspendCount{
		SuspendThread(threadHandle) };

		if (suspendCount == SUSPEND_THREAD_ERROR)
			return STATUS_UNSUCCESSFUL;

		CloseHandle32(threadHandle);
		return STATUS_SUCCESS;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		SetThreadResumed32(_In_ const DWORD threadId) noexcept
	{
		HANDLE threadHandle{ OpenThread(
			THREAD_SUSPEND_RESUME,
			FALSE,
			threadId) };

		if (!IsValidHandle(threadHandle))
			return STATUS_INVALID_HANDLE;

		DWORD suspendCount{
		ResumeThread(threadHandle) };

		if (suspendCount == RESUME_THREAD_ERROR)
			return STATUS_UNSUCCESSFUL;

		CloseHandle32(threadHandle);
		return STATUS_SUCCESS;
	}
#pragma endregion

#pragma region READ
	MUNINN_API NTSTATUS MUNINN_CALL
		GetThreadPriority32(
			_In_ const HANDLE threadHandle,
			_Out_ INT* const pThreadPriority) noexcept
	{
		if (!IsValidHandle(threadHandle))
			return STATUS_INVALID_PARAMETER_1;
		if (pThreadPriority == nullptr)
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
		GetTokenInfoBufferSize32(
			_In_ const HANDLE tokenHandle,
			_In_ const _TOKEN_INFORMATION_CLASS infoClass,
			_Out_ DWORD* const pRequiredSize) noexcept
	{
		if (!IsValidHandle(tokenHandle))
			return STATUS_INVALID_PARAMETER_1;
		if (pRequiredSize == nullptr)
			return STATUS_INVALID_PARAMETER_3;

		*pRequiredSize = 0ul;

		NTSTATUS status{ GetTokenInformation(
			tokenHandle,
			infoClass,
			nullptr,
			0,
			pRequiredSize) ?
			STATUS_SUCCESS :
			STATUS_UNSUCCESSFUL };

		if (!NT_SUCCESS(status))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				return STATUS_BUFFER_TOO_SMALL;
			else return status;
		}

		return status;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		GetSeDebugPrivilege32(
			_In_ const HANDLE tokenHandle,
			_Out_ BOOL* const pIsSeDebugPrivilegeEnabled) noexcept
	{
		if (!IsValidHandle(tokenHandle))
			return STATUS_INVALID_PARAMETER_1;
		if (pIsSeDebugPrivilegeEnabled == nullptr)
			return STATUS_INVALID_PARAMETER_2;

		*pIsSeDebugPrivilegeEnabled = FALSE;

		DWORD requiredBufferSize{};
		NTSTATUS status{ GetTokenInfoBufferSize32(
			tokenHandle,
			TokenPrivileges,
			&requiredBufferSize) };

		if (!NT_SUCCESS(status))
			return status;

		if (!requiredBufferSize)
			return STATUS_UNSUCCESSFUL;

		BYTE* privilegesBuffer{
			new BYTE[requiredBufferSize] };

		status = GetTokenInformation(
			tokenHandle,
			TokenPrivileges,
			privilegesBuffer,
			requiredBufferSize,
			&requiredBufferSize) ?
			STATUS_SUCCESS :
			STATUS_UNSUCCESSFUL;

		PTOKEN_PRIVILEGES pTokenPrivileges{
			reinterpret_cast<PTOKEN_PRIVILEGES>(privilegesBuffer) };

		LUID debugLuid{};
		status = LookupPrivilegeValueW(
			nullptr,
			SE_DEBUG_NAME_W,
			&debugLuid) ?
			STATUS_SUCCESS :
			STATUS_UNSUCCESSFUL;

		if (!NT_SUCCESS(status))
		{
			delete[] privilegesBuffer;
			return status;
		}

		if (!IsValidLuid(debugLuid))
		{
			delete[] privilegesBuffer;
			return STATUS_UNSUCCESSFUL;
		}

		status = STATUS_NOT_FOUND;
		for (DWORD i{}; i < pTokenPrivileges->PrivilegeCount; ++i)
		{
			LUID_AND_ATTRIBUTES& laa{
				pTokenPrivileges->Privileges[i] };

			if (laa.Luid.LowPart == debugLuid.LowPart &&
				laa.Luid.HighPart == debugLuid.HighPart)
			{
				*pIsSeDebugPrivilegeEnabled =
					(laa.Attributes & SE_PRIVILEGE_ENABLED) != FALSE;

				status = STATUS_SUCCESS;
				break;
			}
		}

		delete[] privilegesBuffer;
		return status;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		GetProcessInformation32(
			_In_ const DWORD processId,
			_Out_ PROCESSENTRY32W* const pProcessEntry) noexcept
	{
		if (!IsValidProcessId(processId))
			return STATUS_INVALID_PARAMETER_1;
		if (pProcessEntry == nullptr)
			return STATUS_INVALID_PARAMETER_2;

		*pProcessEntry = {};

		HANDLE snapshotHandle{ CreateToolhelp32Snapshot(
			TH32CS_SNAPPROCESS,
			0) };

		if (!IsValidHandle(snapshotHandle))
			return STATUS_INVALID_HANDLE;


		pProcessEntry->dwSize = sizeof(PROCESSENTRY32W);
		if (!Process32FirstW(snapshotHandle, pProcessEntry))
		{
			CloseHandle32(snapshotHandle);
			return STATUS_UNSUCCESSFUL;
		}

		do
		{
			if (pProcessEntry->th32ProcessID == processId)
			{
				CloseHandle32(snapshotHandle);
				return STATUS_SUCCESS;
			}
		} while (Process32NextW(snapshotHandle, pProcessEntry));

		CloseHandle32(snapshotHandle);
		return STATUS_NOT_FOUND;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		GetImageFileName32(
			_In_ const HANDLE processHandle,
			_Out_writes_(bufferLength)
			WCHAR* const pBuffer,
			_In_ const DWORD bufferLength,
			_Out_ DWORD* const pCopiedLength) noexcept
	{
		if (!IsValidHandle(processHandle))
			return STATUS_INVALID_PARAMETER_1;
		if (pBuffer == nullptr)
			return STATUS_INVALID_PARAMETER_2;
		if (pCopiedLength == nullptr)
			return STATUS_INVALID_PARAMETER_4;

		*pBuffer = L'\0';
		*pCopiedLength = 0ul;

		DWORD length{ bufferLength };
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
		GetModuleBaseAddress32(
			_In_ const DWORD processId,
			_In_ const wchar_t* const pModuleName,
			_Out_ uintptr_t* const pModuleBaseAddress) noexcept
	{
		if (!IsValidProcessId(processId))
			return STATUS_INVALID_PARAMETER_1;
		if (pModuleName == nullptr)
			return STATUS_INVALID_PARAMETER_2;
		if (pModuleBaseAddress == nullptr)
			return STATUS_INVALID_PARAMETER_3;

		*pModuleBaseAddress = 0ull;

		MODULEENTRY32W moduleEntry{};
		moduleEntry.dwSize = sizeof(MODULEENTRY32W);

		HANDLE snapshotHandle{ CreateToolhelp32Snapshot(
			TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
			processId) };

		if (!IsValidHandle(snapshotHandle))
			return STATUS_INVALID_HANDLE;

		if (!Module32FirstW(snapshotHandle, &moduleEntry))
		{
			CloseHandle32(snapshotHandle);
			return STATUS_UNSUCCESSFUL;
		}

		do
		{
			if (_wcsicmp(moduleEntry.szModule, pModuleName) == 0)
			{
				*pModuleBaseAddress
					= reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);

				CloseHandle32(snapshotHandle);
				return STATUS_SUCCESS;
			}
		} while (Module32NextW(snapshotHandle, &moduleEntry));

		CloseHandle32(snapshotHandle);
		return STATUS_NOT_FOUND;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		GetWindowVisibility32(
			_In_ const DWORD processId,
			_Out_ BOOL* const pIsWindowVisible) noexcept
	{
		if (!IsValidProcessId(processId))
			return STATUS_INVALID_PARAMETER_1;
		if (pIsWindowVisible == nullptr)
			return STATUS_INVALID_PARAMETER_2;

		*pIsWindowVisible = FALSE;

		for (HWND hwnd{ GetTopWindow(nullptr) }; hwnd; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT))
		{
			DWORD windowThreadProcessId{};
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
		GetProcessArchitecture32(
			_In_ const HANDLE processHandle,
			_Out_ USHORT* const pProcessMachine,
			_Out_ USHORT* const pNativeMachine,
			_Out_ BOOL* const pIsWow64) noexcept
	{
		if (!IsValidHandle(processHandle))
			return STATUS_INVALID_PARAMETER_1;
		if (pProcessMachine == nullptr)
			return STATUS_INVALID_PARAMETER_2;
		if (pNativeMachine == nullptr)
			return STATUS_INVALID_PARAMETER_3;
		if (pIsWow64 == nullptr)
			return STATUS_INVALID_PARAMETER_4;

		*pProcessMachine = IMAGE_FILE_MACHINE_UNKNOWN;
		*pNativeMachine = IMAGE_FILE_MACHINE_UNKNOWN;
		*pIsWow64 = FALSE;

		NTSTATUS status{ IsWow64Process2(
			processHandle,
			// IMAGE_FILE_MACHINE_UNKNOWN if not a WOW64 process
			pProcessMachine,
			// Native architecture of host system
			pNativeMachine) ?
			STATUS_SUCCESS :
			STATUS_UNSUCCESSFUL };

		if (!NT_SUCCESS(status))
			return status;

		*pIsWow64
			= *pProcessMachine != IMAGE_FILE_MACHINE_UNKNOWN;

		// If running under WOW64, processMachine already contains the guest architecture.
		// Otherwise processMachine is IMAGE_FILE_MACHINE_UNKNOWN, so use the native machine.
		*pProcessMachine = *pIsWow64 ?
			*pProcessMachine :
			*pNativeMachine;

		return STATUS_SUCCESS;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		GetProcessModules32(
			_In_ const HANDLE processHandle,
			_In_ const DWORD processId,
			_Out_writes_(bufferLength)
			MODULEENTRY32W* const pBuffer,
			_In_ const DWORD bufferLength,
			_Out_ DWORD* const pCopiedLength) noexcept
	{
		if (!IsValidHandle(processHandle))
			return STATUS_INVALID_PARAMETER_1;
		if (!IsValidProcessId(processId))
			return STATUS_INVALID_PARAMETER_2;
		if (pBuffer == nullptr)
			return STATUS_INVALID_PARAMETER_3;
		if (bufferLength == 0ul)
			return STATUS_BUFFER_TOO_SMALL;
		if (pCopiedLength == nullptr)
			return STATUS_INVALID_PARAMETER_5;

		*pBuffer = {};
		*pCopiedLength = 0ul;

		HANDLE snapshotHandle{ CreateToolhelp32Snapshot(
			TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
			processId) };

		if (!IsValidHandle(snapshotHandle))
			return STATUS_INVALID_HANDLE;

		MODULEENTRY32W moduleEntry{};
		moduleEntry.dwSize = sizeof(MODULEENTRY32W);

		NTSTATUS status{ Module32FirstW(
			snapshotHandle,
			&moduleEntry) ?
			STATUS_SUCCESS :
			STATUS_UNSUCCESSFUL };

		if (!NT_SUCCESS(status))
		{
			CloseHandle32(snapshotHandle);
			return status;
		}

		DWORD count{};
		do
		{
			if (count >= bufferLength)
			{
				CloseHandle32(snapshotHandle);
				*pCopiedLength = count;
				return STATUS_BUFFER_TOO_SMALL;
			}

			pBuffer[count++] = moduleEntry;

		} while (Module32NextW(snapshotHandle, &moduleEntry));

		CloseHandle32(snapshotHandle);
		*pCopiedLength = count;
		return STATUS_SUCCESS;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		GetProcessThreads32(
			_In_ const HANDLE processHandle,
			_In_ const DWORD processId,
			_Out_writes_(bufferLength)
			THREADENTRY32* const pBuffer,
			_In_ const DWORD bufferLength,
			_Out_ DWORD* const pCopiedLength) noexcept
	{
		if (!IsValidHandle(processHandle))
			return STATUS_INVALID_PARAMETER_1;
		if (!IsValidProcessId(processId))
			return STATUS_INVALID_PARAMETER_2;
		if (pBuffer == nullptr)
			return STATUS_INVALID_PARAMETER_3;
		if (bufferLength == 0ul)
			return STATUS_BUFFER_TOO_SMALL;
		if (pCopiedLength == nullptr)
			return STATUS_INVALID_PARAMETER_5;

		*pBuffer = {};
		*pCopiedLength = 0ul;

		// The processId parameter is technically ignored for TH32CS_SNAPTHREAD.
		HANDLE snapshotHandle{ CreateToolhelp32Snapshot(
			TH32CS_SNAPTHREAD,
			processId) };

		if (!IsValidHandle(snapshotHandle))
			return STATUS_INVALID_HANDLE;

		THREADENTRY32 threadEntry{};
		threadEntry.dwSize = sizeof(THREADENTRY32);
		NTSTATUS status{ Thread32First(
			snapshotHandle,
			&threadEntry) ?
			STATUS_SUCCESS :
			STATUS_UNSUCCESSFUL };

		if (!NT_SUCCESS(status))
		{
			CloseHandle32(snapshotHandle);
			return status;
		}

		DWORD count{};
		do
		{
			if (threadEntry.th32OwnerProcessID != processId)
				continue;

			if (count >= bufferLength)
			{
				CloseHandle32(snapshotHandle);
				*pCopiedLength = count;
				return STATUS_BUFFER_TOO_SMALL;
			}

			pBuffer[count++] = threadEntry;

		} while (Thread32Next(snapshotHandle, &threadEntry));

		CloseHandle32(snapshotHandle);
		*pCopiedLength = count;
		return STATUS_SUCCESS;
	}

	MUNINN_API NTSTATUS MUNINN_CALL
		GetProcessHandles32(
			_In_ const HANDLE processHandle,
			_In_ const DWORD processId,
			_Out_writes_(bufferLength)
			PSS_HANDLE_ENTRY* const pBuffer,
			_In_ const DWORD bufferLength,
			_Out_ DWORD* const pCopiedLength) noexcept
	{
		if (!IsValidHandle(processHandle))
			return STATUS_INVALID_PARAMETER_1;
		if (!IsValidProcessId(processId))
			return STATUS_INVALID_PARAMETER_2;
		if (pBuffer == nullptr)
			return STATUS_INVALID_PARAMETER_3;
		if (bufferLength == 0ul)
			return STATUS_BUFFER_TOO_SMALL;
		if (pCopiedLength == nullptr)
			return STATUS_INVALID_PARAMETER_5;

		*pBuffer = {};
		*pCopiedLength = 0ul;

		HPSS pssSnapshotHandle{};
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

		HPSSWALK walkMarkerHandle{};
		if (PssWalkMarkerCreate(nullptr, &walkMarkerHandle)
			!= ERROR_SUCCESS)
		{
			PssFreeSnapshot(GetCurrentProcess(), pssSnapshotHandle);
			return STATUS_UNSUCCESSFUL;
		}

		DWORD count{};
		while (true)
		{
			PSS_HANDLE_ENTRY handleEntry{};
			DWORD walkStatus{ PssWalkSnapshot(
				pssSnapshotHandle,
				PSS_WALK_HANDLES,
				walkMarkerHandle,
				&handleEntry,
				sizeof(handleEntry)) };

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
#pragma endregion
}

/*
BOOL GetProcessInformationObject32(
	const DWORD processId,
	Muninn::Object::ProcessEntry& processEntry)
{
	if (!IsValidProcessId(processId)) return FALSE;

	HANDLE snapshotHandle{
		CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) };
	if (!IsValidHandle(snapshotHandle)) return FALSE;

	PROCESSENTRY32W processEntry32W{};
	processEntry32W.dwSize = sizeof(PROCESSENTRY32W);
	if (!Process32FirstW(snapshotHandle, &processEntry32W))
	{
		CloseHandle32(snapshotHandle);
		return FALSE;
	}

	do
	{
		if (processEntry32W.th32ProcessID != processId) continue;
		processEntry.processName = processEntry32W.szExeFile;
		processEntry.parentProcessId = processEntry32W.th32ParentProcessID;
		break;

	} while (Process32NextW(snapshotHandle, &processEntry32W));

	CloseHandle32(snapshotHandle);
	return TRUE;
}

BOOL GetProcessModuleObjects32(
		const HANDLE processHandle,
		const DWORD processId,
		std::vector<Muninn::Object::ModuleEntry>& modules)
	{
		if (!IsValidHandle(processHandle)) return FALSE;
		if (!IsValidProcessId(processId)) return FALSE;

		HANDLE snapshotHandle{
			CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId) };
		if (!IsValidHandle(snapshotHandle)) return FALSE;

		MODULEENTRY32W moduleEntry{};
		moduleEntry.dwSize = sizeof(MODULEENTRY32W);
		if (!Module32FirstW(snapshotHandle, &moduleEntry))
		{
			CloseHandle32(snapshotHandle);
			return FALSE;
		}

		do
		{
			MODULEINFO moduleInfo{};
			if (!K32GetModuleInformation(
				processHandle,
				reinterpret_cast<HMODULE>(moduleEntry.modBaseAddr),
				&moduleInfo,
				sizeof(moduleInfo)))
				continue;

			Muninn::Object::ModuleEntry moduleEntry{};
			moduleEntry.moduleName = moduleEntry.szModule;
			moduleEntry.modulePath = moduleEntry.szExePath;
			moduleEntry.moduleLoadAddress =
				reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
			moduleEntry.moduleEntryPoint =
				reinterpret_cast<uintptr_t>(moduleInfo.EntryPoint);
			moduleEntry.moduleBaseAddress =
				reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
			moduleEntry.moduleImageSize = moduleEntry.modBaseSize;
			moduleEntry.processId = moduleEntry.th32ProcessID;
			modules.push_back(moduleEntry);

		} while (Module32NextW(snapshotHandle, &moduleEntry));
		CloseHandle32(snapshotHandle);
		return TRUE;
	}

	BOOL GetProcessThreadObjects32(
		const HANDLE processHandle,
		const DWORD processId,
		std::vector<Muninn::Object::ThreadEntry>& threads)
	{
		if (!IsValidHandle(processHandle)) return FALSE;
		if (!IsValidProcessId(processId)) return FALSE;

		HANDLE snapshotHandle{
			CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, processId) };
		if (!IsValidHandle(snapshotHandle)) return FALSE;

		THREADENTRY32 threadEntry{};
		threadEntry.dwSize = sizeof(THREADENTRY32);
		if (!Thread32First(snapshotHandle, &threadEntry))
		{
			CloseHandle32(snapshotHandle);
			return FALSE;
		}

		do
		{
			if (threadEntry.th32OwnerProcessID != processId) continue;

			Muninn::Object::ThreadEntry threadEntry{};
			threadEntry.nativeThreadBasePriority =
				static_cast<KPRIORITY>(threadEntry.tpBasePri);
			threadEntry.threadId = threadEntry.th32ThreadID;
			threadEntry.threadOwnerProcessId = threadEntry.th32OwnerProcessID;
			threads.push_back(threadEntry);

		} while (Thread32Next(snapshotHandle, &threadEntry));
		CloseHandle32(snapshotHandle);
		return TRUE;
	}

	BOOL GetProcessHandleObjects32(
		const HANDLE processHandle,
		const DWORD processId,
		std::vector<Muninn::Object::HandleEntry>& handles)
	{
		if (!IsValidHandle(processHandle)) return FALSE;
		if (!IsValidProcessId(processId)) return FALSE;

		HPSS pssSnapshotHandle{};
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
			return FALSE;

		HPSSWALK walkMarkerHandle{};
		if (PssWalkMarkerCreate(nullptr, &walkMarkerHandle) != ERROR_SUCCESS)
		{
			PssFreeSnapshot(GetCurrentProcess(), pssSnapshotHandle);
			return FALSE;
		}

		while (true)
		{
			PSS_HANDLE_ENTRY handleEntry{};
			DWORD walkStatus{ PssWalkSnapshot(
				pssSnapshotHandle,
				PSS_WALK_HANDLES,
				walkMarkerHandle,
				&handleEntry,
				sizeof(handleEntry)) };

			if (walkStatus == ERROR_NO_MORE_ITEMS) break;
			if (walkStatus != ERROR_SUCCESS) break;

			Muninn::Object::HandleEntry handleEntry{};
			handleEntry.typeName
				= handleEntry.TypeName ? handleEntry.TypeName : L"";
			handleEntry.objectName
				= handleEntry.ObjectName ? handleEntry.ObjectName : L"";
			handleEntry.handleValue = handleEntry.Handle;
			handleEntry.grantedAccess = handleEntry.GrantedAccess;

			switch (handleEntry.ObjectType)
			{
			case PSS_OBJECT_TYPE_PROCESS:
				handleEntry.userHandleObjectType
					= Muninn::Object::UserHandleObjectType::Process;
				handleEntry.userTargetProcessId
					= handleEntry.TypeSpecificInformation.Process.ProcessId;
				break;
			case PSS_OBJECT_TYPE_THREAD:
				handleEntry.userHandleObjectType
					= Muninn::Object::UserHandleObjectType::Thread;
				handleEntry.userTargetProcessId
					= handleEntry.TypeSpecificInformation.Thread.ProcessId;
				break;
			case PSS_OBJECT_TYPE_MUTANT:
				handleEntry.userHandleObjectType
					= Muninn::Object::UserHandleObjectType::Mutant;
				handleEntry.userTargetProcessId
					= handleEntry.TypeSpecificInformation.Mutant.OwnerProcessId;
				break;
			case PSS_OBJECT_TYPE_EVENT:
				handleEntry.userHandleObjectType
					= Muninn::Object::UserHandleObjectType::Event;
				// PSS_OBJECT_TYPE_EVENT, doesn't own a processId
				break;
			case PSS_OBJECT_TYPE_SECTION:
				handleEntry.userHandleObjectType
					= Muninn::Object::UserHandleObjectType::Section;
				// PSS_OBJECT_TYPE_SECTION, doesn't own a processId
				break;
			case PSS_OBJECT_TYPE_SEMAPHORE:
				handleEntry.userHandleObjectType
					= Muninn::Object::UserHandleObjectType::Semaphore;
				// PSS_OBJECT_TYPE_SEMAPHORE, doesn't own a processId
				break;
			default:
				handleEntry.userHandleObjectType
					= Muninn::Object::UserHandleObjectType::Unknown;
				handleEntry.userTargetProcessId = 0;
				break;
			}

			handles.push_back(handleEntry);
		}

		PssWalkMarkerFree(walkMarkerHandle);
		PssFreeSnapshot(GetCurrentProcess(), pssSnapshotHandle);
		return TRUE;
	}
*/