#pragma once
#include "ProcessModel.h"

namespace Muninn::Model
{
	struct SystemObject
	{
		std::vector<ProcessModel> processList{};
	};
}