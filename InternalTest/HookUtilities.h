#pragma once
#include <cstdint>
#include <Windows.h>

bool PatchInstructions(
	const uintptr_t targetAddress,
	const BYTE* const shellcode,
	const size_t byteLength) noexcept;

bool InstallHook(
	const uintptr_t targetFunctionAddress,
	const uintptr_t hookFunctionAddress,
	const size_t instructionLength) noexcept;

bool InstallTrampolineHook(
	const uintptr_t targetFunctionAddress,
	const uintptr_t hookFunctionAddress,
	const size_t instructionLength) noexcept;

bool InstallTrampolineHook(
	const uintptr_t targetFunctionAddress,
	const uintptr_t hookFunctionAddress,
	const size_t instructionLength,
	BYTE** outGateway) noexcept;