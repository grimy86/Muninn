#include "ProcessObject.h"
#include "IWindowsBackend.h"
#include "MemoryService.h"
#include "MemoryServiceNt.h"
#include "ConverterService.h"
#include <stdexcept>

namespace Corvus::Object
{
	ProcessObject::~ProcessObject()
	{
		if (Corvus::Service::IsValidHandle(m_processHandle))
			Corvus::Service::CloseHandleNt(m_processHandle);
	};

	BOOL ProcessObject::Init(
		const DWORD processId,
		const ACCESS_MASK accessMask)
	{
		if (m_processIdSet || m_processHandleSet) return FALSE;

		if (Corvus::Service::IsValidProcessId(processId))
		{
			m_processEntry.processId = processId;
			m_processIdSet = TRUE;
			m_processHandle = Corvus::Service::OpenHandleNt(m_processEntry.processId, accessMask);
			m_processHandleSet = Corvus::Service::IsValidHandle(m_processHandle) ? TRUE : FALSE;
		}

		return m_processHandleSet;
	}

	const HANDLE ProcessObject::GetProcessHandle() const noexcept { return m_processHandle; }
	const ProcessEntry& ProcessObject::GetProcessEntry() const noexcept { return m_processEntry; }
	const DWORD ProcessObject::GetProcessId() const noexcept { return m_processEntry.processId; }
	const std::vector<ModuleEntry>& ProcessObject::GetModules() const noexcept { return m_processEntry.modules; }
	const std::vector<ThreadEntry>& ProcessObject::GetThreads() const noexcept { return m_processEntry.threads; }
	const std::vector<HandleEntry>& ProcessObject::GetHandles() const noexcept { return m_processEntry.handles; }

	const std::string& ProcessObject::GetProcessEntryNameA() const noexcept { return Corvus::Service::ToString(m_processEntry.processName); }
	const std::string& ProcessObject::GetProcessEntryImageFilePathA() const noexcept { return Corvus::Service::ToString(m_processEntry.imageFilePath); }
	const std::string& ProcessObject::GetProcessIdA() const noexcept { return Corvus::Service::ToString(m_processEntry.processId); }
	const char* ProcessObject::GetPriorityClassA() const noexcept { return Corvus::Service::ToString(m_processEntry.priorityClass); }
	const char* ProcessObject::GetArchitectureTypeA() const noexcept { return Corvus::Service::ToString(m_processEntry.architectureType); }
}