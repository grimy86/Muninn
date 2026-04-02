#pragma once
#include "ProcessModel.h"
#include "InjectorModel.h"
#include <MuninnDal.h>

namespace Muninn::Controller
{
	/// <summary>
	/// Manages process object lifetime, initialization, population & state tracking.
	/// <para> Note that getters are not state tracked. </para>
	/// </summary>
	class ProcessController final
	{
	private:
		Muninn::Model::ProcessModel m_process{};
		Muninn::Model::InjectorModel m_injector{};
		HANDLE m_processHandle{ nullptr };

		bool SetProcessId(const DWORD processId) noexcept;
		bool SetProcessHandle(const ACCESS_MASK accessMask) noexcept;
		bool DisposeHandle();

	public:
		ProcessController() = default;
		ProcessController(
			const DWORD processId,
			const ACCESS_MASK accessMask);
		~ProcessController();

		// Delete copy constructor and copy assignment operator
		ProcessController(const ProcessController&) = delete;
		ProcessController& operator=(const ProcessController&) = delete;

		const bool InitializeProcessEntry() noexcept;
		const bool InitializeModuleList() noexcept;
		const bool InitializeThreadList() noexcept;
		const bool InitializeHandleList() noexcept;
		const bool SimpleInjectDll() noexcept;

		const Muninn::Model::ProcessModel& GetProcessObject() const noexcept;
		const Muninn::Model::InjectorModel& GetInjectorObject() const noexcept;
		const HANDLE& GetProcessHandle() const noexcept;
		const DWORD GetProcessId() const noexcept;

		static DWORD GetProcessId(
			const WCHAR* processName,
			bool& isRunning) noexcept;
	};
}