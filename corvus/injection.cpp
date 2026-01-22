#include "injection.hpp"

namespace corvus::injection
{
	bool SimpleInject(const std::string& dllPath, const std::string& procName)
	{
		// Guard clauses
		if (dllPath.empty() || procName.empty()) return false;

		// Loop five tries (10kms intervals) until the target process is running
		DWORD procId{};
		if (!procId)
		{
			for (int i = 0; i < 5 && !procId; i++)
			{
				procId = corvus::process::GetProcessIdByName(procName);
				if (!procId) Sleep(10000);
			}
			if (!procId) return false;
		}

		// Opens a PROCESS_ALL_ACCESS handle (read/write/thread creation, etc.) to the target process using the PID.
		HANDLE procHandle{ OpenProcess(PROCESS_ALL_ACCESS, 0, procId) };
		if (!corvus::memory::IsValidHandle(procHandle)) return false;


		// +1: Ensures null-termination.
		size_t pathSize{ dllPath.size() + 1 };
		// Allocates memory inside the remote process
		// pathSize > MAX_PATH: Allocate space for the DLL path(up to 260 characters).
		// MEM_COMMIT | MEM_RESERVE : Commit and reserve memory.
		// PAGE_READWRITE: Allows read and write access to the memory page.
		void* location{ VirtualAllocEx(procHandle, 0, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE) };
		if (!location)
		{
			CloseHandle(procHandle);
			return false;
		}

		// Writes the DLL path string into the memory we just allocated in the target process.
		if (!WriteProcessMemory(procHandle, location, dllPath.c_str(), pathSize, 0))
		{
			VirtualFreeEx(procHandle, location, 0, MEM_RELEASE);
			CloseHandle(procHandle);
			return false;
		}

		// Starts a thread inside the target process to load a DLL using LoadLibraryA as the LPTHREAD_START_ROUTINE.
		// lpParameter (A pointer to a variable to be passed to the thread function.) = the DLL path string inside remote memory.
		HANDLE threadHandle{
			CreateRemoteThread(procHandle, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, location, 0,0) };
		if (!corvus::memory::IsValidHandle(threadHandle))
		{
			VirtualFreeEx(procHandle, location, 0, MEM_RELEASE);
			CloseHandle(procHandle);
			return false;
		}

		// Wait for injection & clean up the memory we allocated in the remote process
		WaitForSingleObject(threadHandle, INFINITE);

		// LoadLibraryA didn't return properly, the DLL could be using FreeLibraryAndExitThread
		DWORD exitCode{};
		GetExitCodeThread(threadHandle, &exitCode);

		VirtualFreeEx(procHandle, location, 0, MEM_RELEASE);
		CloseHandle(threadHandle);
		CloseHandle(procHandle);
	}
}