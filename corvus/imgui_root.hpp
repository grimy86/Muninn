#pragma once
#include <imgui.h>
#include <thread>
#include <atomic>

#include "imgui_processes.hpp"

namespace corvus::imgui
{
	// ------------------------------------------------------------
	// Routing
	// ------------------------------------------------------------
	enum class Route
	{
		None,
		Process_List,
		Analyze_Modules,
		Analyze_Threads,
		Analyze_Handles,
	};

	inline Route g_routedView{ Route::None };

	// ------------------------------------------------------------
	// Loading state
	// ------------------------------------------------------------
	inline std::atomic<bool> g_loading{ false };
	inline std::atomic<bool> g_processesLoaded{ false };
	inline float g_loadingAnim = 0.0f;

	// ------------------------------------------------------------
	// Helpers
	// ------------------------------------------------------------
	inline const char* RouteBreadcrumb(Route r)
	{
		switch (r)
		{
		case Route::Process_List:      return "Analyze > Process List";
		case Route::Analyze_Modules:   return "Analyze > Modules";
		case Route::Analyze_Threads:   return "Analyze > Threads";
		case Route::Analyze_Handles:   return "Analyze > Handles";
		default:                       return "Home";
		}
	}

	inline void DrawIndeterminateBar(float height = 3.0f)
	{
		const ImVec2 pos = ImGui::GetCursorScreenPos();
		const float width = ImGui::GetContentRegionAvail().x;

		// Reserve layout space
		ImGui::Dummy(ImVec2(width, height + ImGui::GetStyle().ItemSpacing.y));

		// Animation
		static float anim = 0.0f;
		anim += ImGui::GetIO().DeltaTime * 1.5f;
		float t = fmodf(anim, 1.0f);

		const float barWidth = width * 0.25f;
		const float x = pos.x + (width + barWidth) * t - barWidth;

		ImDrawList* draw = ImGui::GetWindowDrawList();

		// Background
		draw->AddRectFilled(
			pos,
			ImVec2(pos.x + width, pos.y + height),
			ImGui::GetColorU32(ImGuiCol_FrameBg),
			height * 0.5f
		);

		// Moving bar
		draw->AddRectFilled(
			ImVec2(x, pos.y),
			ImVec2(x + barWidth, pos.y + height),
			ImGui::GetColorU32(ImGuiCol_PlotHistogram),
			height * 0.5f
		);
	}

	// ------------------------------------------------------------
	// Navigation
	// ------------------------------------------------------------
	inline void Navigate(Route r)
	{
		g_routedView = r;

		if (r == Route::Process_List && !g_processesLoaded && !g_loading)
		{
			g_loading = true;

			std::thread([] {
				LoadProcessList();   // heavy work
				g_processesLoaded = true;
				g_loading = false;
				}).detach();
		}
	}

	// ------------------------------------------------------------
	// Root UI
	// ------------------------------------------------------------
	inline void root()
	{
		// ------------------ Style ---------------------------------
		ImGuiStyle& style = ImGui::GetStyle();
		style.FramePadding = ImVec2(8, 6);
		style.ItemSpacing = ImVec2(8, 6);
		style.IndentSpacing = 16.0f;
		style.ScrollbarSize = 14.0f;

		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.2f, 0.2f, 0.2f, 0.4f));

		// ------------------ Wait cursor ----------------------------
		if (g_loading)
			ImGui::SetMouseCursor(ImGuiMouseCursor_Wait);

		// ------------------ Root window ----------------------------
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(io.DisplaySize);

		ImGuiWindowFlags wFlags =
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoDecoration;

		ImGui::Begin("##root", nullptr, wFlags);

		// ============================================================
		// Left navigation
		// ============================================================
		ImGui::BeginChild("left", ImVec2(280, 0), true);

		ImGui::TextDisabled("ANALYZE");
		ImGui::Separator();

		if (ImGui::Selectable("Process List", g_routedView == Route::Process_List))
			Navigate(Route::Process_List);

		if (ImGui::Selectable("Modules", g_routedView == Route::Analyze_Modules))
			Navigate(Route::Analyze_Modules);

		if (ImGui::Selectable("Threads", g_routedView == Route::Analyze_Threads))
			Navigate(Route::Analyze_Threads);

		if (ImGui::Selectable("Handles", g_routedView == Route::Analyze_Handles))
			Navigate(Route::Analyze_Handles);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::TextDisabled("UTILITIES");

		ImGui::Selectable("Hook Creator");
		ImGui::Selectable("Shellcode Creator");
		ImGui::Selectable("Pointer Calculator");
		ImGui::Selectable("Address Converter");

		ImGui::EndChild();

		// ============================================================
		// Right canvas
		// ============================================================
		ImGui::SameLine();
		ImGui::BeginChild("canvas", ImVec2(0, 0), false);

		// Header
		ImGui::TextDisabled(RouteBreadcrumb(g_routedView));
		ImGui::Separator();

		// Content
		if (g_loading)
		{
			ImGui::Spacing();
			DrawIndeterminateBar(5.0f);
		}
		else
		{
			switch (g_routedView)
			{
			case Route::Process_List:
				DrawProcessList();
				break;

			case Route::Analyze_Modules:
				DrawModulesView();
				break;

			case Route::Analyze_Threads:
				DrawThreadsView();
				break;

			case Route::Analyze_Handles:
				DrawHandlesView();
				break;

			default:
				ImGui::TextDisabled("Select a module from the left");
				break;
			}
		}

		ImGui::EndChild();
		ImGui::End();

		// ------------------ Style cleanup ---------------------------
		ImGui::PopStyleColor(2);
	}
}
