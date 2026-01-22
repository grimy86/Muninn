#pragma once
#include <Windows.h>
#include <string>
#include "process.hpp"
#include "memory.hpp"

namespace corvus::injection
{
    bool SimpleInject(const std::string& dllPath, const std::string& procName);
}