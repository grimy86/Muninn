#include "memory.hpp"

namespace corvus::memory
{
	uintptr_t GetModuleBaseAddress(const DWORD& processId, const std::string& moduleName)
	{
		// Take a snapshot of 32 & 64-bit modules
		HANDLE hSnapShotHandle{ CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId) };
		DWORD moduleBaseAddress{};

		if (!IsValidHandle(hSnapShotHandle))
		{
			return moduleBaseAddress;
		}

		MODULEENTRY32 modEntry{};
		modEntry.dwSize = sizeof(MODULEENTRY32);
		if (!Module32First(hSnapShotHandle, &modEntry))
		{
			CloseHandle(hSnapShotHandle);
			return moduleBaseAddress;
		}

		do
		{
			if (!_wcsicmp(modEntry.szModule, corvus::converter::StringToWString(moduleName).c_str()))
			{
				CloseHandle(hSnapShotHandle);
				return reinterpret_cast<DWORD>(modEntry.modBaseAddr);
			}
		} while (Module32Next(hSnapShotHandle, &modEntry));

		CloseHandle(hSnapShotHandle);
		return moduleBaseAddress;
	}

	void PatchExecution(HANDLE processHandle, DWORD destination, BYTE* value, unsigned int size)
	{
		// Changes the protection on a region of committed pages in the virtual address space of a specified process.
		// https://learn.microsoft.com/en-us/windows/win32/Memory/memory-protection-constants

		DWORD oldPageProtection;
		VirtualProtectEx(processHandle, (void*)destination, size, PAGE_EXECUTE_READWRITE, &oldPageProtection);
		WriteProcessMemory(processHandle, (void*)destination, value, size, nullptr);
	}

	void NopExecution(HANDLE processHandle, DWORD destination, unsigned int size)
	{
		// Filling an array with x86 NOP instructions (0x90)
		BYTE* noOperationArray = new BYTE[size];
		memset(noOperationArray, 0x90, size);

		corvus::memory::PatchExecution(processHandle, destination, noOperationArray, size);
		delete[] noOperationArray;
	}

	// Find multi-level pointers (external)
	DWORD FindDMAAddy(HANDLE processHandle, DWORD ptr, std::vector<DWORD> offsets)
	{
		DWORD addr{ ptr };
		for (unsigned int i = 0; i < offsets.size(); ++i)
		{
			ReadProcessMemory(processHandle, (void*)addr, &addr, sizeof(addr), nullptr);
			addr += offsets[i];
		}
		return addr;
	}

	//Internal patch function, uses VirtualProtect instead of VirtualProtectEx
	void PatchExecutionI(DWORD destination, BYTE* value, unsigned int size)
	{
		DWORD oldPageProtection;
		VirtualProtect((void*)destination, size, PAGE_EXECUTE_READWRITE, &oldPageProtection);
		memcpy((void*)destination, value, size);
		VirtualProtect((void*)destination, size, oldPageProtection, &oldPageProtection);
	}

	//Internal nop function, uses memset instead of WPM
	void NopExecutionI(DWORD destination, unsigned int size)
	{
		DWORD oldPageProtection;
		VirtualProtect((void*)destination, size, PAGE_EXECUTE_READWRITE, &oldPageProtection);
		memset((void*)destination, 0x90, size);
		VirtualProtect((void*)destination, size, oldPageProtection, &oldPageProtection);
	}

	DWORD FindDMAAddyI(DWORD ptr, std::vector<DWORD> offsets)
	{
		DWORD addr{ ptr };
		for (unsigned int i = 0; i < offsets.size(); ++i)
		{
			addr = *(DWORD*)addr;
			addr += offsets[i];
		}
		return addr;
	}

	bool IsValidProcId(const DWORD procId)
	{
		return procId != 0;
	}

	bool IsValidModuleBaseAddress(const DWORD moduleBaseAddr)
	{
		return moduleBaseAddr != 0;
	}

	bool IsValidHandle(const HANDLE handle)
	{
		return (handle != nullptr && handle != INVALID_HANDLE_VALUE);
	}
}