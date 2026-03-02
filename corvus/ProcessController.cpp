#include "ProcessController.h"

namespace Corvus::Controller
{
	ControllerState& ProcessController::GetState() noexcept
	{
		return m_state;
	}

	bool ProcessController::Initialize(
		const DWORD processId,
		const ACCESS_MASK accessMask)
	{
		static ProcessController instance;
		return instance;
	}

	void Dispose()
	{

	}

	void refresh()
	{

	}
}