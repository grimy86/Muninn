#include "ConverterService.h"

namespace Corvus::Controller
{
	const char* GetPriorityClassA(Corvus::Object::UserProcessBasePriorityClass p)
	{
		switch (p)
		{
		case Corvus::Object::UserProcessBasePriorityClass::Undefined:   return "Undefined";
		case Corvus::Object::UserProcessBasePriorityClass::Idle:        return "Idle";
		case Corvus::Object::UserProcessBasePriorityClass::Normal:      return "Normal";
		case Corvus::Object::UserProcessBasePriorityClass::High:        return "High";
		case Corvus::Object::UserProcessBasePriorityClass::Realtime:    return "Realtime";
		case Corvus::Object::UserProcessBasePriorityClass::BelowNormal: return "BelowNormal";
		case Corvus::Object::UserProcessBasePriorityClass::AboveNormal: return "AboveNormal";
		}
		return "Unknown";
	}
}