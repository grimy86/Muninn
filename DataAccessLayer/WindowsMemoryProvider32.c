#include "WindowsMemoryProvider32.h"

#ifndef NOP_OPCODE
#define NOP_OPCODE 0x90
#endif // !NOP_OPCODE

#ifndef JMP_REL32_OPCODE
#define JMP_REL32_OPCODE 0xE9
#endif // !JMP_REL32_OPCODE

#ifndef JMP_ABS64_OPCODE
#define JMP_ABS64_OPCODE 0xFF
#endif // !JMP_ABS64_OPCODE

#ifndef JMP_ABS64_MODRM
#define JMP_ABS64_MODRM 0x25
#endif // !JMP_ABS64_MODRM

#ifndef JMP_REL32_LENGTH
#define JMP_REL32_LENGTH 5 // E9 xx xx xx xx
#endif // !JMP_REL32_LENGTH

#ifndef JMP_ABS64_LENGTH
#define JMP_ABS64_LENGTH  14  // FF 25 00 00 00 00 <8-byte addr>
#endif // !JMP_ABS64_LENGTH

MUNINN_API NTSTATUS MUNINN_CALL
DAL_PatchMemory32(
	_In_ LPVOID const targetAddress,
	_In_ const BYTE* const patchBytes,
	_In_ SIZE_T size)
{
	if (targetAddress == NULL)
		return STATUS_INVALID_PARAMETER_1;
	if (patchBytes == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (size == 0ul)
		return STATUS_INVALID_PARAMETER_3;

	DWORD pageProtection = 0ul;
	BOOL status = VirtualProtect(
		targetAddress,
		size,
		PAGE_EXECUTE_READWRITE,
		&pageProtection);

	if (!status)
		return STATUS_UNSUCCESSFUL;

	memcpy(targetAddress, patchBytes, size);

	DWORD buffer = 0ul;
	status = VirtualProtect(
		targetAddress,
		size,
		pageProtection,
		&buffer);

	if (!status)
		return STATUS_UNSUCCESSFUL;

	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteRelativeDetour32(
	_In_ LPVOID const targetAddress,
	_In_ LPVOID const detourAddress,
	_In_ SIZE_T const size)
{
	if (targetAddress == NULL)
		return STATUS_INVALID_PARAMETER_1;
	if (detourAddress == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (size < JMP_REL32_LENGTH)
		return STATUS_INVALID_PARAMETER_3;

	ptrdiff_t relativeOffset =
		(uintptr_t)detourAddress -
		((uintptr_t)targetAddress +
			JMP_REL32_LENGTH);

	// Ensure the offset fits within rel32 range (±2GB)
	if (relativeOffset < (ptrdiff_t)INT32_MIN ||
		relativeOffset >(ptrdiff_t)INT32_MAX)
		return STATUS_NOT_SUPPORTED;

	BYTE shellcode[32] = { 0 };
	memset(shellcode, NOP_OPCODE, size);
	shellcode[0] = JMP_REL32_OPCODE;
	memcpy(shellcode + 1, &relativeOffset, sizeof(DWORD));

	return DAL_PatchMemory32(targetAddress, shellcode, size);
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteRelativeTrampoline32(
	_In_  LPVOID const  targetAddress,
	_In_  LPVOID const  detourAddress,
	_In_  SIZE_T const  size,
	_Out_ LPVOID* const pGatewayOut)
{
	if (targetAddress == NULL)
		return STATUS_INVALID_PARAMETER_1;
	if (detourAddress == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (size < JMP_REL32_LENGTH)
		return STATUS_INVALID_PARAMETER_3;
	if (pGatewayOut == NULL)
		return STATUS_INVALID_PARAMETER_4;

	BYTE* gateway = (BYTE*)VirtualAlloc(
		NULL,
		size + JMP_REL32_LENGTH,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE);

	if (gateway == NULL)
		return STATUS_NO_MEMORY;

	// Copy stolen bytes to gateway
	memcpy(gateway, targetAddress, size);

	ptrdiff_t jumpBackOffset =
		(uintptr_t)targetAddress + size -
		((uintptr_t)gateway + size + JMP_REL32_LENGTH);

	// Ensure the offset fits within rel32 range (±2GB)
	if (jumpBackOffset < (ptrdiff_t)INT32_MIN ||
		jumpBackOffset >(ptrdiff_t)INT32_MAX)
	{
		VirtualFree(gateway, 0, MEM_RELEASE);
		return STATUS_NOT_SUPPORTED;
	}

	// Write jump back at end of stolen bytes
	gateway[size] = JMP_REL32_OPCODE;
	memcpy(gateway + size + 1, &jumpBackOffset, sizeof(DWORD));

	NTSTATUS status = DAL_WriteRelativeDetour32(
		targetAddress,
		detourAddress,
		size);

	if (!NT_SUCCESS(status))
	{
		VirtualFree(gateway, 0, MEM_RELEASE);
		return status;
	}

	*pGatewayOut = gateway;
	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_RestoreRelativeTrampoline32(
	_In_ LPVOID const targetAddress,
	_In_ LPVOID const gatewayAddress,
	_In_ SIZE_T const size)
{
	if (targetAddress == NULL)
		return STATUS_INVALID_PARAMETER_1;
	if (gatewayAddress == NULL)
		return STATUS_INVALID_PARAMETER_2;
	if (size < JMP_REL32_LENGTH)
		return STATUS_INVALID_PARAMETER_3;

	// Restore original bytes from gateway back to target
	NTSTATUS status = DAL_PatchMemory32(
		targetAddress,
		(BYTE*)gatewayAddress,
		size);

	if (!NT_SUCCESS(status))
		return status;

	// Free the gateway
	if (!VirtualFree(gatewayAddress, 0, MEM_RELEASE))
		return STATUS_UNSUCCESSFUL;

	return STATUS_SUCCESS;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteAbsoluteDetour32(
	_In_ LPVOID const targetAddress,
	_In_ LPVOID const detourAddress,
	_In_ SIZE_T const size)
{
#ifndef _WIN64
	return STATUS_NOT_IMPLEMENTED;
#endif

	if (targetAddress == NULL)
		return STATUS_INVALID_PARAMETER_1;
	// Ensure address is canonical (user space, bits 63-48 must be zero)
	if ((uintptr_t)detourAddress >> 48 != 0)
		return STATUS_INVALID_PARAMETER_2;
	if (size < JMP_ABS64_LENGTH)
		return STATUS_INVALID_PARAMETER_3;

	BYTE shellcode[32] = { 0 };
	memset(shellcode, NOP_OPCODE, size);

	// FF 25 00000000 — JMP QWORD PTR [RIP+0]
	shellcode[0] = JMP_ABS64_OPCODE;
	shellcode[1] = JMP_ABS64_MODRM;

	// RIP-relative offset = 0
	memset(shellcode + 2, 0x00, sizeof(DWORD));

	// 8-byte absolute destination address
	memcpy(shellcode + 6, &detourAddress, sizeof(UINT64));

	return DAL_PatchMemory32(targetAddress, shellcode, size);
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteVTableHook32(
	_In_  LPVOID const  pObject,
	_In_  DWORD const   methodIndex,
	_In_  LPVOID const  hookAddress,
	_Out_ LPVOID* const pOriginalOut)
{
	return STATUS_NOT_IMPLEMENTED;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_RestoreVTableHook32(
	_In_ LPVOID const pObject,
	_In_ DWORD const  methodIndex,
	_In_ LPVOID const originalAddress)
{
	return STATUS_NOT_IMPLEMENTED;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteVTableInlineHook32(
	_In_  LPVOID const  vTableAddress,
	_In_  DWORD const   methodIndex,
	_In_  LPVOID const  hookAddress,
	_Out_ LPVOID* const pOriginalOut)
{
	return STATUS_NOT_IMPLEMENTED;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_RestoreVTableInlineHook32(
	_In_ LPVOID const vTableAddress,
	_In_ DWORD const methodIndex,
	_In_ LPVOID const originalAddress)
{
	return STATUS_NOT_IMPLEMENTED;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteIATHook32(
	_In_  const WCHAR* const moduleName,
	_In_  const WCHAR* const functionName,
	_In_  LPVOID const hookAddress,
	_Out_ LPVOID* const pOriginalOut)
{
	return STATUS_NOT_IMPLEMENTED;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_RestoreIATHook32(
	_In_ LPCWSTR const moduleName,
	_In_ LPCWSTR const functionName,
	_In_ LPVOID const originalAddress)
{
	return STATUS_NOT_IMPLEMENTED;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_WriteHWBP32(
	_In_ LPVOID const targetAddress,
	_In_ DWORD const condition,
	_In_ PVECTORED_EXCEPTION_HANDLER const veHandler)
{
	return STATUS_NOT_IMPLEMENTED;
}

MUNINN_API NTSTATUS MUNINN_CALL
DAL_RemoveHWBP32(
	_In_ LPVOID const targetAddress,
	_In_ PVECTORED_EXCEPTION_HANDLER const veHandler)
{
	return STATUS_NOT_IMPLEMENTED;
}