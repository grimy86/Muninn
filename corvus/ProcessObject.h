#pragma once
#include "ProcessStructures.h"
#include <vector>
#include <memory>

#pragma region ProcessObject
namespace Corvus::Data
{
	class IWindowsBackend;
}

namespace Corvus::Object
{
	class ProcessObject final
	{
	private:
		ProcessEntry m_processEntry{};
		HANDLE m_processHandle{};
		BOOL m_processIdSet{ FALSE };
		BOOL m_processHandleSet{ FALSE };

	public:
		ProcessObject() = default;
		~ProcessObject();
		ProcessObject(const ProcessObject&) = delete;
		ProcessObject& operator=(const ProcessObject&) = delete;

		BOOL Init(const DWORD processId, const ACCESS_MASK accessMask);

		const DWORD GetProcessId() const noexcept;
		const HANDLE GetProcessHandle() const noexcept;
		const ProcessEntry& GetProcessEntry() const noexcept;
		const std::vector<ModuleEntry>& GetModules() const noexcept;
		const std::vector<ThreadEntry>& GetThreads() const noexcept;
		const std::vector<HandleEntry>& GetHandles() const noexcept;

		// UTF-8 (narrow string) versions
		const std::string& GetProcessEntryNameA() const noexcept;
		const std::string& GetProcessEntryImageFilePathA() const noexcept;
		const std::string& GetProcessIdA() const noexcept;
		const char* GetPriorityClassA() const noexcept;
		const char* GetArchitectureTypeA() const noexcept;
	};
}
#pragma endregion