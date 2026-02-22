#include "ProcessController.h"
#include "IWindowsBackend.h"
#include "MemoryService.h"

namespace Corvus::Controller
{
	ProcessController& ProcessController::GetInstance() noexcept
	{
		static ProcessController instance;
		return instance;
	}

	const std::vector<Corvus::Object::ProcessEntry>&
		ProcessController::GetProcessList(BOOL useNt) const noexcept
	{
		return useNt ? m_processesNt : m_processes32;
	}

	BOOL ProcessController::UpdateProcessList32()
	{
		if (!m_processes32.empty())
			m_processes32.clear();

		m_processes32 = m_backend32.QueryProcesses();
		return !m_processes32.empty() ? TRUE : FALSE;
	}

	BOOL ProcessController::UpdateProcessListNt()
	{
		if (!m_processesNt.empty())
			m_processesNt.clear();

		m_processesNt = m_backendNt.QueryProcesses();
		return !m_processesNt.empty() ? TRUE : FALSE;
	}

	BOOL ProcessController::UpdateModuleList32()
	{
		for (Corvus::Object::ProcessEntry& pEntry : m_processes32)
		{
			if (!Corvus::Service::IsValidProcessId(pEntry.processId))
				continue;

			const ACCESS_MASK accessMasks[]
			{
				PROCESS_ALL_ACCESS,
				PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
				PROCESS_QUERY_LIMITED_INFORMATION
			};

			HANDLE hProcess{};
			Corvus::Object::ProcessObject winProc{};
			for (const ACCESS_MASK accessMask : accessMasks)
			{
				if (!winProc.Init(pEntry.processId, accessMask))
					continue;
			}

			if (!Corvus::Service::IsValidHandle(winProc.GetProcessHandle()))
				continue;

			pEntry.modules = m_backend32.QueryModules(winProc);
		}
		return TRUE;
	}
}