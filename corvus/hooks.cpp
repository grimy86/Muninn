#include "hooks.hpp"

namespace corvus::hooks
{
	/// <summary>
	/// Installs an inline detour hook (x86).
	/// Overwrites the first few bytes of a function with a JMP to your hook.
	/// </summary>
	/// <param name="originalFunc">Pointer to the function you want to hook.</param>
	/// <param name="hookFunc">Pointer to your custom hook function.</param>
	/// <param name="opcodeLength">
	/// Number of bytes to overwrite. Must be ≥ 5 (5 bytes = JMP rel32 instruction).
	/// </param>
	/// <param name="outHookInfo">A HookInfo struct that will be filled out.</param>
	/// <returns>True if the hook was successfully installed.</returns>
	bool InstallInlineHook(void* originalFunctionToHook, void* hookFunction, int opcodeLength, corvus::hooks::HookInfo& outHookInfo)
	{
		if (opcodeLength < 5) return false; // JMP rel32 = 5 bytes

		DWORD oldProtect;
		if (!VirtualProtect(originalFunctionToHook, opcodeLength, PAGE_EXECUTE_READWRITE, &oldProtect))
			return false;

		// NOP the region for safety in case we overwrite more than 5 bytes
		memset(originalFunctionToHook, 0x90, opcodeLength);

		// Calculate relative offset for JMP rel32 (target - source - 5), -5 OR - sizeof(opcodeLength)?
		DWORD relativeAddress = (DWORD)hookFunction - (DWORD)originalFunctionToHook - 5;

		// Write JMP instruction
		*(BYTE*)originalFunctionToHook = 0xE9;
		*(DWORD*)((DWORD)originalFunctionToHook + 1) = relativeAddress;

		// Restore memory protection
		DWORD temp;
		VirtualProtect(originalFunctionToHook, opcodeLength, oldProtect, &temp);

		// Fill out hook information
		outHookInfo.originalFunc = originalFunctionToHook;
		outHookInfo.hookFunc = hookFunction;
		outHookInfo.returnAddress = (void*)((DWORD)originalFunctionToHook + opcodeLength);
		outHookInfo.overwrittenBytes = opcodeLength;

		return true;
	}
}