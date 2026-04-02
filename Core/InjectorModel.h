#pragma once
#include <Windows.h>

namespace Muninn::Model
{
	struct InjectorModel
	{
		bool IsInjected{ false };
		const char* DllPathA{ nullptr };
		const wchar_t* DllPathW{ nullptr };
		HMODULE ModuleHandle{ nullptr };
		// HANDLE InjectionThreadHandle{ nullptr };
	};
}