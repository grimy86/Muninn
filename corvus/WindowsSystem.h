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
		WindowsSystem() = default;

		std::vector<Corvus::Process::ProcessEntry> m_processes32{};
		std::vector<Corvus::Process::ProcessEntry> m_processesNt{};
		Corvus::Backend::Backend32 m_backend32{};
		Corvus::Backend::BackendNt m_backendNt{};

	public:
		WindowsSystem(const WindowsSystem&) = delete;
		WindowsSystem& operator=(const WindowsSystem&) = delete;
		~WindowsSystem() = default;

		static WindowsSystem& GetInstance() noexcept;
		const std::vector<Corvus::Process::ProcessEntry>&
			GetProcessList(BOOL useNt = FALSE) const noexcept;

		BOOL UpdateProcessList32();
		BOOL UpdateProcessListNt();
		BOOL UpdateModuleList32();
	};
}