#include "ProcessController.h"
#include "WindowsProviderNt.h"
#include "MemoryService.h"

namespace Corvus::Controller
{
	ControllerState& ProcessController::GetState() noexcept
	{
		return m_state;
	}

	bool ProcessController::Initialize(
		const DWORD processId,
		const ACCESS_MASK processAccessMask,
		const ACCESS_MASK tokenAccessMask)
	{
		m_processHandle
			= Corvus::Data::OpenProcessHandleNt(processId, processAccessMask);
		if (!Corvus::Data::IsValidHandle(m_processHandle))
		{
			m_state = ControllerState::Error;
			return false;
		}

		m_tokenHandle
			= Corvus::Data::OpenProcessTokenHandleNt(m_processHandle, tokenAccessMask);
		if (!Corvus::Data::IsValidHandle(m_tokenHandle))
		{
			m_state = ControllerState::Error;
			return false;
		}

		m_state = ControllerState::Initialized;
		return true;
	}

	bool ProcessController::Dispose()
	{
		if (!Corvus::Data::IsValidHandle(m_processHandle))
		{
			m_state = ControllerState::Error;
			return false;
		}

		if (!Corvus::Data::IsValidHandle(m_tokenHandle))
		{
			m_state = ControllerState::Error;
			return false;
		}

		if (!Corvus::Data::CloseHandleNt(m_processHandle))
		{
			m_state = ControllerState::Error;
			return false;
		}

		if (!Corvus::Data::CloseHandleNt(m_tokenHandle))
		{
			m_state = ControllerState::Error;
			return false;
		}

		m_state = ControllerState::Disposed;
		return true;
	}
}