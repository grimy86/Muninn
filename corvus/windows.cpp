#include "windows.hpp"

namespace corvus::windows
{
	WinProc::WinProc(const std::string& processName)
		: _processName{ processName }
	{
		_processId = corvus::proc::GetProcessIdByName(GetProcessName());
		_moduleBaseAddress = corvus::memory::GetModuleBaseAddress(_processId, processName);
		_processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _processId);
	}

	WinProc::~WinProc()
	{
		if (corvus::memory::IsValidHandle(_processHandle))
		{
			CloseHandle(_processHandle);
		};
	}
}