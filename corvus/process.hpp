#pragma once
#include <string>
#include <windows.h>
#include <TlHelp32.h>
#include <vector>
#include <unordered_set>
#include "memory.hpp"
#include "converter.hpp"

namespace corvus::process
{
    DWORD GetProcessIdByName(const std::string& processName);
    std::vector<std::string> GetVisibleProcesses();
    std::vector<std::string> GetFilteredProcesses();
    bool HasVisibleWindow(DWORD pid);

	class Win32_Process
	{
	private:
		// Members
		std::string _processName{};
		DWORD _processId{};
		DWORD _moduleBaseAddress{};
		HANDLE _processHandle{};

	public:
		// Getters
		inline std::string GetProcessName() const { return _processName; }
		inline DWORD GetProcessId() const { return _processId; }
		inline DWORD GetModuleBase() const { return _moduleBaseAddress; }
		inline HANDLE GetProcessHandle() const { return _processHandle; }

		// Constructor & Destructor
		Win32_Process(const std::string& processName);
		Win32_Process(const Win32_Process&) = delete; //No copy constructor
		~Win32_Process();

		// Operators
		Win32_Process& operator=(const Win32_Process&) = delete; //No copy assignment
	};
}