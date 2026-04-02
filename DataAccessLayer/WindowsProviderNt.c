#include "DataUtilities.h"
#include "WindowsProviderNt.h"

#ifndef QSI_MIN_BUFFER_SIZE 
#define QSI_MIN_BUFFER_SIZE 0x20
#endif // !QSI_MIN_BUFFER_SIZE 

#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif // !_MAX_PATH

#ifndef MAX_PATH
#define MAX_PATH _MAX_PATH
#endif // !MAX_PATH

#ifndef MAX_MODULES
#define MAX_MODULES 1024
#endif // !MAX_MODULES

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif // !PAGE_SIZE

#ifndef NT_CURRENT_PROCESS
#define NT_CURRENT_PROCESS ((HANDLE)(LONG_PTR)-1)
#endif // !NT_CURRENT_PROCESS

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteVirtualMemoryNt(
	_In_ const HANDLE processHandle,
	_In_ const uintptr_t address,
	_In_ const void* const value,
	_In_ const SIZE_T size)
{
	return NtWriteVirtualMemory(
		processHandle,
		(PVOID)address,
		value,
		size,
		NULL);
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_ReadVirtualMemoryNt(
	_In_ const HANDLE processHandle,
	_In_ const uintptr_t address,
	_Out_ void* const out,
	_In_ const SIZE_T size)
{
	return NtReadVirtualMemory(
		processHandle,
		(PVOID)address,
		out,
		size,
		NULL);
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_OpenProcessHandleNt(
	_In_ const DWORD processId,
	_In_ const ACCESS_MASK accessMask,
	_Out_ HANDLE* const pHandle)
{
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_1;
	if (pHandle == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pHandle = NULL;

	OBJECT_ATTRIBUTES objectAttributes = { 0 };
	objectAttributes.Length = sizeof(OBJECT_ATTRIBUTES);

	CLIENT_ID clientId = { NULL, NULL };
	clientId.UniqueProcess = (HANDLE)(uintptr_t)processId;
	clientId.UniqueThread = NULL;

	NTSTATUS status = NtOpenProcess(
		pHandle,
		accessMask,
		&objectAttributes,
		&clientId);

	if (!DAL_IsValidHandle(*pHandle))
		*pHandle = NULL;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_CloseHandleNt(_In_ const HANDLE handle)
{
	if (!DAL_IsValidHandle(handle))
		return STATUS_INVALID_PARAMETER_1;

	return NtClose(handle);
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_DuplicateHandleNt(
	_In_ const HANDLE sourceHandle,
	_In_ const DWORD processId,
	_Out_ HANDLE* const pDuplicatedHandle)
{
	if (!DAL_IsValidHandle(sourceHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_2;
	if (pDuplicatedHandle == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pDuplicatedHandle = NULL;

	// Prefer explicity over InitializeObjectAttributes(p, n, a, r, s)
	OBJECT_ATTRIBUTES objectAttributes = { 0 };
	objectAttributes.Length = sizeof(OBJECT_ATTRIBUTES);
	objectAttributes.ObjectName = NULL;
	objectAttributes.Attributes = 0ul;
	objectAttributes.RootDirectory = NULL;
	objectAttributes.SecurityDescriptor = NULL;
	objectAttributes.SecurityQualityOfService = NULL;

	CLIENT_ID clientId = { NULL, NULL };
	clientId.UniqueProcess = (HANDLE)(uintptr_t)(processId);
	clientId.UniqueThread = NULL;

	HANDLE remoteProcessHandle = NULL;
	NTSTATUS status = NtOpenProcess(
		&remoteProcessHandle,
		PROCESS_DUP_HANDLE,
		&objectAttributes,
		&clientId);
	if (!NT_SUCCESS(status)) return status;

	HANDLE duplicatedHandle = NULL;
	status = NtDuplicateObject(
		remoteProcessHandle,
		sourceHandle,
		NT_CURRENT_PROCESS,
		pDuplicatedHandle,
		0ul,
		0ul,
		DUPLICATE_SAME_ACCESS);
	DAL_CloseHandleNt(remoteProcessHandle);

	if (!NT_SUCCESS(status))
		*pDuplicatedHandle = NULL;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_OpenProcessTokenHandleNt(
	_In_ const HANDLE processHandle,
	_In_ const ACCESS_MASK accessMask,
	_Out_ HANDLE* const pTokenHandle)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pTokenHandle == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pTokenHandle = NULL;

	NTSTATUS status = NtOpenProcessToken(
		processHandle,
		accessMask,
		pTokenHandle);

	if (!NT_SUCCESS(status))
		*pTokenHandle = NULL;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetFullLuidNt(
	_In_ const LUID luid,
	_Out_ uint64_t* const pFullLuid)
{
	if (!DAL_IsValidLuid(luid))
		return STATUS_INVALID_PARAMETER_1;
	if (pFullLuid == NULL)
		return STATUS_INVALID_PARAMETER_2;

	// cast to uint32_t first to avoid sign extension of signed LONG
	*pFullLuid
		= ((uint64_t)(uint32_t)luid.HighPart << 32) |
		(uint64_t)luid.LowPart;

	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetQSIBufferSizeNt(
	_In_ const SYSTEM_INFORMATION_CLASS infoClass,
	_Out_ DWORD* const pRequiredBufferSize)
{
	if (pRequiredBufferSize == NULL)
		return STATUS_INVALID_PARAMETER_2;

	*pRequiredBufferSize = 0ul;

	BYTE buffer[QSI_MIN_BUFFER_SIZE] = {0};
	NTSTATUS status = NtQuerySystemInformation(
		infoClass,
		buffer,
		sizeof(buffer),
		pRequiredBufferSize);

	if (status != STATUS_INFO_LENGTH_MISMATCH)
		*pRequiredBufferSize = 0ul;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetQOBufferSizeNt(
	_In_ const HANDLE duplicatedHandle,
	_In_ const OBJECT_INFORMATION_CLASS infoClass,
	_Out_ DWORD* const pRequiredBufferSize)
{
	if (!DAL_IsValidHandle(duplicatedHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pRequiredBufferSize == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pRequiredBufferSize = 0ul;

	NTSTATUS status = NtQueryObject(
		duplicatedHandle,
		infoClass,
		NULL,
		0ul,
		pRequiredBufferSize);

	if (status != STATUS_INFO_LENGTH_MISMATCH)
		*pRequiredBufferSize = 0ul;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetQITBufferSizeNt(
	_In_ const HANDLE tokenHandle,
	_In_ const TOKEN_INFORMATION_CLASS infoClass,
	_Out_ DWORD* const pRequiredBufferSize)
{
	if (!DAL_IsValidHandle(tokenHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pRequiredBufferSize == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pRequiredBufferSize = 0ul;

	NTSTATUS status = NtQueryInformationToken(
		tokenHandle,
		infoClass,
		NULL,
		0,
		pRequiredBufferSize);

	if (status != STATUS_INFO_LENGTH_MISMATCH)
		*pRequiredBufferSize = 0ul;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetObjectNameNt(
	_In_ const HANDLE sourceHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	WCHAR* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength)
{
	if (!DAL_IsValidHandle(sourceHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_2;
	if (pBuffer == NULL)
		return STATUS_INVALID_PARAMETER_3;
	if (bufferLength == 0ul)
		return STATUS_BUFFER_TOO_SMALL;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_5;

	memset(
		pBuffer,
		0,
		bufferLength * sizeof(WCHAR));

	*pCopiedLength = 0ul;

	HANDLE duplicatedHandle = NULL;
	NTSTATUS status = DAL_DuplicateHandleNt(
		sourceHandle,
		processId,
		&duplicatedHandle);

	if (!NT_SUCCESS(status))
		return status;

	if (!DAL_IsValidHandle(duplicatedHandle))
		return STATUS_INVALID_HANDLE;

	DWORD requiredBufferSize = 0ul;
	status = DAL_GetQOBufferSizeNt(
		duplicatedHandle,
		ObjectNameInformation,
		&requiredBufferSize);

	if (status != STATUS_INFO_LENGTH_MISMATCH &&
		!NT_SUCCESS(status))
	{
		DAL_CloseHandleNt(duplicatedHandle);
		return status;
	}

	if (!requiredBufferSize)
	{
		DAL_CloseHandleNt(duplicatedHandle);
		return STATUS_UNSUCCESSFUL;
	}

	BYTE* nameInfoBuffer =
		(BYTE*)malloc(requiredBufferSize);
	if (nameInfoBuffer == NULL)
		return STATUS_NO_MEMORY;

	status = NtQueryObject(
		duplicatedHandle,
		ObjectNameInformation,
		nameInfoBuffer,
		requiredBufferSize,
		NULL);

	if (!NT_SUCCESS(status))
	{
		free(nameInfoBuffer);
		DAL_CloseHandleNt(duplicatedHandle);
		return status;
	}

	OBJECT_NAME_INFORMATION* nameInfo =
		(OBJECT_NAME_INFORMATION*)(nameInfoBuffer);

	if (nameInfo->Name.Buffer &&
		nameInfo->Name.Length > 0)
	{
		DWORD charsToCopy =
			nameInfo->Name.Length / sizeof(WCHAR);

		// leave room for null terminator -> (-1ul)
		if (charsToCopy >= bufferLength)
			charsToCopy = bufferLength - 1ul;

		for (DWORD i = 0ul; i < charsToCopy; ++i)
			pBuffer[i] = nameInfo->Name.Buffer[i];

		pBuffer[charsToCopy] = L'\0';
		*pCopiedLength = charsToCopy;
	}

	free(nameInfoBuffer);
	DAL_CloseHandleNt(duplicatedHandle);
	return STATUS_SUCCESS;
}

// Rework from here on down
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetObjectTypeNameNt(
	_In_ const HANDLE sourceHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	WCHAR* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength)
{
	if (!DAL_IsValidHandle(sourceHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_2;
	if (pBuffer == NULL)
		return STATUS_INVALID_PARAMETER_3;
	if (bufferLength == 0ul)
		return STATUS_BUFFER_TOO_SMALL;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_5;

	memset(
		pBuffer,
		0,
		bufferLength * sizeof(WCHAR));

	*pCopiedLength = 0ul;

	HANDLE duplicatedHandle = NULL;
	NTSTATUS status = DAL_DuplicateHandleNt(
		sourceHandle,
		processId,
		&duplicatedHandle);

	if (!NT_SUCCESS(status))
		return status;

	if (!DAL_IsValidHandle(duplicatedHandle))
		return STATUS_INVALID_HANDLE;

	DWORD requiredBufferSize = 0ul;
	status = DAL_GetQOBufferSizeNt(
		duplicatedHandle,
		ObjectTypeInformation,
		&requiredBufferSize);

	if (status != STATUS_INFO_LENGTH_MISMATCH &&
		!NT_SUCCESS(status))
	{
		DAL_CloseHandleNt(duplicatedHandle);
		return status;
	}

	if (!requiredBufferSize)
	{
		DAL_CloseHandleNt(duplicatedHandle);
		return STATUS_UNSUCCESSFUL;
	}

	BYTE* typeInfoBuffer =
		(BYTE*)malloc(requiredBufferSize);
	if (typeInfoBuffer == NULL)
		return STATUS_NO_MEMORY;

	status = NtQueryObject(
		duplicatedHandle,
		ObjectTypeInformation,
		typeInfoBuffer,
		requiredBufferSize,
		NULL);

	if (!NT_SUCCESS(status))
	{
		free(typeInfoBuffer);
		DAL_CloseHandleNt(duplicatedHandle);
		return status;
	}

	OBJECT_TYPE_INFORMATION* typeInfo =
		(OBJECT_TYPE_INFORMATION*)typeInfoBuffer;

	if (typeInfo->TypeName.Buffer &&
		typeInfo->TypeName.Length > 0)
	{
		DWORD charsToCopy =
			typeInfo->TypeName.Length / sizeof(WCHAR);

		// leave room for null terminator -> (-1ul)
		if (charsToCopy >= bufferLength)
			charsToCopy = bufferLength - 1ul;

		for (DWORD i = 0ul; i < charsToCopy; ++i)
			pBuffer[i] = typeInfo->TypeName.Buffer[i];

		pBuffer[charsToCopy] = L'\0';
		*pCopiedLength = charsToCopy;
	}

	free(typeInfoBuffer);
	DAL_CloseHandleNt(duplicatedHandle);
	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetRemoteUnicodeStringNt(
	_In_ const HANDLE processHandle,
	_In_ const UNICODE_STRING* const pRemoteUnicodeString,
	_Out_writes_(bufferLength)
	WCHAR* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pRemoteUnicodeString == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pBuffer == NULL)
		return STATUS_INVALID_PARAMETER_3;
	if (bufferLength == 0ul)
		return STATUS_BUFFER_TOO_SMALL;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_5;

	memset(
		pBuffer,
		0,
		bufferLength * sizeof(WCHAR));

	*pCopiedLength = 0ul;

	UNICODE_STRING localString = {0};

	NTSTATUS status = DAL_ReadVirtualMemoryNt(
		processHandle,
		(uintptr_t)pRemoteUnicodeString,
		&localString,
		sizeof(localString));

	if (!NT_SUCCESS(status))
		return status;

	// The string is empty
	if (!localString.Buffer || localString.Length == 0)
		return STATUS_SUCCESS;

	DWORD charsToCopy =
		localString.Length / sizeof(WCHAR);

	if (charsToCopy >= bufferLength)
		charsToCopy = bufferLength - 1ul;

	SIZE_T bytesToRead =
		charsToCopy * sizeof(WCHAR);

	status = DAL_ReadVirtualMemoryNt(
		processHandle,
		(uintptr_t)localString.Buffer,
		pBuffer,
		bytesToRead);

	if (!NT_SUCCESS(status))
		return status;

	pBuffer[charsToCopy] = L'\0';
	*pCopiedLength = charsToCopy;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetSystemProcessInformationNt(
	_Out_ BYTE** const ppBuffer,
	_Out_ DWORD* const pSize)
{
	if (ppBuffer == NULL)
		return STATUS_INVALID_PARAMETER_1;
	if (pSize == NULL)
		return STATUS_INVALID_PARAMETER_2;

	*ppBuffer = NULL;
	*pSize = 0ul;

	DWORD requiredBufferSize = 0ul;
	NTSTATUS status = DAL_GetQSIBufferSizeNt(
		SystemProcessInformation,
		&requiredBufferSize);

	if (!requiredBufferSize)
		return STATUS_UNSUCCESSFUL;

	BYTE* systemInfoBuffer =
		(BYTE*)malloc(requiredBufferSize);
	if (systemInfoBuffer == NULL)
		return STATUS_NO_MEMORY;

	status = NtQuerySystemInformation(
		SystemProcessInformation,
		systemInfoBuffer,
		requiredBufferSize,
		NULL);

	if (!NT_SUCCESS(status))
	{
		free(systemInfoBuffer);
		return status;
	}

	*ppBuffer = systemInfoBuffer;
	*pSize = requiredBufferSize;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessInformationNt(
	_In_ const HANDLE processHandle,
	_Out_ PROCESS_EXTENDED_BASIC_INFORMATION* const pProcessInfo)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pProcessInfo == NULL)
		return STATUS_INVALID_PARAMETER_2;

	memset(
		pProcessInfo,
		0,
		sizeof(*pProcessInfo));

	NTSTATUS status = NtQueryInformationProcess(
		processHandle,
		ProcessBasicInformation,
		pProcessInfo,
		sizeof(PROCESS_EXTENDED_BASIC_INFORMATION),
		NULL);

	if (!NT_SUCCESS(status))
		memset(
			pProcessInfo,
			0,
			sizeof(*pProcessInfo));

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetImageFileNameNt(
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
	if (bufferLength == 0ul)
		return STATUS_BUFFER_TOO_SMALL;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_4;

	memset(
		pBuffer,
		0,
		bufferLength * sizeof(WCHAR));

	*pCopiedLength = 0ul;

	BYTE imageFileNameBuffer[MAX_PATH];
	memset(
		imageFileNameBuffer,
		0,
		sizeof(imageFileNameBuffer));

	NTSTATUS status = NtQueryInformationProcess(
		processHandle,
		ProcessImageFileName,
		imageFileNameBuffer,
		sizeof(imageFileNameBuffer),
		NULL);

	if (!NT_SUCCESS(status))
		return status;

	PUNICODE_STRING pImageFileName =
		(PUNICODE_STRING)imageFileNameBuffer;

	if (pImageFileName->Buffer &&
		pImageFileName->Length)
	{
		DWORD charsToCopy =
			pImageFileName->Length / sizeof(WCHAR);

		if (charsToCopy >= bufferLength)
			charsToCopy = bufferLength - 1ul;

		for (DWORD i = 0ul; i < charsToCopy; ++i)
			pBuffer[i] = pImageFileName->Buffer[i];

		pBuffer[charsToCopy] = L'\0';
		*pCopiedLength = charsToCopy;
	}

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetImageFileNameWin32Nt(
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
	if (bufferLength == 0ul)
		return STATUS_BUFFER_TOO_SMALL;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_4;

	memset(
		pBuffer,
		0,
		bufferLength * sizeof(WCHAR));

	*pCopiedLength = 0ul;

	BYTE imageFileNameBuffer[MAX_PATH];
	memset(
		imageFileNameBuffer,
		0,
		sizeof(imageFileNameBuffer));

	NTSTATUS status = NtQueryInformationProcess(
		processHandle,
		ProcessImageFileNameWin32,
		imageFileNameBuffer,
		sizeof(imageFileNameBuffer),
		NULL);

	if (!NT_SUCCESS(status))
		return status;

	PUNICODE_STRING pImageFileName =
		(PUNICODE_STRING)imageFileNameBuffer;

	if (pImageFileName->Buffer &&
		pImageFileName->Length)
	{
		DWORD charsToCopy =
			pImageFileName->Length / sizeof(WCHAR);

		if (charsToCopy >= bufferLength)
			charsToCopy = bufferLength - 1ul;

		for (DWORD i = 0ul; i < charsToCopy; ++i)
			pBuffer[i] = pImageFileName->Buffer[i];

		pBuffer[charsToCopy] = L'\0';
		*pCopiedLength = charsToCopy;
	}

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetPebBaseAddressNt(
	_In_ const HANDLE processHandle,
	_Out_ uintptr_t* const pPebBaseAddress)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pPebBaseAddress == NULL)
		return STATUS_INVALID_PARAMETER_2;

	*pPebBaseAddress = 0ull;

	PROCESS_EXTENDED_BASIC_INFORMATION processInfo = { 0 };
	NTSTATUS status = DAL_GetProcessInformationNt(
		processHandle,
		&processInfo);

	if (!NT_SUCCESS(status))
		return status;

	*pPebBaseAddress =
		(uintptr_t)processInfo.BasicInfo.PebBaseAddress;

	if (!DAL_IsValidAddress(*pPebBaseAddress))
	{
		*pPebBaseAddress = 0ull;
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetPebBaseAddressFromProcessInfoNt(
	_In_ const PROCESS_EXTENDED_BASIC_INFORMATION* const pProcessInfo,
	_Out_ uintptr_t* const pPebBaseAddress)
{
	if (pProcessInfo == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pPebBaseAddress == NULL)
		return STATUS_INVALID_PARAMETER_2;

	*pPebBaseAddress =
		(uintptr_t)pProcessInfo->BasicInfo.PebBaseAddress;

	if (!DAL_IsValidAddress(*pPebBaseAddress))
	{
		*pPebBaseAddress = 0ull;
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetPebBaseAddressAndProcessInfoNt(
	_In_ const HANDLE processHandle,
	_Out_ uintptr_t* const pPebBaseAddress,
	_Out_ PROCESS_EXTENDED_BASIC_INFORMATION* const pProcessInfo)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pPebBaseAddress == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pProcessInfo == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pPebBaseAddress = 0ull;

	memset(
		pProcessInfo,
		0,
		sizeof(*pProcessInfo));

	NTSTATUS status = DAL_GetProcessInformationNt(
		processHandle,
		pProcessInfo);

	if (!NT_SUCCESS(status))
		return status;

	*pPebBaseAddress =
		(uintptr_t)pProcessInfo->BasicInfo.PebBaseAddress;

	if (!DAL_IsValidAddress(*pPebBaseAddress))
	{
		*pPebBaseAddress = 0ull;

		memset(
			pProcessInfo,
			0,
			sizeof(*pProcessInfo));

		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetPebNt(
	_In_ const HANDLE processHandle,
	_Out_ PEB* const pPeb)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pPeb == NULL)
		return STATUS_INVALID_PARAMETER_2;

	memset(
		pPeb,
		0,
		sizeof(*pPeb));

	uintptr_t pebBaseAddress = 0ull;
	NTSTATUS status = DAL_GetPebBaseAddressNt(
		processHandle,
		&pebBaseAddress);

	if (!NT_SUCCESS(status))
		return status;

	if (!DAL_IsValidAddress(pebBaseAddress))
		return STATUS_INVALID_ADDRESS;

	status = DAL_ReadVirtualMemoryNt(
		processHandle,
		pebBaseAddress,
		pPeb,
		sizeof(*pPeb));

	if (!NT_SUCCESS(status))
		memset(
			pPeb,
			0,
			sizeof(*pPeb));

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetPebAndPebBaseAddressNt(
	_In_ const HANDLE processHandle,
	_Out_ uintptr_t* const pPebBaseAddress,
	_Out_ PEB* const pPeb)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pPebBaseAddress == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pPeb == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pPebBaseAddress = 0ull;

	memset(
		pPeb,
		0,
		sizeof(*pPeb));

	NTSTATUS status = DAL_GetPebBaseAddressNt(
		processHandle,
		pPebBaseAddress);

	if (!NT_SUCCESS(status))
		return status;

	status = DAL_ReadVirtualMemoryNt(
		processHandle,
		*pPebBaseAddress,
		pPeb,
		sizeof(*pPeb));

	if (!NT_SUCCESS(status))
	{
		*pPebBaseAddress = 0ull;

		memset(
			pPeb,
			0,
			sizeof(*pPeb));
	}

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetModuleBaseAddressNt(
	_In_ const HANDLE processHandle,
	_Out_ uintptr_t* const pModuleBaseAddress)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pModuleBaseAddress == NULL)
		return STATUS_INVALID_PARAMETER_2;

	*pModuleBaseAddress = 0ull;

	uintptr_t pebBaseAddress = 0ull;
	PROCESS_EXTENDED_BASIC_INFORMATION processInfo = { 0 };
	NTSTATUS status = DAL_GetPebBaseAddressAndProcessInfoNt(
		processHandle,
		&pebBaseAddress,
		&processInfo);

	if (!NT_SUCCESS(status))
		return status;

	if (!DAL_IsValidAddress(pebBaseAddress))
		return STATUS_INVALID_ADDRESS;

	PEB peb = { 0 };
	status = DAL_ReadVirtualMemoryNt(
		processHandle,
		pebBaseAddress,
		&peb,
		sizeof(peb));

	if (!NT_SUCCESS(status))
		return status;

	if (!peb.Ldr)
		return STATUS_UNSUCCESSFUL;

	uintptr_t loaderAddress =
		(uintptr_t)peb.Ldr;

	if (!DAL_IsValidAddress(loaderAddress))
		return STATUS_INVALID_ADDRESS;

	// Read loader data
	PEB_LDR_DATA loaderData = { 0 };
	status = DAL_ReadVirtualMemoryNt(
		processHandle,
		loaderAddress,
		&loaderData,
		sizeof(loaderData));

	if (!NT_SUCCESS(status))
		return status;

	// First module in load order list
	uintptr_t firstLink =
		(uintptr_t)loaderData.InLoadOrderModuleList.Flink;

	if (!DAL_IsValidAddress(firstLink))
		return STATUS_INVALID_ADDRESS;

	// Get the LDR_DATA_TABLE_ENTRY
	uintptr_t entryAddress =
		firstLink - offsetof(LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
	LDR_DATA_TABLE_ENTRY entry = { 0 };
	status = DAL_ReadVirtualMemoryNt(
		processHandle,
		entryAddress,
		&entry,
		sizeof(entry));

	if (entry.DllBase == NULL)
		return STATUS_UNSUCCESSFUL;

	*pModuleBaseAddress =
		(uintptr_t)entry.DllBase;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetModuleBaseAddressFromProcessInfoNt(
	_In_ const HANDLE processHandle,
	_In_ const PROCESS_EXTENDED_BASIC_INFORMATION* const processInfo,
	_Out_ uintptr_t* const pModuleBaseAddress)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (processInfo == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pModuleBaseAddress == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pModuleBaseAddress = 0ull;

	uintptr_t pebBaseAddress =
		(uintptr_t)processInfo->BasicInfo.PebBaseAddress;

	if (!DAL_IsValidAddress(pebBaseAddress))
		return STATUS_INVALID_ADDRESS;

	PEB peb = { 0 };
	NTSTATUS status = DAL_ReadVirtualMemoryNt(
		processHandle,
		pebBaseAddress,
		&peb,
		sizeof(peb));

	if (!NT_SUCCESS(status))
		return status;
	if (!peb.Ldr)
		return STATUS_UNSUCCESSFUL;

	uintptr_t loaderAddress =
		(uintptr_t)peb.Ldr;

	if (!DAL_IsValidAddress(loaderAddress))
		return STATUS_INVALID_ADDRESS;

	PEB_LDR_DATA loaderData = { 0 };
	status = DAL_ReadVirtualMemoryNt(
		processHandle,
		loaderAddress,
		&loaderData,
		sizeof(PEB_LDR_DATA));

	if (!NT_SUCCESS(status))
		return status;

	uintptr_t firstLink =
		(uintptr_t)loaderData.InLoadOrderModuleList.Flink;

	if (!DAL_IsValidAddress(firstLink))
		return STATUS_INVALID_ADDRESS;

	uintptr_t entryAddress =
		firstLink - offsetof(LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

	LDR_DATA_TABLE_ENTRY entry = { 0 };
	status = DAL_ReadVirtualMemoryNt(
		processHandle,
		entryAddress,
		&entry,
		sizeof(entry));

	if (!NT_SUCCESS(status))
		return status;
	if (entry.DllBase == NULL)
		return STATUS_UNSUCCESSFUL;

	*pModuleBaseAddress =
		(uintptr_t)entry.DllBase;

	return status;
}

// AI generated, to be reviewed
MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetModuleBaseAddressFromPebBaseAddressNt(
	_In_ const HANDLE processHandle,
	_In_ const uintptr_t* const pPebBaseAddress,
	_Out_ uintptr_t* const pModuleBaseAddress)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pPebBaseAddress == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pModuleBaseAddress == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pModuleBaseAddress = 0ull;

	if (!DAL_IsValidAddress(*pPebBaseAddress))
		return STATUS_INVALID_ADDRESS;

	PEB peb = { 0 };
	NTSTATUS status = DAL_ReadVirtualMemoryNt(
		processHandle,
		*pPebBaseAddress,
		&peb,
		sizeof(peb));

	if (!NT_SUCCESS(status))
		return status;
	if (!peb.Ldr)
		return STATUS_UNSUCCESSFUL;

	uintptr_t loaderAddress =
		(uintptr_t)peb.Ldr;

	if (!DAL_IsValidAddress(loaderAddress))
		return STATUS_INVALID_ADDRESS;

	PEB_LDR_DATA loaderData = { 0 };
	status = DAL_ReadVirtualMemoryNt(
		processHandle,
		loaderAddress,
		&loaderData,
		sizeof(loaderData));

	if (!NT_SUCCESS(status))
		return status;

	uintptr_t firstLink =
		(uintptr_t)loaderData.InLoadOrderModuleList.Flink;

	if (!DAL_IsValidAddress(firstLink))
		return STATUS_INVALID_ADDRESS;

	uintptr_t entryAddress =
		firstLink - offsetof(LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

	LDR_DATA_TABLE_ENTRY entry = { 0 };
	status = DAL_ReadVirtualMemoryNt(
		processHandle,
		entryAddress,
		&entry,
		sizeof(entry));

	if (!NT_SUCCESS(status))
		return status;
	if (entry.DllBase == NULL)
		return STATUS_UNSUCCESSFUL;

	*pModuleBaseAddress =
		(uintptr_t)entry.DllBase;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetModuleBaseAddressFromPebNt(
	_In_ const HANDLE processHandle,
	_In_ const PEB* const pPeb,
	_Out_ uintptr_t* const pModuleBaseAddress)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pPeb == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pModuleBaseAddress == NULL)
		return STATUS_INVALID_PARAMETER_3;

	*pModuleBaseAddress = 0ull;

	if (!pPeb->Ldr)
		return STATUS_UNSUCCESSFUL;

	uintptr_t loaderAddress =
		(uintptr_t)pPeb->Ldr;

	if (!DAL_IsValidAddress(loaderAddress))
		return STATUS_INVALID_ADDRESS;

	PEB_LDR_DATA loaderData = { 0 };
	NTSTATUS status = DAL_ReadVirtualMemoryNt(
		processHandle,
		loaderAddress,
		&loaderData,
		sizeof(loaderData));

	if (!NT_SUCCESS(status))
		return status;

	uintptr_t firstLink =
		(uintptr_t)loaderData.InLoadOrderModuleList.Flink;

	if (!DAL_IsValidAddress(firstLink))
		return STATUS_INVALID_ADDRESS;

	uintptr_t entryAddress =
		firstLink - offsetof(LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

	LDR_DATA_TABLE_ENTRY entry = { 0 };
	status = DAL_ReadVirtualMemoryNt(
		processHandle,
		entryAddress,
		&entry,
		sizeof(entry));

	if (!NT_SUCCESS(status))
		return status;
	if (entry.DllBase == NULL)
		return STATUS_UNSUCCESSFUL;

	*pModuleBaseAddress =
		(uintptr_t)entry.DllBase;

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetWow64InfoNt(
	_In_ const HANDLE processHandle,
	_Out_ ULONG_PTR* const pWow64Info)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pWow64Info == NULL)
		return STATUS_INVALID_PARAMETER_2;

	*pWow64Info = 0ull;

	NTSTATUS status = NtQueryInformationProcess(
		processHandle,
		ProcessWow64Information,
		pWow64Info,
		sizeof(ULONG_PTR),
		NULL);

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessModulesNt(
	_In_ const HANDLE processHandle,
	_In_ const PEB* const pPeb,
	_Out_writes_(bufferLength)
	LDR_DATA_TABLE_ENTRY* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pPeb == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pBuffer == NULL)
		return STATUS_INVALID_PARAMETER_3;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_5;

	PEB peb = {0};

	NTSTATUS status = DAL_ReadVirtualMemoryNt(
		processHandle,
		(uintptr_t)pPeb,
		&peb,
		sizeof(peb));
	if (!NT_SUCCESS(status))
		return status;

	memset(
		pBuffer,
		0,
		bufferLength * sizeof(LDR_DATA_TABLE_ENTRY));

	*pCopiedLength = 0ul;

	uintptr_t loaderAddress =
		(uintptr_t)peb.Ldr;
	if (!DAL_IsValidAddress(loaderAddress))
		return STATUS_INVALID_ADDRESS;

	PEB_LDR_DATA loaderData = { 0 };
	status = DAL_ReadVirtualMemoryNt(
		processHandle,
		loaderAddress,
		&loaderData,
		sizeof(loaderData));

	if (!NT_SUCCESS(status))
		return status;
	if (!loaderData.InLoadOrderModuleList.Flink)
		return STATUS_UNSUCCESSFUL;

	uintptr_t listHead =
		loaderAddress + offsetof(PEB_LDR_DATA, InLoadOrderModuleList);

	if (!DAL_IsValidAddress(listHead))
		return STATUS_INVALID_ADDRESS;

	uintptr_t currentLink =
		(uintptr_t)loaderData.InLoadOrderModuleList.Flink;

	if (!DAL_IsValidAddress(currentLink))
		return STATUS_INVALID_ADDRESS;

	DWORD copied = 0ul;
	DWORD sanityCounter = 0ul;
	while (currentLink && currentLink != listHead)
	{
		if (++sanityCounter > MAX_MODULES)
			break;

		if (copied >= bufferLength)
			return STATUS_BUFFER_TOO_SMALL;

		// first remote module = fLink - ILOL offset
		uintptr_t entryAddress =
			currentLink - offsetof(LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

		LDR_DATA_TABLE_ENTRY entry = { 0 };
		status = DAL_ReadVirtualMemoryNt(
			processHandle,
			entryAddress,
			&entry,
			sizeof(entry));

		if (!NT_SUCCESS(status))
			break;

		pBuffer[copied++] = entry;

		uintptr_t next =
			(uintptr_t)entry.InLoadOrderLinks.Flink;

		if (!DAL_IsValidAddress(next) || next == currentLink)
			break;

		currentLink = next;
	};

	*pCopiedLength = copied;
	return STATUS_SUCCESS;
};

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessThreadsNt(
	_In_ const HANDLE processHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	SYSTEM_THREAD_INFORMATION* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_2;
	if (pBuffer == NULL)
		return STATUS_INVALID_PARAMETER_3;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_5;

	memset(
		pBuffer,
		0,
		bufferLength * sizeof(SYSTEM_THREAD_INFORMATION));

	*pCopiedLength = 0ul;

	DWORD requiredBufferSize = 0ul;
	NTSTATUS status = DAL_GetQSIBufferSizeNt(
		SystemProcessInformation,
		&requiredBufferSize);

	if (!NT_SUCCESS(status))
		return status;

	BYTE* processInfoBuffer =
		(BYTE*)malloc(requiredBufferSize);
	if (processInfoBuffer == NULL)
		return STATUS_NO_MEMORY;

	status = NtQuerySystemInformation(
		SystemProcessInformation,
		processInfoBuffer,
		requiredBufferSize,
		NULL);

	if (!NT_SUCCESS(status))
	{
		free(processInfoBuffer);
		return status;
	}

	PSYSTEM_PROCESS_INFORMATION processInfo =
		(PSYSTEM_PROCESS_INFORMATION)processInfoBuffer;

	DWORD threadCount = 0ul;
	if (!processInfo)
	{
		free(processInfoBuffer);
		return STATUS_UNSUCCESSFUL;
	}

	while (processInfo)
	{
		DWORD processInfoId =
			(DWORD)(uintptr_t)(processInfo->UniqueProcessId);

		if (processInfoId == processId)
		{
			threadCount = processInfo->NumberOfThreads;
			if (pCopiedLength)
				*pCopiedLength = threadCount;

			if (pBuffer)
			{
				uint32_t toCopy =
					DAL_MinU32(bufferLength, threadCount);

				for (uint32_t i = 0u; i < toCopy; ++i)
					pBuffer[i] = processInfo->Threads[i];
			}
			break;
		}
		if (processInfo->NextEntryOffset == 0)
			break;

		processInfo =
			(PSYSTEM_PROCESS_INFORMATION)
			((BYTE*)processInfo +
				processInfo->NextEntryOffset);
	}

	free(processInfoBuffer);
	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessHandlesNt(
	_In_ const HANDLE processHandle,
	_In_ const DWORD processId,
	_Out_writes_(bufferLength)
	SYSTEM_HANDLE_TABLE_ENTRY_INFO* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength)
{
	if (!DAL_IsValidHandle(processHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (!DAL_IsValidProcessId(processId))
		return STATUS_INVALID_PARAMETER_2;
	if (pBuffer == NULL)
		return STATUS_INVALID_PARAMETER_3;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_5;

	memset(
		pBuffer,
		0,
		bufferLength *
		sizeof(SYSTEM_HANDLE_TABLE_ENTRY_INFO));

	*pCopiedLength = 0ul;

	DWORD requiredBufferSize = 0ul;
	NTSTATUS status = DAL_GetQSIBufferSizeNt(
		SystemHandleInformation,
		&requiredBufferSize);
	requiredBufferSize += PAGE_SIZE;

	if (!NT_SUCCESS(status))
		return status;

	if (!requiredBufferSize)
		return STATUS_UNSUCCESSFUL;

	BYTE* handleInfoBuffer =
		(BYTE*)malloc(requiredBufferSize);
	if (handleInfoBuffer == NULL)
		return STATUS_NO_MEMORY;

	status = NtQuerySystemInformation(
		SystemHandleInformation,
		handleInfoBuffer,
		requiredBufferSize,
		NULL);

	if (!NT_SUCCESS(status))
	{
		free(handleInfoBuffer);
		return status;
	}

	PSYSTEM_HANDLE_INFORMATION handleInfo =
		(PSYSTEM_HANDLE_INFORMATION)handleInfoBuffer;

	if (!handleInfo)
	{
		free(handleInfoBuffer);
		return STATUS_UNSUCCESSFUL;
	}

	DWORD copied = 0ul;
	DWORD total = 0ul;

	for (DWORD i = 0ul; i < handleInfo->NumberOfHandles; ++i)
	{
		const SYSTEM_HANDLE_TABLE_ENTRY_INFO* entry =
			&handleInfo->Handles[i];

		if ((DWORD)entry->UniqueProcessId != processId)
			continue;

		if (copied < bufferLength)
		{
			pBuffer[copied] = *entry;
			++copied;
		}

		++total;
	}

	free(handleInfoBuffer);
	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessTokenStatisticsNt(
	_In_ const HANDLE tokenHandle,
	_Out_ TOKEN_STATISTICS* const pTokenStatistics)
{
	if (!DAL_IsValidHandle(tokenHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pTokenStatistics == NULL)
		return STATUS_INVALID_PARAMETER_2;

	memset(
		pTokenStatistics,
		0,
		sizeof(TOKEN_STATISTICS));

	// requiredBufferSize, the tarnished one
	DWORD requiredBufferSize = 0ul;
	NTSTATUS status = NtQueryInformationToken(
		tokenHandle,
		(TOKEN_INFORMATION_CLASS)TokenStatistics, // phnt
		pTokenStatistics,
		sizeof(TOKEN_STATISTICS),
		&requiredBufferSize);

	if (!NT_SUCCESS(status))
		memset(
			pTokenStatistics,
			0,
			sizeof(TOKEN_STATISTICS));

	return status;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessTokenPriviligesNt(
	_In_ const HANDLE tokenHandle,
	_Out_writes_(bufferLength)
	LUID_AND_ATTRIBUTES* const pBuffer,
	_In_ const DWORD bufferLength,
	_Out_ DWORD* const pCopiedLength)
{
	if (!DAL_IsValidHandle(tokenHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pBuffer == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (pCopiedLength == NULL)
		return STATUS_INVALID_PARAMETER_4;

	DWORD requiredBufferSize = 0ul;
	NTSTATUS status = DAL_GetQITBufferSizeNt(
		tokenHandle,
		TokenPrivileges,
		&requiredBufferSize);

	if (!NT_SUCCESS(status))
		return status;

	BYTE* privilegesBuffer =
		(BYTE*)malloc(requiredBufferSize);
	if (privilegesBuffer == NULL)
		return STATUS_NO_MEMORY;

	status = NtQueryInformationToken(
		tokenHandle,
		TokenPrivileges,
		privilegesBuffer,
		requiredBufferSize,
		&requiredBufferSize);

	if (!NT_SUCCESS(status))
	{
		free(privilegesBuffer);
		return status;
	}

	PTOKEN_PRIVILEGES privileges =
		(PTOKEN_PRIVILEGES)privilegesBuffer;

	DWORD totalPrivileges = privileges->PrivilegeCount;

	DWORD copied = DAL_MinU32(
		bufferLength,
		totalPrivileges);

	for (DWORD i = 0ul; i < copied; ++i)
	{
		pBuffer[i] = privileges->Privileges[i];
	}

	*pCopiedLength = totalPrivileges;
	free(privilegesBuffer);
	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_GetProcessTokenSessionIdNt(
	_In_ const HANDLE tokenHandle,
	_Out_ DWORD* const pSessionId)
{
	if (!DAL_IsValidHandle(tokenHandle))
		return STATUS_INVALID_PARAMETER_1;
	if (pSessionId == NULL)
		return STATUS_INVALID_PARAMETER_2;

	*pSessionId = 0ul;

	DWORD requiredBufferSize = 0ul;
	NTSTATUS status = NtQueryInformationToken(
		tokenHandle,
		TokenSessionId,
		pSessionId,
		sizeof(ULONG),
		&requiredBufferSize);

	if (!NT_SUCCESS(status))
		*pSessionId = 0ul;

	return status;
}

/*
	BOOL GetProcessHandleObjectsNt(
	const HANDLE processHandle,
	const DWORD processId,
	std::vector<Muninn::Object::HandleModel>& handles)
{
	if (!IsValidHandle(processHandle)) return FALSE;
	if (!IsValidProcessId(processId)) return FALSE;

	DWORD requiredBufferSize{ DAL_GetQSIBufferSizeNt(SystemHandleInformation) + PAGE_SIZE };
	BYTE* handleInfoBuffer = new BYTE[requiredBufferSize];
	NTSTATUS ntStatus{ NtQuerySystemInformation(
			SystemHandleInformation,
			handleInfoBuffer,
			requiredBufferSize,
			nullptr) };

	if (!NT_SUCCESS(ntStatus))
	{
		delete[] handleInfoBuffer;
		return {};
	}

	PSYSTEM_HANDLE_INFORMATION handleInfo{ reinterpret_cast<PSYSTEM_HANDLE_INFORMATION>(handleInfoBuffer) };
	if (!handleInfo)
	{
		delete[] handleInfoBuffer;
		return {};
	}

	for (ULONG i{ 0 }; i < handleInfo->NumberOfHandles; ++i)
	{
		const SYSTEM_HANDLE_TABLE_ENTRY_INFO& sHandleInfo{ handleInfo->Handles[i] };
		if (static_cast<uintptr_t>(sHandleInfo.UniqueProcessId) != static_cast<uintptr_t>(processId))
			continue;

		Muninn::Object::HandleModel handleEntry{};
		handleEntry.handleValue = reinterpret_cast<HANDLE>(sHandleInfo.HandleValue);
		handleEntry.typeName = DAL_GetObjectTypeNameNt(handleEntry.handleValue, processId);
		handleEntry.objectName = DAL_GetObjectNameNt(handleEntry.handleValue, processId);
		handleEntry.grantedAccess = sHandleInfo.GrantedAccess;

		// experimental
		if (handleEntry.typeName == L"Process")
			handleEntry.userHandleObjectType = Muninn::Object::UserHandleObjectType::Process;
		else if (handleEntry.typeName == L"Thread")
			handleEntry.userHandleObjectType = Muninn::Object::UserHandleObjectType::Thread;
		else if (handleEntry.typeName == L"Mutant")
			handleEntry.userHandleObjectType = Muninn::Object::UserHandleObjectType::Mutant;
		else if (handleEntry.typeName == L"Event")
			handleEntry.userHandleObjectType = Muninn::Object::UserHandleObjectType::Event;
		else if (handleEntry.typeName == L"Section")
			handleEntry.userHandleObjectType = Muninn::Object::UserHandleObjectType::Section;
		else if (handleEntry.typeName == L"Semaphore")
			handleEntry.userHandleObjectType = Muninn::Object::UserHandleObjectType::Semaphore;

		handles.push_back(handleEntry);
	}
	delete[] handleInfoBuffer;
	return TRUE;
}

std::vector<SYSTEM_EXTENDED_THREAD_INFORMATION> GetProcessThreadsExtendedNt(const HANDLE processHandle, const DWORD processId)
{
	if (!IsValidHandle(processHandle)) return {};
	if (!IsValidProcessId(processId)) return {};

	const DWORD requiredBufferSize{ DAL_GetQSIBufferSizeNt(SystemProcessInformation) };
	BYTE* processInfoBuffer = new BYTE[requiredBufferSize];
	NTSTATUS status{ NtQuerySystemInformation(
		SystemProcessInformation,
		processInfoBuffer,
		requiredBufferSize,
		nullptr) };

	if (!NT_SUCCESS(status))
	{
		delete[] processInfoBuffer;
		return {};
	}

	PSYSTEM_PROCESS_INFORMATION processInfo
	{ reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(processInfoBuffer) };
	if (!processInfo)
	{
		delete[] processInfoBuffer;
		return {};
	}

	std::vector<SYSTEM_EXTENDED_THREAD_INFORMATION> threads{};
	while (processInfo)
	{
		DWORD processInfoId{ static_cast<DWORD>(
			reinterpret_cast<uintptr_t>(processInfo->UniqueProcessId)) };

		if (processInfoId == processId)
		{
			for (ULONG i{ 0 }; i < processInfo->NumberOfThreads; ++i)
			{
				const SYSTEM_EXTENDED_THREAD_INFORMATION& sThreadInfo = processInfo->ThreadsEx[i];
				threads.push_back(sThreadInfo);
			} break;
		}
		if (processInfo->NextEntryOffset == 0) break;

		processInfo = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(
			reinterpret_cast<BYTE*>(processInfo) +
			processInfo->NextEntryOffset);
	}
	delete[] processInfoBuffer;
	return threads;
}

BOOL GetProcessThreadObjectsNt(
	const HANDLE processHandle,
	const DWORD processId,
	std::vector<Muninn::Object::ThreadModel>& threads)
{
	if (!IsValidHandle(processHandle)) return FALSE;
	if (!IsValidProcessId(processId)) return FALSE;

	const DWORD requiredBufferSize{ DAL_GetQSIBufferSizeNt(SystemProcessInformation) };
	BYTE* processInfoBuffer = new BYTE[requiredBufferSize];
	NTSTATUS status{ NtQuerySystemInformation(
		SystemProcessInformation,
		processInfoBuffer,
		requiredBufferSize,
		nullptr) };

	if (!NT_SUCCESS(status))
	{
		delete[] processInfoBuffer;
		return FALSE;
	}

	PSYSTEM_PROCESS_INFORMATION processInfo
	{ reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(processInfoBuffer) };
	if (!processInfo)
	{
		delete[] processInfoBuffer;
		return FALSE;
	}

	while (processInfo)
	{
		DWORD processInfoId{ static_cast<DWORD>(
			reinterpret_cast<uintptr_t>(processInfo->UniqueProcessId)) };

		if (processInfoId == processId)
		{
			for (ULONG i{ 0 }; i < processInfo->NumberOfThreads; ++i)
			{
				const SYSTEM_THREAD_INFORMATION& sThreadInfo{ processInfo->Threads[i] };
				Muninn::Object::ThreadModel threadEntry{};
				threadEntry.kernelThreadStartAddress =
					reinterpret_cast<uintptr_t>(sThreadInfo.StartAddress);
				threadEntry.nativeThreadBasePriority =
					static_cast<KPRIORITY>(sThreadInfo.BasePriority);
				threadEntry.threadId =
					static_cast<DWORD>(reinterpret_cast<uintptr_t>(sThreadInfo.ClientId.UniqueThread));
				threadEntry.threadOwnerProcessId = processId;
				threads.push_back(threadEntry);
			} break;
		}
		if (processInfo->NextEntryOffset == 0) break;

		processInfo = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(
			reinterpret_cast<BYTE*>(processInfo) +
			processInfo->NextEntryOffset);
	}
	delete[] processInfoBuffer;
	return TRUE;
}

	BOOL GetProcessThreadObjectsExtendedNt(
		const HANDLE processHandle,
		const DWORD processId,
		std::vector<Muninn::Object::ThreadModel>& threads)
	{
		if (!IsValidHandle(processHandle)) return FALSE;
		if (!IsValidProcessId(processId)) return FALSE;

		const DWORD requiredBufferSize{ DAL_GetQSIBufferSizeNt(SystemExtendedProcessInformation) };
		BYTE* processInfoBuffer = new BYTE[requiredBufferSize];
		NTSTATUS status{ NtQuerySystemInformation(
			SystemExtendedProcessInformation,
			processInfoBuffer,
			requiredBufferSize,
			nullptr) };

		if (!NT_SUCCESS(status))
		{
			delete[] processInfoBuffer;
			return FALSE;
		}

		PSYSTEM_PROCESS_INFORMATION processInfo
		{ reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(processInfoBuffer) };
		if (!processInfo)
		{
			delete[] processInfoBuffer;
			return FALSE;
		}

		while (processInfo)
		{
			DWORD processInfoId{ static_cast<DWORD>(
				reinterpret_cast<uintptr_t>(processInfo->UniqueProcessId)) };

			if (processInfoId == processId)
			{
				for (ULONG i{ 0 }; i < processInfo->NumberOfThreads; ++i)
				{
					const SYSTEM_THREAD_INFORMATION& sThreadInfo{ processInfo->Threads[i] };
					const SYSTEM_EXTENDED_THREAD_INFORMATION& sThreadExInfo{ processInfo->ThreadsEx[i] };
					Muninn::Object::ThreadModel threadEntry{};
					threadEntry.kernelThreadStartAddress =
						reinterpret_cast<uintptr_t>(sThreadInfo.StartAddress);
					threadEntry.win32ThreadStartAddress =
						reinterpret_cast<uintptr_t>(sThreadExInfo.Win32StartAddress);
					threadEntry.tebBaseAddress =
						reinterpret_cast<uintptr_t>(sThreadExInfo.TebBase);
					threadEntry.nativeThreadBasePriority =
						static_cast<KPRIORITY>(sThreadInfo.BasePriority);
					threadEntry.threadId =
						static_cast<DWORD>(
							reinterpret_cast<uintptr_t>(sThreadInfo.ClientId.UniqueThread));
					threadEntry.threadOwnerProcessId = processId;
					threads.push_back(threadEntry);
				} break;
			}
			if (processInfo->NextEntryOffset == 0) break;

			processInfo = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(
				reinterpret_cast<BYTE*>(processInfo) +
				processInfo->NextEntryOffset);
		}
		delete[] processInfoBuffer;
		return TRUE;
	}

BOOL GetProcessModuleObjectsNt(
	const HANDLE processHandle,
	const DWORD processId,
	const PEB& peb,
	std::vector<Muninn::Object::ModuleModel>& modules)
{
	if (!IsValidHandle(processHandle)) return FALSE;
	if (!IsValidProcessId(processId)) return FALSE;
	if (!peb.Ldr) return FALSE;

	uintptr_t loaderAddress{ reinterpret_cast<uintptr_t>(peb.Ldr) };
	if (!IsValidAddress(loaderAddress)) return FALSE;

	PEB_LDR_DATA loaderData{};
	if (!NT_SUCCESS(ReadVirtualMemoryNt<PEB_LDR_DATA>(processHandle, loaderAddress, loaderData))) return {};
	if (!loaderData.InLoadOrderModuleList.Flink) return FALSE;

	uintptr_t listHead{ loaderAddress + offsetof(PEB_LDR_DATA, InLoadOrderModuleList) };
	if (!IsValidAddress(listHead)) return FALSE;

	uintptr_t currentLink{ reinterpret_cast<uintptr_t>(loaderData.InLoadOrderModuleList.Flink) };
	if (!IsValidAddress(currentLink)) return FALSE;

	size_t sanityCounter{ 0 };
	while (currentLink && currentLink != listHead)
	{
		if (++sanityCounter > MAX_MODULES)
			break;

		// first remote module = fLink - ILOL offset
		uintptr_t entryAddress{ currentLink - offsetof(LDR_DATA_TABLE_ENTRY, InLoadOrderLinks) };
		LDR_DATA_TABLE_ENTRY entry{};
		if (!NT_SUCCESS(ReadVirtualMemoryNt<LDR_DATA_TABLE_ENTRY>(processHandle, entryAddress, entry)))
			break;

		Muninn::Object::ModuleModel moduleEntry{};
		moduleEntry.moduleName = DAL_GetRemoteUnicodeStringNt(processHandle, entry.BaseDllName);
		moduleEntry.modulePath = DAL_GetRemoteUnicodeStringNt(processHandle, entry.FullDllName);
		moduleEntry.moduleEntryPoint
			= reinterpret_cast<uintptr_t>(entry.EntryPoint);
		moduleEntry.moduleBaseAddress
			= reinterpret_cast<uintptr_t>(entry.DllBase);
		moduleEntry.parentDllBaseAddress
			= reinterpret_cast<uintptr_t>(entry.ParentDllBase);
		moduleEntry.moduleImageSize = entry.SizeOfImage;
		moduleEntry.processId = processId;
		moduleEntry.tlsIndex = entry.TlsIndex;
		modules.push_back(std::move(moduleEntry));

		uintptr_t next =
			reinterpret_cast<uintptr_t>(entry.InLoadOrderLinks.Flink);
		if (!IsValidAddress(next) || next == currentLink) break;
		else currentLink = next;
	};
	return TRUE;
};

std::vector<Muninn::Object::ProcessEntry> WindowsProviderNt::QueryProcesses()
{
	const DWORD requiredBufferSize{ Muninn::Service::DAL_GetQSIBufferSizeNt(SystemProcessInformation) };
	std::unique_ptr<BYTE[]> pBuffer(new BYTE[requiredBufferSize]);
	NTSTATUS systemInfoStatus{ NtQuerySystemInformation(
		SystemProcessInformation,
		pBuffer.get(),
		requiredBufferSize,
		nullptr) };

	if (!NT_SUCCESS(systemInfoStatus)) return {};

	std::vector<Muninn::Object::ProcessEntry> processList{};
	PSYSTEM_PROCESS_INFORMATION processInfo = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(pBuffer.get());
	while (processInfo)
	{
		Muninn::Object::ProcessEntry pEntry{};
		pEntry.processId = static_cast<DWORD>(reinterpret_cast<uintptr_t>(processInfo->UniqueProcessId));
		pEntry.parentProcessId = static_cast<DWORD>(reinterpret_cast<uintptr_t>(processInfo->InheritedFromUniqueProcessId));
		pEntry.processName = (processInfo->ImageName.Buffer) ? processInfo->ImageName.Buffer : L"";
		QueryModuleBaseAddress(pEntry.processId, pEntry.processName);

		ACCESS_MASK accessMasks[]{
			PROCESS_ALL_ACCESS,
			PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			PROCESS_QUERY_LIMITED_INFORMATION
		};

		HANDLE hProc{};
		for (ACCESS_MASK accessMask : accessMasks)
		{
			hProc = Muninn::Service::OpenHandleNt(pEntry.processId, accessMask);
			if (Muninn::Service::IsValidHandle(hProc)) break;
			else return{};
		}

		GetExtendedProcessInfo(hProc);
		DAL_GetImageFileNameNt(hProc);
		QueryPriorityClassNt(hProc);
		QueryArchitectureNt(hProc);

		// Threads
		for (ULONG i = 0; i < processInfo->NumberOfThreads; ++i)
		{
			Muninn::Object::ThreadModel threadEntry{};
			const SYSTEM_THREAD_INFORMATION& sThreadInfo = processInfo->Threads[i];

			threadEntry.structureSize = sizeof(SYSTEM_THREAD_INFORMATION);
			threadEntry.threadId = static_cast<DWORD>(
				reinterpret_cast<uintptr_t>(sThreadInfo.ClientId.UniqueThread));
			threadEntry.ownerProcessId = pEntry.processId;
			threadEntry.basePriority = sThreadInfo.BasePriority;
			threadEntry.startAddress = sThreadInfo.StartAddress;
			threadEntry.threadState = sThreadInfo.ThreadState;
			pEntry.threads.push_back(threadEntry);
		}
		processList.push_back(pEntry);
		Muninn::Service::DAL_CloseHandleNt(hProc);

		// Advance to next process (ALWAYS)
		if (processInfo->NextEntryOffset)
		{
			processInfo = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(
				reinterpret_cast<BYTE*>(processInfo) + processInfo->NextEntryOffset);
		}
		else break;
	}
	return processList;
}

BOOL GetProcessTokenPriviligeObjectsNt(const HANDLE tokenHandle, std::vector<Muninn::Object::PrivilegeEntry>& privileges)
{
	if (!IsValidHandle(tokenHandle)) return FALSE;

	std::vector<LUID_AND_ATTRIBUTES> priviligesBuffer
	{ DAL_GetProcessTokenPriviligesNt(tokenHandle) };
	if (priviligesBuffer.empty()) return FALSE;

	for (LUID_AND_ATTRIBUTES privilege : priviligesBuffer)
	{
		Muninn::Object::PrivilegeEntry privilegeEntry{};
		privilegeEntry.TokenLuid = DAL_GetFullLuidNt(privilege.Luid);
		privilegeEntry.TokenAttributes = privilege.Attributes;
		privileges.push_back(privilegeEntry);
	}
	return TRUE;
}

BOOL GetProcessAccessTokenObjectNt(
	const HANDLE processHandle,
	const ACCESS_MASK accessMask,
	Muninn::Object::AccessTokenModel& accessToken)
{
	if (!IsValidHandle(processHandle)) return FALSE;

	HANDLE tokenHandle{ DAL_OpenProcessTokenHandleNt(processHandle, accessMask) };
	if (!IsValidHandle(tokenHandle)) return FALSE;

	TOKEN_STATISTICS statistics{
		DAL_GetProcessTokenStatisticsNt(tokenHandle) };
	if (!IsValidLuid(statistics.TokenId)) return FALSE;
	if (statistics.PrivilegeCount <= 0) return FALSE;

	DWORD sessionId{ DAL_GetProcessTokenSessionIdNt(tokenHandle) };
	if (!sessionId) return FALSE;

	std::vector<Muninn::Object::PrivilegeEntry> privileges{};
	if (!GetProcessTokenPriviligeObjectsNt(tokenHandle, privileges))
		return FALSE;

	accessToken.TokenPrivileges = privileges;
	accessToken.TokenId = DAL_GetFullLuidNt(statistics.TokenId);
	accessToken.AuthenticationId = DAL_GetFullLuidNt(statistics.AuthenticationId);
	accessToken.SessionId = sessionId;
	return TRUE;
}
*/