#pragma once
#include <cstdint>
#include <Windows.h>

bool PatchMemory(
	uintptr_t const targetAddress,
	const BYTE* const shellcode,
	const size_t byteLength) noexcept;

bool HookFunction(
	uintptr_t const targetFunctionAddress,
	uintptr_t const hookFunctionAddress,
	const size_t instructionLength) noexcept;