#ifndef DATA_UTILITIES_H
#define DATA_UTILITIES_H

#include "DalConfig.h"
#include <phnt_windows.h>
#include <stdbool.h>
#include <stdint.h>

MUNINN_API bool MUNINN_CALL
DAL_IsValidProcessId(const DWORD processId);

MUNINN_API bool MUNINN_CALL
DAL_IsValidHandle(const HANDLE handle);

MUNINN_API bool MUNINN_CALL
DAL_IsValidAddress(const DWORD address);

MUNINN_API bool MUNINN_CALL
DAL_IsValidLuid(const LUID luid);

MUNINN_API size_t MUNINN_CALL
DAL_MinSizeT(const size_t a, const size_t b);

MUNINN_API uint32_t MUNINN_CALL
DAL_MinU32(const uint32_t a, const uint32_t b);

#endif // !DATA_UTILITIES_H