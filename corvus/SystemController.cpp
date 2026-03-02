#include "SystemController.h"

namespace Corvus::Controller
{
	SystemController& SystemController::GetInstance() noexcept
	{
		static SystemController instance;
		return instance;
	}
}