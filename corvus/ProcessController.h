#pragma once
#include "ProcessController.h"
#include "Backend32.h"
#include "BackendNt.h"
#include <vector>
#include <memory>

namespace Corvus::Controller
{
	class ProcessController final
	{
	private:
		ProcessController() = default;

		std::vector<ProcessEntry> m_processes32{};
		std::vector<ProcessEntry> m_processesNt{};
		Corvus::Data::Backend32 m_backend32{};
		Corvus::Data::BackendNt m_backendNt{};

	public:
		ProcessController(const ProcessController&) = delete;
		ProcessController& operator=(const ProcessController&) = delete;
		~ProcessController() = default;

		static ProcessController& GetInstance() noexcept;
		const std::vector<ProcessEntry>&
			GetProcessList(BOOL useNt = FALSE) const noexcept;

		BOOL UpdateProcessList32();
		BOOL UpdateProcessListNt();
		BOOL UpdateModuleList32();
	};
}