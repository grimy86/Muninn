#ifndef DATA_UTILITIES_H
#define DATA_UTILITIES_H

#include "DalConfig.h"
#include <stdbool.h>
#include <stdint.h>

MUNINN_API bool MUNINN_CALL
DAL_IsValidProcessId(
	_In_ const DWORD processId);

MUNINN_API
_Success_(return != FALSE)
bool MUNINN_CALL
DAL_IsValidHandle(
	_In_ const HANDLE handle);

MUNINN_API bool MUNINN_CALL
DAL_IsValidAddress(
	_In_ const uintptr_t address);

MUNINN_API bool MUNINN_CALL
DAL_IsValidLuid(
	_In_ const LUID luid);

MUNINN_API size_t MUNINN_CALL
DAL_MinSizeT(
	_In_ const size_t a,
	_In_ const size_t b);

MUNINN_API uint32_t MUNINN_CALL
DAL_MinU32(
	_In_ const uint32_t a,
	_In_ const uint32_t b);

#endif // !DATA_UTILITIES_H