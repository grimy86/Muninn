#include "DataUtilities.h"

bool DAL_IsValidProcessId(const DWORD processId)
{
	return processId % 4ul == 0ul;
}

bool DAL_IsValidHandle(const HANDLE handle)
{
	return (handle != NULL &&
		handle != INVALID_HANDLE_VALUE);
}

bool DAL_IsValidAddress(const DWORD address)
{
	return address != ERROR_INVALID_ADDRESS;
}

bool DAL_IsValidLuid(const LUID luid)
{
	return (luid.HighPart != 0ul && luid.LowPart != 0ul);
}

size_t DAL_MinSizeT(const size_t a, const size_t b)
{
	return a < b ? a : b;
}

uint32_t DAL_MinU32(const uint32_t a, const uint32_t b)
{
	return (a < b) ? a : b;
}