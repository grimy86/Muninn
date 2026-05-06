#ifndef MEMORY_PROVIDER_H
#define MEMORY_PROVIDER_H

#include "DalConfig.h"
#include <stdbool.h>
#include <stddef.h>

#ifndef u8_t
typedef unsigned char u8_t;
#endif

#ifndef byte_t
typedef unsigned char byte_t;
#endif

#ifndef u16_t
typedef unsigned short u16_t;
#endif

#ifndef u32_t
typedef unsigned long u32_t;
#endif

#ifndef u64_t
typedef unsigned long long u64_t;
#endif

#ifndef uptr_t
#  if defined(_WIN64) || defined(__x86_64__)
typedef unsigned long long uptr_t;
#  else
typedef unsigned long uptr_t;
#  endif
#endif

MUNINN_API bool MUNINN_CALL
PatchInstructions(
    _In_ void* targetAddress,
    _In_ const byte_t* patchBytes,
    _In_ size_t byteCount);

#endif // !MEMORY_PROVIDER_H

