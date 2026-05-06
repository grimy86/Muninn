#include "MemoryProvider.h"

MUNINN_API bool MUNINN_CALL
PatchInstructions(
	_In_ void* targetAddress,
	_In_ const byte_t* patchBytes,
	_In_ size_t byteCount)
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