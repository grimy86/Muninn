#pragma once
#include "WindowsProcess.h"
#include "Backend32.h"
#include "BackendNt.h"
#include <vector>
#include <memory>

namespace Corvus::System
{
	class WindowsSystem final
	{
	private:
		std::vector<Corvus::Process::ProcessEntry> m_processes32{};
		std::vector<Corvus::Process::ProcessEntry> m_processesNt{};
		Corvus::Backend::Backend32 m_backend32{};
		Corvus::Backend::BackendNt m_backendNt{};

	public:
		WindowsSystem() = default;
		~WindowsSystem() = default;

		BOOL GetProcessList32();
		BOOL GetProcessListNt();
	};
}