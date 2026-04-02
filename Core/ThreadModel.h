#pragma once
#include <Windows.h>

#ifndef UNDEFINED_PRIORITY_CLASS
#define UNDEFINED_PRIORITY_CLASS 0x00000000
#endif // !UNDEFINED_PRIORITY_CLASS

#ifndef KPRIORITY
#define KPRIORITY LONG
#endif // !KPRIORITY

namespace Muninn::Model
{
	enum class NativeThreadBasePriority : KPRIORITY
	{
		Idle = 0L,
		Lowest = 1L,
		BelowNormal = 2L,
		Normal = 8L,
		AboveNormal = 10L,
		Highest = 15L,
		TimeCritical = 31L,
		Unknown = 0xFF
	};

	/// <summary>
		/// THREADENTRY32 @ Tlhelp32.h data
		/// <para> SYSTEM_EXTENDED_THREAD_INFORMATION @ ntdll.h data </para>
		/// </summary>
	struct ThreadModel
	{
		/// <summary>
		///  PVOID StartAddress @ SYSTEM_THREAD_INFORMATION
		/// </summary>
		uintptr_t kernelThreadStartAddress{};

		/// <summary>
		///  PVOID Win32StartAddress @ SYSTEM_EXTENDED_THREAD_INFORMATION
		/// </summary>
		uintptr_t win32ThreadStartAddress{};

		/// <summary>
		/// PVOID TebBaseAddress @ SYSTEM_EXTENDED_THREAD_INFORMATION
		/// </summary>
		uintptr_t tebBaseAddress{};

		/// <summary>
		/// LONG tpBasePri @ THREADENTRY32
		/// <para> KPRIORITY BasePriority @ SYSTEM_THREAD_INFORMATION </para>
		/// </summary>
		KPRIORITY nativeThreadBasePriority{};

		/// <summary>
		/// DWORD th32ThreadID @ THREADENTRY32
		/// <para> CLIENT_ID ClientId @ SYSTEM_THREAD_INFORMATION</para>
		/// </summary>
		DWORD threadId{};
		DWORD threadOwnerProcessId{};
	};
}