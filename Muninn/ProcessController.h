#pragma once
#include "WindowsModels.h"
#include <MuninnApi.h>

namespace Muninn::Controller
{
	/// <summary>
	/// Manages process object lifetime, initialization, population & state tracking.
	/// <para> Note that getters are not state tracked. </para>
	/// </summary>
	class ProcessController final
	{
	private:
		Muninn::Models::ProcessObject m_process32{};
		Muninn::Models::ProcessObject m_processNt{};
		HANDLE m_processHandle{ nullptr };

		bool InitializeHandle(
			const DWORD processId,
			const ACCESS_MASK accessMask);

		bool DisposeHandle();

		bool InitializeProcessEntry32(const DWORD processId);

	public:
		ProcessController() = default;
		ProcessController(const DWORD processId, const ACCESS_MASK processAccessMask);
		~ProcessController();

		// Delete copy constructor and copy assignment operator
		ProcessController(const ProcessController&) = delete;
		ProcessController& operator=(const ProcessController&) = delete;

		const Muninn::Models::ProcessObject& GetProcessObject32() const noexcept;
		const Muninn::Models::ProcessObject& GetProcessObjectNt() const noexcept;
		const HANDLE& GetProcessHandle() const noexcept;
	};
}