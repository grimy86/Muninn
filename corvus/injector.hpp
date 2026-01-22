#pragma once
#include <Windows.h>
#include <string>
#include "proc.hpp"
#include "memory.hpp"

namespace corvus::injector
{
    uintptr_t GetModuleBaseAddress(const DWORD& processId, const std::string& moduleName);
}