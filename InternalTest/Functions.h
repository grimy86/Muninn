#pragma once
#include <cstdint>

template<typename ReturnType, typename... Args>
ReturnType call_cdecl(uintptr_t offset, Args&&... args)
{
    using Fn = ReturnType(__cdecl*)(Args...);
    return reinterpret_cast<Fn>(offset)(std::forward<Args>(args)...);
}

template<typename ReturnType, typename... Args>
ReturnType call_stdcall(uintptr_t offset, Args&&... args)
{
    using Fn = ReturnType(__stdcall*)(Args...);
    return reinterpret_cast<Fn>(offset)(std::forward<Args>(args)...);
}