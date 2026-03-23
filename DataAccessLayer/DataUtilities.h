#ifndef DATA_UTILITIES_H
#define DATA_UTILITIES_H

#include <phnt_windows.h>
#include <stdbool.h>
#include <stdint.h>

bool DAL_IsValidProcessId(const DWORD processId);
bool DAL_IsValidHandle(const HANDLE processHandle);
bool DAL_IsValidAddress(const DWORD moduleBaseAddress);
bool DAL_IsValidLuid(const LUID luid);
size_t DAL_MinSizeT(const size_t a, const size_t b);
uint32_t DAL_MinU32(const uint32_t a, const uint32_t b);

#endif // !DATA_UTILITIES_H