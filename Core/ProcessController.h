#pragma once
#include "WindowsModels.h"
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
		Muninn::Models::ProcessObject m_process{};
		HANDLE m_processHandle{ nullptr };

		bool InitializeProcessId(const DWORD processId) noexcept;
		bool InitializeProcessHandle(const ACCESS_MASK accessMask) noexcept;
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

		const Muninn::Models::ProcessObject& GetProcessObject() const noexcept;
		const HANDLE& GetProcessHandle() const noexcept;
		static DWORD GetProcessId(const WCHAR* processName) noexcept;
	};
}