#pragma once
#include <Windows.h>
#include <string>

namespace Muninn::Model
{
	/// <summary>
	/// Flags struct @ LDR_DATA_TABLE_ENTRY, ntdll.h
	/// </summary>
	enum class KernelModuleFlags : DWORD
	{
		PackagedBinary = 0b1UL << 0,
		MarkedForRemoval = 0b1UL << 1,
		ImageDll = 0b1UL << 2,
		LoadNotificationsSent = 0b1UL << 3,
		TelemetryEntryProcessed = 0b1UL << 4,
		ProcessStaticImport = 0b1UL << 5,
		InLegacyLists = 0b1UL << 6,
		InIndexes = 0b1UL << 7,
		ShimDll = 0b1UL << 8,
		InExceptionTable = 0b1UL << 9,
		VerifierProvider = 0b1UL << 10, // 24H2
		ShimEngineCalloutSent = 0b1UL << 11, // 24H2
		LoadInProgress = 0b1UL << 12,
		LoadConfigProcessed = 0b1UL << 13, // WIN10
		EntryProcessed = 0b1UL << 14,
		ProtectDelayLoad = 0b1UL << 15, // WINBLUE
		AuxIatCopyPrivate = 0b1UL << 16, // 24H2
		ReservedFlags3 = 0b1UL << 17,
		DontCallForThreads = 0b1UL << 18,
		ProcessAttachCalled = 0b1UL << 19,
		ProcessAttachFailed = 0b1UL << 20,
		ScpInExceptionTable = 0b1UL << 21, // CorDeferredValidate before 24H2
		CorImage = 0b1UL << 22,
		DontRelocate = 0b1UL << 23,
		CorILOnly = 0b1UL << 24,
		ChpeImage = 0b1UL << 25, // RS4
		ChpeEmulatorImage = 0b1UL << 26, // WIN11
		ReservedFlags5 = 0b1UL << 27,
		Redirected = 0b1UL << 28,
		ReservedFlags6 = 0b11UL << 29, // 2 bits
		CompatDatabaseProcessed = 0b1UL << 31
	};

	/// <summary>
	/// MODULEENTRY32W, Tlhelp32.h data
	/// <para> MODULEINFO, Psapi.h data </para>
	/// <para> LDR_DATA_TABLE_ENTRY, ntdll.h data </para>
	/// </summary>
	struct ModuleModel
	{
		/// <summary>
		/// WCHAR szModule[MAX_MODULE_NAME32 + 1] @ MODULEENTRY32
		/// <para> UNICODE_STRING BaseDllName @ LDR_DATA_TABLE_ENTRY </para>
		/// </summary>
		std::wstring moduleName{};

		/// <summary>
		/// WCHAR szExePath[MAX_PATH] @ MODULEENTRY32
		/// <para> UNICODE_STRING FullDllName @ LDR_DATA_TABLE_ENTRY </para>
		/// </summary>
		std::wstring modulePath{};

		/// <summary>
		/// LPVOID lpBaseOfDll @ MODULEINFO
		/// <para> PVOID DllBase @ LDR_DATA_TABLE_ENTRY </para>
		/// </summary>
		uintptr_t moduleLoadAddress{};
		uintptr_t moduleEntryPoint{};
		uintptr_t moduleBaseAddress{};
		uintptr_t parentDllBaseAddress{};

		/// <summary>
		/// Use KernelModuleFlags structure for mapping
		/// </summary>
		DWORD kernelModuleFlags{};
		DWORD moduleImageSize{};
		DWORD processId{};
		WORD tlsIndex{};
	};
}