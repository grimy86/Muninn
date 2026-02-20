#pragma once
#include "WindowsStructures.h"
#include <vector>
#include <memory>

#pragma region WindowsProcess
namespace Corvus::Backend
{
	class IProcessBackend;
}

namespace Corvus::Process
{
	class WindowsProcess final
	{
	private:
		DWORD m_processId{};
		HANDLE m_processHandle{};
		ProcessEntry m_processEntry{};
		BOOL m_processIdSet{ FALSE };
		BOOL m_processHandleSet{ FALSE };

		// converters
		static std::string ToString(const std::wstring& wstring);
		static std::string ToString(DWORD processId);
		static const char* ToString(ArchitectureType arch);
		static const char* ToString(PriorityClass priorityClass);
		static const char* DecodeAccessBits(DWORD access, const AccessBit* bits, size_t count);
		static const char* MapAccess(std::wstring type, DWORD access);
		static const char* MapAttributes(DWORD attribute);

	public:
		WindowsProcess() = default;
		~WindowsProcess();
		WindowsProcess(const WindowsProcess&) = delete;
		WindowsProcess& operator=(const WindowsProcess&) = delete;

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