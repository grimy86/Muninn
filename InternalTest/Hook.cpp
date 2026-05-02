#include "Hook.h"

#ifndef NOP_OPCODE
#define NOP_OPCODE 0x90
#endif // !NOP

#ifndef JMP_REL32_OPCODE
#define JMP_REL32_OPCODE 0xE9
#endif // !JMP_REL32_OPCODE

#ifndef JMP_REL32_LENGTH
#define JMP_REL32_LENGTH 5 // bytes
#endif // !JMP_LENGTH

bool PatchMemory(
	uintptr_t const targetAddress,
	const BYTE* const shellcode,
	const size_t byteLength) noexcept
{
	// Change the memory protection of the page containing the target function
	// Also store the original page protection to restore it later
	DWORD pageProtection{ 0ul };
	BOOL status{ VirtualProtect(
		(LPVOID)targetAddress,
		byteLength,
		PAGE_EXECUTE_READWRITE,
		&pageProtection) };

	// If the function fails, the return value is zero.
	if (!status)
		return false;

	// Overwrite the target function's instructions with the provided data
	memcpy((LPVOID)targetAddress, shellcode, byteLength);

	// Restore the original page protection
	DWORD buffer{ 0ul };
	status = VirtualProtect(
		(LPVOID)targetAddress,
		byteLength,
		pageProtection,
		&buffer);
	return status;
}

bool HookFunction(
	uintptr_t const targetFunctionAddress,
	uintptr_t const hookFunctionAddress,
	const size_t instructionLength) noexcept
{
#ifdef _WIN64
	return false;
#endif

	// Must have enough room for a 5-byte jmp
	if (instructionLength < JMP_REL32_LENGTH)
		return false;

	// Calculate the relative address from target to hook
	uintptr_t relativeHookAddress{
		hookFunctionAddress -
		targetFunctionAddress -
		JMP_REL32_LENGTH };

	// Build the shellcode: NOPs + jmp rel32
	BYTE shellcode[32]{};
	memset(shellcode, NOP_OPCODE, instructionLength);
	shellcode[0] = JMP_REL32_OPCODE;
	*(DWORD*)(shellcode + 1) = (DWORD)relativeHookAddress;

	// Write the shellcode to the target function
	return PatchMemory(targetFunctionAddress, shellcode, instructionLength);
}