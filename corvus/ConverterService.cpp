#include "ConverterService.h"

namespace Corvus::Controller
{
	const char* ToString(Corvus::Object::UserProcessBasePriorityClass p)
	{
		switch (p)
		{
		case UserProcessBasePriorityClass::Undefined:   return "Undefined";
		case UserProcessBasePriorityClass::Idle:        return "Idle";
		case UserProcessBasePriorityClass::Normal:      return "Normal";
		case UserProcessBasePriorityClass::High:        return "High";
		case UserProcessBasePriorityClass::Realtime:    return "Realtime";
		case UserProcessBasePriorityClass::BelowNormal: return "BelowNormal";
		case UserProcessBasePriorityClass::AboveNormal: return "AboveNormal";
		}
		return "Unknown";
	}
}