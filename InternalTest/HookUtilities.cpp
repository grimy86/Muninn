#include "HookUtilities.h"
#include "Globals.h"

#ifndef NOP_OPCODE
#define NOP_OPCODE 0x90
#endif // !NOP

#ifndef JMP_REL32_OPCODE
#define JMP_REL32_OPCODE 0xE9
#endif // !JMP_REL32_OPCODE

#ifndef JMP_REL32_LENGTH
#define JMP_REL32_LENGTH 5 // bytes
#endif // !JMP_LENGTH

#ifndef MEM_PAGE
#define MEM_PAGE 0x1000
#endif // !MEM_PAGE

bool PatchInstructions(
	const uintptr_t targetAddress,
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

bool InstallHook(
	const uintptr_t targetFunctionAddress,
	const uintptr_t hookFunctionAddress,
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
	return PatchInstructions(targetFunctionAddress, shellcode, instructionLength);
}

bool InstallTrampolineHook(
	const uintptr_t targetFunctionAddress,
	const uintptr_t hookFunctionAddress,
	const size_t instructionLength) noexcept
{
#ifdef _WIN64
	return false;
#endif

	// Allocate executable memory for the gateway
	BYTE* gateway{ (BYTE*)VirtualAlloc(
		0,
		MEM_PAGE,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE) };

	if (!gateway)
		return false;

	// Steal the orginal bytes and move them to the gateway
	memcpy(
		gateway,
		(LPVOID)targetFunctionAddress,
		instructionLength);

	// Place the jump back instruction at the end of the gateway
	gateway[instructionLength] = JMP_REL32_OPCODE;

	// Calculate the jump back address to the original function
	int jumpBackAddress{
		(intptr_t)(targetFunctionAddress + instructionLength) -
		(intptr_t)&(gateway[instructionLength + JMP_REL32_LENGTH]) };

	// Set the jump back address in the gateway
	*(DWORD*)&gateway[instructionLength + 1] =
		(DWORD)jumpBackAddress;

	// Set the global gateway variable
	AssaultCube::gateway = gateway;

	bool status{ InstallHook(
		targetFunctionAddress,
		hookFunctionAddress,
		instructionLength) };

	return status;
}

// in HookUtilities.cpp
bool InstallTrampolineHook(
	const uintptr_t targetFunctionAddress,
	const uintptr_t hookFunctionAddress,
	const size_t instructionLength,
	BYTE** outGateway) noexcept
{
	// same as your existing implementation but also set outGateway
	BYTE* gateway = (BYTE*)VirtualAlloc(0, MEM_PAGE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!gateway) return false;

	memcpy(gateway, (LPVOID)targetFunctionAddress, instructionLength);
	gateway[instructionLength] = JMP_REL32_OPCODE;

	int jumpBackAddress{
		(intptr_t)(targetFunctionAddress + instructionLength) -
		(intptr_t) & (gateway[instructionLength + JMP_REL32_LENGTH]) };

	*(DWORD*)&gateway[instructionLength + 1] = (DWORD)jumpBackAddress;

	if (outGateway) *outGateway = gateway; // return gateway to caller
	AssaultCube::gateway = gateway;

	return InstallHook(targetFunctionAddress, hookFunctionAddress, instructionLength);
}