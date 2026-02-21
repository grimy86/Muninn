#include "WindowsSystem.h"
#include "IProcessBackend.h"
#include "MemoryService.h"

namespace Corvus::System
{
	WindowsSystem& WindowsSystem::GetInstance() noexcept
	{
		static WindowsSystem instance;
		return instance;
	}

	const std::vector<Corvus::Process::ProcessEntry>&
		WindowsSystem::GetProcessList(BOOL useNt) const noexcept
	{
		return useNt ? m_processesNt : m_processes32;
	}

	BOOL WindowsSystem::UpdateProcessList32()
	{
		if (!m_processes32.empty())
			m_processes32.clear();

		m_processes32 = m_backend32.QueryProcesses();
		return !m_processes32.empty() ? TRUE : FALSE;
	}

	BOOL WindowsSystem::UpdateProcessListNt()
	{
		if (!m_processesNt.empty())
			m_processesNt.clear();

		m_processesNt = m_backendNt.QueryProcesses();
		return !m_processesNt.empty() ? TRUE : FALSE;
	}

	BOOL WindowsSystem::UpdateModuleList32()
	{
		for (Corvus::Process::ProcessEntry& pEntry : m_processes32)
		{
			if (!Corvus::Memory::IsValidProcessId(pEntry.processId))
				continue;

			const ACCESS_MASK accessMasks[]
			{
				PROCESS_ALL_ACCESS,
				PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
				PROCESS_QUERY_LIMITED_INFORMATION
			};

			HANDLE hProcess{};
			Corvus::Process::WindowsProcess winProc{};
			for (const ACCESS_MASK accessMask : accessMasks)
			{
				if (!winProc.Init(pEntry.processId, accessMask))
					continue;
			}

			if (!Corvus::Memory::IsValidHandle(winProc.GetProcessHandle()))
				continue;

			pEntry.modules = m_backend32.QueryModules(winProc);
		}
		return TRUE;
	}
}