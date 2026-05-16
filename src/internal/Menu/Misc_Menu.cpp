#pragma once
#define NOMINMAX
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/hooks.h"
#include "../Config/Configs.h"
#include "ConfigImGui.h"
#include "Misc_Menu.h"
#include "../Util/Util.h"
#include "../Hack/Hack.h"
#include "../../external/SDK/SDK_Headers.hpp"
#include "../Language/LanguageManager.h"

namespace g_DrawImGui {
	void Misc_Menu() {
		const char* tabLabel = LanguageManager::Misc_Menu::TabLabel;
		const char* secMenu = LanguageManager::Misc_Menu::SectionMenu;
		const char* menuColor = LanguageManager::Misc_Menu::MenuColor;
		const char* lockLayout = LanguageManager::Misc_Menu::LockLayout;
		const char* resetLayout = LanguageManager::Misc_Menu::ResetLayout;
		const char* secMisc = LanguageManager::Misc_Menu::SectionMisc;
		const char* btnSuicide = LanguageManager::Misc_Menu::Suicide;
		const char* btnUnload = LanguageManager::Misc_Menu::Unload;
		const char* btnDump = LanguageManager::Misc_Menu::DumpServerInfo;
		const char* chkNotes = LanguageManager::Misc_Menu::UnlockExplorerNotes;
		const char* chkFeed = LanguageManager::Misc_Menu::AutoFeed;
		const char* chkFlyer = LanguageManager::Misc_Menu::SuperFlyer;
		const char* chkTurn = LanguageManager::Misc_Menu::ForceTurn;
		const char* chkLogDamage = LanguageManager::Misc_Menu::LogDamage;

		if (ImGui::BeginTabItem(tabLabel)) {
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(14.0f, 14.0f));
			BeginTabRegion("MiscRegion");

			ImGui::TextColored(ThemeColors::GetAccent(), secMenu);
			DrawAnimatedSeparator();

			DrawCustomColorPicker("MenuCol", g_Config::MenuColor, menuColor);

			DrawCustomCheckbox(lockLayout, &g_Config::bMenuLockResize);
			ImGui::SameLine();

			if (DrawCustomButton(resetLayout)) {
				g_Config::bMenuNeedReset = true;
			}
			ImGui::SameLine();

			if (DrawCustomButton(btnUnload)) {
				g_Hook::StopAllHooks();
			}
			DrawAnimatedSeparator();

			ImGui::TextColored(ThemeColors::GetAccent(), secMisc);
			DrawAnimatedSeparator();

			ImGui::BeginDisabled(!g_Hook::UWorldTickOK);
			{
				if (DrawCustomButton(btnSuicide)) {
					g_Config::bSuicide = true;
				}
			}
			ImGui::EndDisabled();

			ImGui::SameLine();
			if (DrawCustomButton(btnDump)) {
				g_Hack::DumpServerInfo();
			}

			DrawAnimatedSeparator();

			ImGui::BeginDisabled(!g_Hook::UWorldTickOK);
			{
				DrawCustomCheckbox(chkNotes, &g_Config::bUnlockExplorerNotes);
				DrawCustomCheckbox(chkFeed, &g_Config::bAutoFeed);
				DrawCustomCheckbox(chkFlyer, &g_Config::bSuperFlyer);
			}
			ImGui::EndDisabled();

			ImGui::BeginDisabled(!g_Hook::PhysicsRotationOK);
			{
				DrawCustomCheckbox(chkTurn, &g_Config::bForceTurn);
			}
			ImGui::EndDisabled();

			ImGui::BeginDisabled(!g_Hook::TakeDamageOK);
			{
				DrawColorPickerRow(chkLogDamage, &g_Config::bLogDamage, "LogDamageCol", g_Config::LogDamageColor);
			}
			ImGui::EndDisabled();

			DrawAnimatedSeparator();

			EndTabRegion();
			ImGui::PopStyleVar();
			ImGui::EndTabItem();
		}
	}
}