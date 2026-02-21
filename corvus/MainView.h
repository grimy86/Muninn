#pragma once
#include "GuiStructures.h"
#include "Backend32.h"
#include "MemoryService32.h"
#include "WindowsSystem.h"
#include <imgui.h>

namespace Corvus::Gui
{
	inline constexpr ImGuiWindowFlags g_wndFlags{
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoSavedSettings };

	inline constexpr ImGuiTreeNodeFlags g_tnFlags{
		ImGuiTreeNodeFlags_DefaultOpen |
		ImGuiTreeNodeFlags_Framed |
		ImGuiTreeNodeFlags_Leaf };

	inline constexpr ImGuiTableFlags g_tFlags{
			ImGuiTableFlags_RowBg |
			ImGuiTableFlags_Borders |
			ImGuiTableFlags_ScrollX |
			ImGuiTableFlags_NoHostExtendX |
			ImGuiTableFlags_SizingFixedFit };

	class MainView final
	{
	private:
		MainView() = delete;
		MainView(const MainView&) = delete;
		MainView& operator=(const MainView&) = delete;

	public:
		inline static Corvus::System::WindowsSystem& m_system{ Corvus::System::WindowsSystem::GetInstance() };
		inline static Corvus::Gui::View s_view{ Corvus::Gui::View::Process };
		inline static ImGuiViewport* s_viewport{};
		inline static bool s_isSeDebugEnabled{ false };
		inline static bool s_isNtChecked{ false };

		static void Init();
		static void DrawHeader();
		static void DrawNavBar();
		static void DrawContentView();
		static void DrawMainView();
	};
}