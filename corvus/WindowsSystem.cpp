#include "WindowsSystem.h"
#include "IProcessBackend.h"

namespace Corvus::System
{
	BOOL WindowsSystem::GetProcessList32()
	{
		if (!m_processes32.empty())
			m_processes32.clear();

		m_processes32 = m_backend32.QueryProcesses();
		return !m_processes32.empty() ? TRUE : FALSE;
	}

	BOOL WindowsSystem::GetProcessListNt()
	{
		if (!m_processesNt.empty())
			m_processesNt.clear();

		m_processesNt = m_backendNt.QueryProcesses();
		return !m_processesNt.empty() ? TRUE : FALSE;
	}
}