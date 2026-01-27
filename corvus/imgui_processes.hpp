#pragma once
#include <imgui.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <atomic>

#include "process.hpp"
#include "converter.hpp"

namespace corvus::imgui {

	// ============================================================
	// External UI state
	// ============================================================
	extern std::atomic<bool> g_loading;
	extern bool g_hasLoadedProcesses;
	extern bool g_useNtProcessList;

	// ============================================================
	// Shared state
	// ============================================================
	inline std::vector<corvus::process::WindowsProcessWin32> g_ProcessCache;
	inline std::vector<corvus::process::WindowsProcessNt>    g_ProcessCacheNt;

	inline std::shared_ptr<corvus::process::WindowsProcessWin32> g_SelectedProcess;
	inline std::shared_ptr<corvus::process::WindowsProcessNt>    g_SelectedProcessNt;

	// ============================================================
	// Loader API
	// ============================================================
	inline void LoadProcessList()
	{
		g_ProcessCache = corvus::process::WindowsProcessWin32::GetProcessListW32();
		g_ProcessCacheNt = corvus::process::WindowsProcessNt::GetProcessListNt();
		g_SelectedProcess.reset();
		g_SelectedProcessNt.reset();
	}

	// ============================================================
	// Helpers
	// ============================================================
	inline void DrawSectionHeader(const char* label)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::TextDisabled(label);
	}

	// ============================================================
	// Win32 Row
	// ============================================================
	inline void DrawProcessRowWin32(corvus::process::WindowsProcessWin32& p)
	{
		ImGui::TableNextRow();

		const bool selected =
			g_SelectedProcess &&
			g_SelectedProcess->GetProcessId() == p.GetProcessId();

		ImGui::TableSetColumnIndex(0);
		if (ImGui::Selectable(std::to_string(p.GetProcessId()).c_str(),
			selected, ImGuiSelectableFlags_SpanAllColumns))
		{
			g_SelectedProcess = std::make_shared<corvus::process::WindowsProcessWin32>(p);
			g_SelectedProcessNt.reset();
		}

		ImGui::TableSetColumnIndex(1);
		ImGui::TextUnformatted(
			corvus::converter::WStringToString(p.GetName()).c_str());

		ImGui::TableSetColumnIndex(2);
		ImGui::TextUnformatted(
			corvus::converter::ArchitectureToString(p.GetArchitecture()));
	}

	// ============================================================
	// NT Row
	// ============================================================
	inline void DrawProcessRowNt(corvus::process::WindowsProcessNt& p)
	{
		ImGui::TableNextRow();

		const bool selected =
			g_SelectedProcessNt &&
			g_SelectedProcessNt->GetProcessId() == p.GetProcessId();

		ImGui::TableSetColumnIndex(0);
		if (ImGui::Selectable(std::to_string(p.GetProcessId()).c_str(),
			selected, ImGuiSelectableFlags_SpanAllColumns))
		{
			g_SelectedProcessNt = std::make_shared<corvus::process::WindowsProcessNt>(p);
			g_SelectedProcess.reset();
		}

		ImGui::TableSetColumnIndex(1);
		ImGui::TextUnformatted(
			corvus::converter::WStringToString(p.GetName()).c_str());

		ImGui::TableSetColumnIndex(2);
		ImGui::TextUnformatted(
			corvus::converter::WStringToString(p.GetImageFilePath()).c_str());

		ImGui::TableSetColumnIndex(3);
		ImGui::Text("0x%p", (void*)p.GetModuleBaseAddress());

		ImGui::TableSetColumnIndex(4);
		ImGui::Text("0x%p", (void*)p.GetPEBAddress());

		ImGui::TableSetColumnIndex(5);
		ImGui::Text("%ld", p.GetBasePriority());

		ImGui::TableSetColumnIndex(6);
		ImGui::Text("%d", (int)p.GetArchitecture());

		ImGui::TableSetColumnIndex(7);
		ImGui::TextUnformatted(p.IsWow64() ? "Yes" : "No");

		ImGui::TableSetColumnIndex(8);
		ImGui::TextUnformatted(p.IsProtectedProcess() ? "Yes" : "No");

		ImGui::TableSetColumnIndex(9);
		ImGui::TextUnformatted(p.IsBackgroundProcess() ? "Yes" : "No");

		ImGui::TableSetColumnIndex(10);
		ImGui::TextUnformatted(p.IsSecureProcess() ? "Yes" : "No");

		ImGui::TableSetColumnIndex(11);
		ImGui::TextUnformatted(p.IsSubsystemProcess() ? "Yes" : "No");
	}

	// ============================================================
	// Win32 Process List
	// ============================================================
	inline void DrawProcessList()
	{
		if (ImGui::Button("Refresh") && !g_loading)
			g_hasLoadedProcesses = false;

		ImGui::Separator();

		if (g_ProcessCache.empty()) {
			ImGui::TextDisabled("No process data available");
			return;
		}

		std::vector<std::reference_wrapper<corvus::process::WindowsProcessWin32>> fg, bg;
		for (auto& p : g_ProcessCache)
			(p.HasVisibleWindow() ? fg : bg).emplace_back(p);

		if (ImGui::BeginTable("process_table_w32", 3,
			ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
			ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 100.f);
			ImGui::TableSetupColumn("Process Name");
			ImGui::TableSetupColumn("CPU ISA", ImGuiTableColumnFlags_WidthFixed, 90.f);
			ImGui::TableHeadersRow();

			if (!fg.empty()) {
				DrawSectionHeader("Windowed Processes");
				for (auto& p : fg) DrawProcessRowWin32(p.get());
			}
			if (!bg.empty()) {
				DrawSectionHeader("Background Processes");
				for (auto& p : bg) DrawProcessRowWin32(p.get());
			}

			ImGui::EndTable();
		}
	}

	// ============================================================
	// NT Process List
	// ============================================================
	inline void DrawProcessListNt()
	{
		if (ImGui::Button("Refresh") && !g_loading)
			g_hasLoadedProcesses = false;

		ImGui::Separator();

		if (g_ProcessCacheNt.empty()) {
			ImGui::TextDisabled("No NT process data available");
			return;
		}

		if (ImGui::BeginTable("process_table_nt", 12,
			ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
			ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 100.f);
			ImGui::TableSetupColumn("Image Name");
			ImGui::TableSetupColumn("Image Path");
			ImGui::TableSetupColumn("Module Base");
			ImGui::TableSetupColumn("PEB");
			ImGui::TableSetupColumn("Base Pri");
			ImGui::TableSetupColumn("Arch");
			ImGui::TableSetupColumn("Wow64");
			ImGui::TableSetupColumn("Protected");
			ImGui::TableSetupColumn("Background");
			ImGui::TableSetupColumn("Secure");
			ImGui::TableSetupColumn("Subsystem");
			ImGui::TableHeadersRow();

			for (auto& p : g_ProcessCacheNt)
				DrawProcessRowNt(p);

			ImGui::EndTable();
		}
	}

	// ============================================================
	// Modules View (stub, but valid)
	// ============================================================
	inline void DrawModulesView()
	{
		if (!g_SelectedProcess && !g_SelectedProcessNt) {
			ImGui::TextDisabled("No process selected");
			return;
		}

		ImGui::TextDisabled("Modules view not implemented yet");
	}

	// ============================================================
	// Threads View
	// ============================================================
	inline void DrawThreadsView()
	{
		if (!g_SelectedProcess) {
			ImGui::TextDisabled("No Win32 process selected");
			return;
		}

		const auto& threads = g_SelectedProcess->GetThreads();
		if (threads.empty()) {
			ImGui::TextDisabled("No thread data available");
			return;
		}

		if (ImGui::BeginTable("threads_table", 6,
			ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
			ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("TID");
			ImGui::TableSetupColumn("Owner PID");
			ImGui::TableSetupColumn("Base Pri");
			ImGui::TableSetupColumn("Delta Pri");
			ImGui::TableSetupColumn("State");
			ImGui::TableSetupColumn("Wait");
			ImGui::TableHeadersRow();

			for (const auto& t : threads) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("%lu", t.threadId);
				ImGui::TableSetColumnIndex(1); ImGui::Text("%lu", t.ownerProcessId);
				ImGui::TableSetColumnIndex(2); ImGui::Text("%ld", t.basePriority);
				ImGui::TableSetColumnIndex(3); ImGui::Text("%ld", t.deltaPriority);
				ImGui::TableSetColumnIndex(4); ImGui::Text("%d", (int)t.threadState);
				ImGui::TableSetColumnIndex(5); ImGui::Text("%d", (int)t.waitReason);
			}

			ImGui::EndTable();
		}
	}

	// ============================================================
	// Handles View
	// ============================================================
	inline void DrawHandlesView()
	{
		if (!g_SelectedProcess) {
			ImGui::TextDisabled("No Win32 process selected");
			return;
		}

		const auto& handles = g_SelectedProcess->GetHandles();
		if (handles.empty()) {
			ImGui::TextDisabled("No handle data available");
			return;
		}

		if (ImGui::BeginTable("handles_table", 8,
			ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
			ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Handle");
			ImGui::TableSetupColumn("Type");
			ImGui::TableSetupColumn("Type Name");
			ImGui::TableSetupColumn("Object");
			ImGui::TableSetupColumn("Access");
			ImGui::TableSetupColumn("Flags");
			ImGui::TableSetupColumn("Attrib");
			ImGui::TableSetupColumn("RefCnt");
			ImGui::TableHeadersRow();

			for (const auto& h : handles) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("0x%p", h.handle);
				ImGui::TableSetColumnIndex(1); ImGui::Text("%u", (uint32_t)h.objectType);
				ImGui::TableSetColumnIndex(2);
				ImGui::TextUnformatted(corvus::converter::WStringToString(h.typeName).c_str());
				ImGui::TableSetColumnIndex(3);
				ImGui::TextUnformatted(corvus::converter::WStringToString(h.objectName).c_str());
				ImGui::TableSetColumnIndex(4); ImGui::Text("0x%08lX", h.grantedAccess);
				ImGui::TableSetColumnIndex(5); ImGui::Text("0x%08lX", h.flags);
				ImGui::TableSetColumnIndex(6); ImGui::Text("0x%08lX", h.attributes);
				ImGui::TableSetColumnIndex(7); ImGui::Text("%lu", h.handleCount);
			}

			ImGui::EndTable();
		}
	}
}