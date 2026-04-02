#include "ProcessController.h"
#undef NTSTATUS // Undefine the WindowsProvider32.h definition
#include <ntdef.h> // NTSTATUS codes

#ifndef MAX_MODULES
#define MAX_MODULES 128ul
#endif // !MAX_MODULES

#ifndef MAX_THREADS
#define MAX_THREADS 128ul
#endif // !MAX_THREADS

#ifndef MAX_HANDLES
#define MAX_HANDLES 128ul
#endif // !MAX_HANDLES

namespace Muninn::Controller
{
	ProcessController::ProcessController(
		const DWORD processId,
		const ACCESS_MASK accessMask)
	{
		if (!SetProcessId(processId))
			return;

		if (!SetProcessHandle(accessMask))
			return;
	}

	bool ProcessController::SetProcessId(
		const DWORD processId) noexcept
	{
		if (!DAL_IsValidProcessId(processId))
			return false;
		m_process.processEntry.processId = processId;
	}

	bool ProcessController::SetProcessHandle(
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

	// All to be reviewed, should make use of getters n setters
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

		DWORD copiedLength{0ul};
		m_process.processEntry.userFullProcessImageName.resize(MAX_PATH);

		status = DAL_GetImageFileNameWin32Nt(
			m_processHandle,
			m_process.processEntry.userFullProcessImageName.data(),
			MAX_PATH,
			&copiedLength);
		if (!NT_SUCCESS(status))
			return false;

		m_process.processEntry.NativeImageFileName.resize(MAX_PATH);
		status = DAL_GetImageFileNameNt(
			m_processHandle,
			m_process.processEntry.NativeImageFileName.data(),
			MAX_PATH,
			&copiedLength);
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
			m_process.processEntry.architectureType = Muninn::Model::ArchitectureType::x86;
			break;
		case(IMAGE_FILE_MACHINE_AMD64):
			m_process.processEntry.architectureType = Muninn::Model::ArchitectureType::x64;
			break;
		default:
			m_process.processEntry.architectureType = Muninn::Model::ArchitectureType::Unknown;
			break;
		}

