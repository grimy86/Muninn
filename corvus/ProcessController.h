#pragma once
#include "WindowsStructures.h"
#include "ControllerState.h"
#include <Windows.h>
#include <vector>

namespace Corvus::Controller
{
	class ProcessController final
	{
	private:
		ProcessController() = default;

		Corvus::Object::ProcessObject m_process32{};
		Corvus::Object::ProcessObject m_processNt{};
		HANDLE m_processHandle{};
		HANDLE m_tokenHandle{};
		ControllerState m_state{ ControllerState::Uninitialized };

	public:
		// Delete copy constructor and copy assignment operator
		ProcessController(const ProcessController&) = delete;
		ProcessController& operator=(const ProcessController&) = delete;
		~ProcessController() = default;

		ControllerState& GetState() noexcept;

		bool Initialize(
			const DWORD processId,
			const ACCESS_MASK processAccessMask,
			const ACCESS_MASK tokenAccessMask);

		bool Dispose();
	};
}