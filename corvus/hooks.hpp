#pragma once
#include <windows.h>

namespace corvus::hooks
{
    struct HookInfo
    {
        void* originalFunc;
        void* hookFunc;
        void* returnAddress;
        int   overwrittenBytes;
    };

    bool InstallInlineHook(void* originalFunctionToHook,
        void* hookFunction,
        int opcodeLength,
        HookInfo& outHookInfo);
}