#pragma once
#include <Windows.h>
#include <string>
#include "memory.hpp"
#include "proc.hpp"

namespace corvus::windows
{
	class WinProc
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
		WinProc(const std::string& processName);
		WinProc(const WinProc&) = delete; //No copy constructor
		~WinProc();

		// Operators
		WinProc& operator=(const WinProc&) = delete; //No copy assignment
	};
}