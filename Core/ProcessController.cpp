#include "ProcessController.h"
#undef NTSTATUS // Undefine the WindowsProvider32.h definition
#include <ntdef.h> // NTSTATUS codes

namespace Muninn::Controller
{
	ProcessController::ProcessController(
		const DWORD processId,
		const ACCESS_MASK accessMask)
	{
		if (!InitializeProcessId(processId))
			return;

		if (!InitializeProcessHandle(accessMask))
			return;
	}

	bool ProcessController::InitializeProcessId(
		const DWORD processId) noexcept
	{
		if (!DAL_IsValidProcessId(processId))
			return false;
		m_process.processEntry.processId = processId;
	}

	bool ProcessController::InitializeProcessHandle(
		const ACCESS_MASK accessMask) noexcept
	{
		if (!DAL_IsValidProcessId(m_process.processEntry.processId))
			return false;

		if (!DAL_IsValidHandle(m_processHandle))
		{
			NTSTATUS status{ DAL_OpenProcessHandleNt(
			m_process.processEntry.processId,
			accessMask,
			&m_processHandle) };

			if (!NT_SUCCESS(status) || !DAL_IsValidHandle(m_processHandle))
				return false;
		}

		return true;
	}

	const bool ProcessController::InitializeProcessEntry() noexcept
	{
		if (!DAL_IsValidProcessId(m_process.processEntry.processId))
			return false;
		if (!DAL_IsValidHandle(m_processHandle))
			return false;

		PROCESSENTRY32W processEntry32{};
		PROCESS_EXTENDED_BASIC_INFORMATION processInfo{};

		NTSTATUS status{ DAL_GetProcessInformation32(
			m_process.processEntry.processId,
			&processEntry32) };
		if (!NT_SUCCESS(status))
			return false;

		m_process.processEntry.processName =
			processEntry32.szExeFile;

		status = DAL_GetImageFileNameWin32Nt(
			m_processHandle,
			m_process.processEntry.userFullProcessImageName.data(),
			MAX_PATH,
			NULL);
		if (!NT_SUCCESS(status))
			return false;

		status = DAL_GetImageFileNameNt(
			m_processHandle,
			m_process.processEntry.NativeImageFileName.data(),
			MAX_PATH,
			NULL);
		if (!NT_SUCCESS(status))
			return false;

		status = DAL_GetPebBaseAddressAndProcessInfoNt(
			m_processHandle,
			&m_process.processEntry.pebBaseAddress,
			&processInfo);
		if (!NT_SUCCESS(status))
			return false;

		status = DAL_GetModuleBaseAddressFromPebBaseAddressNt(
			m_processHandle,
			&m_process.processEntry.pebBaseAddress,
			&m_process.processEntry.moduleBaseAddress);
		if (!NT_SUCCESS(status))
			return false;

		m_process.processEntry.parentProcessId =
			static_cast<DWORD>(
				reinterpret_cast<uintptr_t>(
					processInfo.InheritedFromUniqueProcessId));

		m_process.processEntry.isProtectedProcess =
			processInfo.IsProtectedProcess;
		m_process.processEntry.isWow64Process =
			processInfo.IsWow64Process;
		m_process.processEntry.isBackgroundProcess =
			processInfo.IsBackground;
		m_process.processEntry.isSecureProcess =
			processInfo.IsSecureProcess;
		m_process.processEntry.isSubsystemProcess =
			processInfo.IsSubsystemProcess;

		status = DAL_GetWindowVisibility32(
			m_process.processEntry.processId,
			&m_process.processEntry.hasVisibleWindow);
		if (!NT_SUCCESS(status))
			return false;

		USHORT processMachine{};
		USHORT nativeMachine{};
		BOOL isWow64{};

		status = DAL_GetProcessArchitecture32(
			m_processHandle,
			&processMachine,
			&nativeMachine,
			&isWow64);
		if (!NT_SUCCESS(status))
			return false;

		switch (processMachine)
		{
		case(IMAGE_FILE_MACHINE_I386):
			m_process.processEntry.architectureType = Muninn::Models::ArchitectureType::x86;
			break;
		case(IMAGE_FILE_MACHINE_AMD64):
			m_process.processEntry.architectureType = Muninn::Models::ArchitectureType::x64;
			break;
		default:
			m_process.processEntry.architectureType = Muninn::Models::ArchitectureType::Unknown;
			break;
		}

		return true;
	}

	ProcessController::~ProcessController()
	{
		if (!DisposeHandle());
	}

	bool ProcessController::DisposeHandle()
	{
		if (!DAL_IsValidHandle(m_processHandle))
			return true;

		if (!DAL_CloseHandleNt(m_processHandle))
			return false;

		m_processHandle = nullptr;
		return true;
	}



	const Muninn::Models::ProcessObject&
		ProcessController::GetProcessObject() const noexcept
	{
		return m_process;
	}

	const HANDLE& ProcessController::GetProcessHandle() const noexcept
	{
		return m_processHandle;
	}

	DWORD ProcessController::GetProcessId(const WCHAR* processName) noexcept
	{
		DWORD processId{ 0ul };
		DAL_GetProcessId32(processName, &processId);
		return processId;
	}
}