		return true;
	}

	const bool ProcessController::InitializeModuleList() noexcept
	{
		if (!DAL_IsValidProcessId(m_process.processEntry.processId))
			return false;
		if (!DAL_IsValidHandle(m_processHandle))
			return false;
		if (m_process.processEntry.pebBaseAddress == NULL)
			return false;

		PEB* pPeb{ reinterpret_cast<PEB*>(
				m_process.processEntry.pebBaseAddress) };

		std::vector<LDR_DATA_TABLE_ENTRY> moduleList{ MAX_MODULES };
		DWORD copiedLength{0ul};

		NTSTATUS status{ DAL_GetProcessModulesNt(
			m_processHandle,
			pPeb,
			moduleList.data(),
			MAX_MODULES,
			&copiedLength) };
		if (!NT_SUCCESS(status))
			return false;

		for (DWORD i{0ul}; i < copiedLength; ++i)
		{
			Model::ModuleModel moduleEntry{};

			/*
			status = DAL_GetRemoteUnicodeStringNt(
				m_processHandle,
				&moduleList[i].BaseDllName,
				moduleEntry.moduleName.data(),
				MAX_PATH,
				&copiedLength);
			if (!NT_SUCCESS(status))
				return false;

			status = DAL_GetRemoteUnicodeStringNt(
				m_processHandle,
				&moduleList[i].FullDllName,
				moduleEntry.modulePath.data(),
				static_cast<DWORD>(moduleEntry.modulePath.size()),
				&copiedLength);
			if (!NT_SUCCESS(status))
				return false;
				*/

			moduleEntry.moduleLoadAddress =
				reinterpret_cast<uintptr_t>(
					moduleList[i].DllBase);

			moduleEntry.moduleEntryPoint =
				reinterpret_cast<uintptr_t>(
					moduleList[i].EntryPoint);

			moduleEntry.moduleBaseAddress =
				reinterpret_cast<uintptr_t>(
					moduleList[i].DllBase);

			moduleEntry.parentDllBaseAddress =
				reinterpret_cast<uintptr_t>(
					moduleList[i].ParentDllBase);

			moduleEntry.kernelModuleFlags =
				moduleList[i].Flags;

			moduleEntry.moduleImageSize =
				moduleList[i].SizeOfImage;

			moduleEntry.processId =
				m_process.processEntry.processId;

			moduleEntry.tlsIndex =
				moduleList[i].TlsIndex;

			m_process.moduleList.push_back(moduleEntry);
		}
		return true;
	}

	const bool ProcessController::InitializeThreadList() noexcept
	{
		if (!DAL_IsValidProcessId(m_process.processEntry.processId))
			return false;
		if (!DAL_IsValidHandle(m_processHandle))
			return false;

		std::vector<SYSTEM_THREAD_INFORMATION> threadList{ MAX_THREADS };
		DWORD copiedLength{ 0ul };

		NTSTATUS status{ DAL_GetProcessThreadsNt(
			m_processHandle,
			m_process.processEntry.processId,
			threadList.data(),
			sizeof(MAX_THREADS),
			&copiedLength) };
		if (!NT_SUCCESS(status))
			return false;

		for (DWORD i{ 0ul }; i < copiedLength / sizeof(SYSTEM_THREAD_INFORMATION); ++i)
		{
			Model::ThreadModel threadEntry{};

			threadEntry.kernelThreadStartAddress =
				reinterpret_cast<uintptr_t>(
					threadList[i].StartAddress);

			threadEntry.threadId =
				static_cast<DWORD>(
				reinterpret_cast<uintptr_t>(
					threadList[i].ClientId.UniqueThread));

			threadEntry.threadOwnerProcessId =
				static_cast<DWORD>(
					reinterpret_cast<uintptr_t>(
						threadList[i].ClientId.UniqueProcess));

			threadEntry.nativeThreadBasePriority =
				threadList[i].BasePriority;

			m_process.threadList.push_back(threadEntry);
		}
		return true;
	}

	const bool ProcessController::InitializeHandleList() noexcept
	{
		if (!DAL_IsValidProcessId(m_process.processEntry.processId))
			return false;
		if (!DAL_IsValidHandle(m_processHandle))
			return false;

		std::vector<SYSTEM_HANDLE_TABLE_ENTRY_INFO> handleList{ MAX_HANDLES };
		DWORD copiedLength{ 0ul };
		NTSTATUS status{ DAL_GetProcessHandlesNt(
			m_processHandle,
			m_process.processEntry.processId,
			handleList.data(),
			MAX_HANDLES,
			&copiedLength) };
		if (!NT_SUCCESS(status))
			return false;

		for (DWORD i{ 0ul }; i < copiedLength / sizeof(SYSTEM_HANDLE_TABLE_ENTRY_INFO); ++i)
		{
			Model::HandleModel handleEntry{};
			handleEntry.objectName = std::wstring(
				reinterpret_cast<WCHAR*>(handleList[i].Object),
				MAX_PATH);

			handleEntry.handleValue =
				reinterpret_cast<HANDLE>(
					handleList[i].HandleValue);

			handleEntry.grantedAccess =
				handleList[i].GrantedAccess;

			handleEntry.userTargetProcessId =
				static_cast<DWORD>(
						handleList[i].UniqueProcessId);

			m_process.handleList.push_back(handleEntry);
		}
		return true;
	}

	const bool ProcessController::SimpleInjectDll(
		const WCHAR* dllPath, HMODULE& pModuleHandle) noexcept
	{
		if (!DAL_IsValidProcessId(m_process.processEntry.processId))
			return false;
		if (!DAL_IsValidHandle(m_processHandle))
			return false;

		NTSTATUS status{ DAL_SimpleDLLInjectW32(
			m_processHandle,
			dllPath,
			&pModuleHandle) };
			
		return NT_SUCCESS(status) == STATUS_SUCCESS ?
			true :
			false;
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

	const Muninn::Model::ProcessModel&
		ProcessController::GetProcessObject() const noexcept
	{
		return m_process;
	}

	const Muninn::Model::InjectorModel&
		ProcessController::GetInjectorObject() const noexcept
	{
		return m_injector;
	}

	const HANDLE& ProcessController::GetProcessHandle() const noexcept
	{
		return m_processHandle;
	}

	const DWORD ProcessController::GetProcessId() const noexcept
	{
		return m_process.processEntry.processId;
	}

	DWORD ProcessController::GetProcessId(const WCHAR* processName, bool& isRunning) noexcept
	{
		DWORD processId{ 0ul };
		BOOL isRunningBuffer{ FALSE };

		NTSTATUS status{ DAL_GetProcessId32(
			processName,
			&processId,
			&isRunningBuffer) };

		isRunning = 
			(isRunningBuffer == TRUE);

		if (!NT_SUCCESS(status))
			return 0ul;
		
		return processId;
	}
}