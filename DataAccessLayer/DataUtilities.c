#include "DataUtilities.h"

MUNINN_API bool MUNINN_CALL
DAL_IsValidProcessId(const DWORD processId)
{
	return processId % 4ul == 0ul;
}

MUNINN_API bool MUNINN_CALL
DAL_IsValidHandle(const HANDLE handle)
{
	return (handle != NULL &&
		handle != INVALID_HANDLE_VALUE);
}

MUNINN_API bool MUNINN_CALL
DAL_IsValidAddress(const DWORD address)
{
	return address != ERROR_INVALID_ADDRESS;
}

MUNINN_API bool MUNINN_CALL
DAL_IsValidLuid(const LUID luid)
{
	return (luid.HighPart != 0ul && luid.LowPart != 0ul);
}

MUNINN_API size_t MUNINN_CALL
DAL_MinSizeT(const size_t a, const size_t b)
{
	return a < b ? a : b;
}

MUNINN_API uint32_t MUNINN_CALL
DAL_MinU32(const uint32_t a, const uint32_t b)
{
	return (a < b) ? a : b;
}