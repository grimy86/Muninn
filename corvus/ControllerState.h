#pragma once
#include <stdint.h>

namespace Corvus::Controller
{
	enum class ControllerState : uint8_t
	{
		Uninitialized,
		Initialized,
		Error,
		Disposed
	};
}