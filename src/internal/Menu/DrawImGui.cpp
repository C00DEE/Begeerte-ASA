#define NOMINMAX  
#if defined(__cpp_char8_t)
#define U8(str) reinterpret_cast<const char*>(u8##str)
#else
#define U8(str) u8##str
#endif

#include "../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../../external/SDK/SDK_Headers.hpp"
// #include "../ESP/ESP.h"
#include "../Config/Configs.h"
// #include "../ESP/DrawESP.h"
#include "ConfigImGui.h"
#include "DrawImGui.h"
#include "Aimbot_Menu.h"
#include "Visuals_Menu.h"
#include "EntityList_Menu.h"
#include "StructureList_Menu.h"
#include "ItemList_Menu.h"
#include "Misc_Menu.h"
#include "Language_Menu.h"
#include "Configs_Menu.h"
#include "Lua_Menu.h"
#include "../Log/LogManager.h"
#include "../Language/LanguageManager.h"
#include "../Config/ConfigManager.h"
#include "../Lua/LuaManager.h"
#include "../Util/Util.h"

#include <cstdio>
#include <string>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <map>

namespace g_DrawImGui {
	float g_MenuAlpha = 0.0f;

	void MyImGuiDraw(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		if (!style_initialized) { SetupCustomImGuiStyle(); style_initialized = true; }

		ImU32 current_color_u32 = ImGui::ColorConvertFloat4ToU32(ImVec4(g_Config::MenuColor[0], g_Config::MenuColor[1], g_Config::MenuColor[2], g_Config::MenuColor[3]));
		static ImU32 last_menu_color = 0;

		if (current_color_u32 != last_menu_color) {
			last_menu_color = current_color_u32;

			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4 accent = ThemeColors::GetAccent();

			// 更新全局样式表中的依赖颜色
			style.Colors[ImGuiCol_CheckMark] = accent;
			style.Colors[ImGuiCol_SliderGrab] = accent;
			style.Colors[ImGuiCol_SliderGrabActive] = accent;

			style.Colors[ImGuiCol_Header] = ImVec4(accent.x, accent.y, accent.z, 0.08f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(accent.x, accent.y, accent.z, 0.14f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(accent.x, accent.y, accent.z, 0.22f);

			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(accent.x, accent.y, accent.z, 0.35f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(accent.x, accent.y, accent.z, 0.55f);

			style.Colors[ImGuiCol_TabHovered] = ImVec4(accent.x, accent.y, accent.z, 0.08f);
			style.Colors[ImGuiCol_TabActive] = ImVec4(accent.x, accent.y, accent.z, 0.15f);

			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(accent.x, accent.y, accent.z, 0.35f);
			style.Colors[ImGuiCol_DragDropTarget] = ImVec4(accent.x, accent.y, accent.z, 0.55f);
			style.Colors[ImGuiCol_NavHighlight] = ImVec4(accent.x, accent.y, accent.z, 0.85f);
		}

		const float FADE_SPEED = 5.0f;
		static bool g_PrevMenuState = false;
		bool isNowOpen = g_MDX12::g_MenuState::g_isOpen;

		if (isNowOpen && !g_PrevMenuState) {
			LanguageManager::Get().RefreshFileList();
			LuaManager::Get().RefreshFileList();
			ConfigManager::Get().RefreshFileList();
			LuaManager::Get().Lua_OnMenuOpen();
		}
		g_PrevMenuState = isNowOpen;

		ImGuiIO& io = ImGui::GetIO();
		ImVec2 screenSize = io.DisplaySize;
		float deltaTime = io.DeltaTime;

		if (g_MDX12::g_MenuState::g_isOpen) {
			g_MenuAlpha += deltaTime * FADE_SPEED;
			if (g_MenuAlpha > 1.0f) g_MenuAlpha = 1.0f;
		}
		else {
			g_MenuAlpha -= deltaTime * FADE_SPEED;
			if (g_MenuAlpha < 0.0f) g_MenuAlpha = 0.0f;
		}

		if (g_MenuAlpha > 0.001f) {
			LuaManager::Get().Lua_OnPaintMenu(g_MenuAlpha);

			const float menu_size_w = 1020.f;
			const float menu_size_h = 720.f;

			ImGui::GetStyle().WindowMinSize = ImVec2(menu_size_w, menu_size_h);
			// 处理重置逻辑
			if (g_Config::bMenuNeedReset) {
				ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.382, io.DisplaySize.y * 0.414), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize(ImVec2(menu_size_w, menu_size_h), ImGuiCond_Always);
				g_Config::bMenuNeedReset = false;
			}
			else {
				// 正常状态：限制最大尺寸不超过屏幕
				ImGui::SetNextWindowSizeConstraints(ImVec2(menu_size_w, menu_size_h), screenSize);

				// 如果你希望第一次打开是 menu_size_w x menu_size_h，可以用 FirstUseEver
				// 但不要放在 if 块外面，否则会干扰上面的 Always 逻辑
				ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.382, io.DisplaySize.y * 0.414), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize(ImVec2(menu_size_w, menu_size_h), ImGuiCond_FirstUseEver);
			}

			// ImGui::SetNextWindowSize(ImVec2(menu_size_w, menu_size_h), ImGuiCond_FirstUseEver);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, g_MenuAlpha);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(22.0f, 22.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 18.0f);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

			ImVec4 accentColor = ThemeColors::GetAccent();

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(7.0f * g_Util::inv255, 8.0f * g_Util::inv255, 10.0f * g_Util::inv255, 0.97f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.08f));

			ImGuiWindowFlags wFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
			io.ConfigWindowsResizeFromEdges = false;
			if (g_Config::bMenuLockResize) {
				wFlags |= ImGuiWindowFlags_NoResize; // 如果勾选，禁止缩放
			}

			if (ImGui::Begin("Begeerte", nullptr, wFlags)) {

				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				ImVec2 title_pos = ImGui::GetWindowPos();
				float title_width = ImGui::GetWindowWidth();
				float time = ImGui::GetTime();

				// 获取最新的 Accent 颜色用于绘制装饰线条
				ImU32 colAccentU32 = ImGui::GetColorU32(accentColor);
				ImU32 colAccentTransparentU32 = ImGui::GetColorU32(ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.0f));

				draw_list->AddRectFilledMultiColor(
					ImVec2(title_pos.x, title_pos.y),
					ImVec2(title_pos.x + title_width, title_pos.y + 2.0f),
					colAccentTransparentU32,
					colAccentU32,
					colAccentU32,
					colAccentTransparentU32
				);

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(22.0f, 11.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 0.0f));
				ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(1.0f, 1.0f, 1.0f, 0.03f));
				ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.1f));
				ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.18f));
				ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ImVec4(1.0f, 1.0f, 1.0f, 0.03f));
				ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(1.0f, 1.0f, 1.0f, 0.05f));

				if (ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_None)) {

					Aimbot_Menu();
					Visuals_Menu();
					EntityList_Menu();
					StructureList_Menu();
					ItemList_Menu();
					Misc_Menu();
					Language_Menu();
					Configs_Menu();
					Lua_Menu();

					ImGui::EndTabBar();

				}

				ImGui::PopStyleColor(5);
				ImGui::PopStyleVar(2);
			}

			ImGui::End();
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar(4);
		}

		// g_Aimbot::Tick();
		// g_DrawESP::DrawESP();
		g_LogManager::Run();
		LuaManager::Get().Lua_OnPaint();
	}
}