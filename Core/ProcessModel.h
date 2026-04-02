#pragma once
#include "ModuleModel.h"
#include "ThreadModel.h"
#include "HandleModel.h"
#include "AccessTokenModel.h"

namespace Muninn::Model
{
	enum class ArchitectureType : uint8_t
	{
		Unknown,
		x86,
		x64
	};

	/// <summary>
	/// PROCESSENTRY32W @ Tlhelp32.h
	/// <para> QueryFullProcessImageNameW @ WinBase.h </para>
	/// <para> SYSTEM_PROCESS_INFORMATION @ ntdll.h </para>
	/// <para> PROCESS_EXTENDED_BASIC_INFORMATION @ ntdll.h </para>
	/// </summary>
	struct ProcessEntry
	{
		/// <summary>
		/// The file name of the executable image.
		/// <para> WCHAR szExeFile[MAX_PATH] @ PROCESSENTRY32W </para>
		/// <para> UNICODE_STRING ImageName @ SYSTEM_PROCESS_INFORMATION </para>
		/// </summary>
		std::wstring processName{};

		/// <summary>
		/// LPWSTR lpExeName @ QueryFullProcessImageNameW()
		/// </summary>
		std::wstring userFullProcessImageName{};

		/// <summary>
		/// UNICODE_STRING SystemInformation @ NtQueryInformationProcess()
		/// <para> Arg: PROCESSINFOCLASS::ProcessImageFileName (27) </para>
		/// </summary>
		std::wstring NativeImageFileName{};
		uintptr_t pebBaseAddress{};
		uintptr_t moduleBaseAddress{};
		DWORD processId{};
		DWORD parentProcessId{};
		BOOL isProtectedProcess{};

		/// <summary>
		/// Indicates that the process is 32-bit and runs under the WoW64 emulation.
		/// </summary>
		BOOL isWow64Process{};

		/// <summary>
		/// The process belongs to a background job.
		/// </summary>
		BOOL isBackgroundProcess{};

		/// <summary>
		/// The process runs in Isolated User Mode (IUM).
		/// </summary>
		BOOL isSecureProcess{};

		/// <summary>
		/// The process is a Pico or a WSL process.
		/// </summary>
		BOOL isSubsystemProcess{};
		BOOL hasVisibleWindow{};
		ArchitectureType architectureType{};
	};

	struct ProcessModel
	{
		std::vector<ModuleModel> moduleList{};
		std::vector<ThreadModel> threadList{};
		std::vector<HandleModel> handleList{};
		ProcessEntry processEntry{};
		AccessTokenModel accessToken{};
		HANDLE processHandle{ nullptr };
	};
}