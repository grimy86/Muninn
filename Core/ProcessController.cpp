#include "ProcessController.h"
#undef NTSTATUS // Undefine the WindowsProvider32.h definition
#include <ntdef.h> // NTSTATUS codes
#include <iostream>

namespace Muninn::Controller
{
	ProcessController::ProcessController(
		const DWORD processId,
		const ACCESS_MASK processAccessMask)
	{
		if (!InitializeHandle(processId, processAccessMask))
			return;
	}

	ProcessController::~ProcessController()
	{
		if (!DisposeHandle());
	}

	bool ProcessController::InitializeHandle(
		const DWORD processId,
		const ACCESS_MASK accessMask)
	{
		if (!DAL_IsValidProcessId(processId))
			return false;
		if (DAL_IsValidHandle(m_processHandle))
			return false;

		NTSTATUS status{ DAL_OpenProcessHandleNt(
			processId,
			accessMask,
			&m_processHandle) };

		if (!NT_SUCCESS(status))
			return false;

		if (!DAL_IsValidHandle(m_processHandle))
			return false;
		else return true;
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

	bool ProcessController::InitializeProcessEntry32(const DWORD processId)
	{
		if (!DAL_IsValidProcessId(processId))
			return false;
		if (!DAL_IsValidHandle(m_processHandle))
			return false;

		m_process32.processEntry = {};


	}

	const Muninn::Models::ProcessObject&
		ProcessController::GetProcessObject32() const noexcept
	{
		return m_process32;
	}

	const Muninn::Models::ProcessObject&
		ProcessController::GetProcessObjectNt() const noexcept
	{
		return m_processNt;
	}

	const HANDLE&
		ProcessController::GetProcessHandle() const noexcept
	{
		return m_processHandle;
	}
}