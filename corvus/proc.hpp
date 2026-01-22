#pragma once
#include <string>
#include <windows.h>
#include <TlHelp32.h>
#include <vector>
#include <unordered_set>
#include "memory.hpp"
#include "converter.hpp"

namespace corvus::proc
{
    DWORD GetProcessIdByName(const std::string& processName);
    std::vector<std::string> GetVisibleProcesses();
    std::vector<std::string> GetFilteredProcesses();
    bool HasVisibleWindow(DWORD pid);
}