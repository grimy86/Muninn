#pragma once
#include <stdint.h>

namespace Corvus::Controller
{
	enum class ControllerState : uint8_t
	{
		Uninitialized,
		Initializing,
		Ready,
		Busy,
		Error,
		Disposed
	};
}