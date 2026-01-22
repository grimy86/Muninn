#include "process.hpp"

namespace corvus::process
{
	DWORD GetProcessIdByName(const std::string& processName)
	{
		//Handle to the snapshot of all processes
		HANDLE hSnapShotHandle{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) };
		DWORD procId{};

		if (!corvus::memory::IsValidHandle(hSnapShotHandle))
		{
			return procId;
		}

		// Process object(s) buffer
		// If you do not initialize dwSize, Process32First fails.
		PROCESSENTRY32 procEntry{};
		procEntry.dwSize = sizeof(PROCESSENTRY32);

		// Retrieves information about the first process encountered in a system snapshot.
		// Returns TRUE if the first entry of the process list has been copied to the buffer or FALSE otherwise.
		if (!Process32First(hSnapShotHandle, &procEntry))
		{
			CloseHandle(hSnapShotHandle);
			return procId;
		}

		// Use tail-controlled loop to run once before the condition check
		do
		{
			// Copy the next process into procEntry
			// wchar string compare case incensitive
			if (!_wcsicmp(procEntry.szExeFile, corvus::converter::StringToWString(processName).c_str()))
			{
				CloseHandle(hSnapShotHandle);
				return procEntry.th32ProcessID;
			}
		} while (Process32Next(hSnapShotHandle, &procEntry));

		CloseHandle(hSnapShotHandle);
		return procId;
	}

	std::vector<std::string> GetVisibleProcesses()
	{
		std::vector<std::string> processList;
		std::unordered_set<std::wstring> seenExeNames;

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE)
			return processList;

		PROCESSENTRY32 entry = {};
		entry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(snapshot, &entry))
		{
			do
			{
				if (wcslen(entry.szExeFile) == 0)
					continue;

				std::wstring exeName = entry.szExeFile;

				// Skip duplicates
				if (seenExeNames.count(exeName))
					continue;

				// Only include processes with visible windows
				if (!HasVisibleWindow(entry.th32ProcessID))
					continue;

				seenExeNames.insert(exeName);

				char buffer[256];
				sprintf_s(buffer, "%lu - %ws", entry.th32ProcessID, entry.szExeFile);
				processList.emplace_back(buffer);

			} while (Process32Next(snapshot, &entry));
		}

		CloseHandle(snapshot);
		return processList;
	}

	std::vector<std::string> GetFilteredProcesses()
	{
		std::vector<std::string> processList;
		std::unordered_set<DWORD> seenPids;

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE)
			return processList;

		PROCESSENTRY32 entry = {};
		entry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(snapshot, &entry))
		{
			do
			{
				if (wcslen(entry.szExeFile) == 0)
					continue;

				// Filter by session (skip system)
				DWORD sessionId = 0;
				if (!ProcessIdToSessionId(entry.th32ProcessID, &sessionId) || sessionId == 0)
					continue;

				// Skip known system executables
				std::wstring exeName = entry.szExeFile;
				if (exeName == L"System" || exeName == L"Idle" || exeName == L"svchost.exe")
					continue;

				// Remove duplicate PIDs
				if (seenPids.count(entry.th32ProcessID))
					continue;

				seenPids.insert(entry.th32ProcessID);

				char buffer[256];
				sprintf_s(buffer, "%lu - %ws", entry.th32ProcessID, entry.szExeFile);
				processList.emplace_back(buffer);

			} while (Process32Next(snapshot, &entry));
		}

		CloseHandle(snapshot);
		return processList;
	}

	bool HasVisibleWindow(DWORD pid)
	{
		HWND hwnd = GetTopWindow(NULL);
		while (hwnd)
		{
			DWORD winPid = 0;
			GetWindowThreadProcessId(hwnd, &winPid);

			if (winPid == pid && IsWindowVisible(hwnd))
				return true;

			hwnd = GetNextWindow(hwnd, GW_HWNDNEXT);
		}
		return false;
	}

	Win32_Process::Win32_Process(const std::string& processName)
		: _processName{ processName }
	{
		_processId = corvus::process::GetProcessIdByName(GetProcessName());
		_moduleBaseAddress = corvus::memory::GetModuleBaseAddress(_processId, processName);
		_processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _processId);
	}

	Win32_Process::~Win32_Process()
	{
		if (corvus::memory::IsValidHandle(_processHandle))
		{
			CloseHandle(_processHandle);
		};
	}
}