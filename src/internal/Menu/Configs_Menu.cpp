#pragma once
#define NOMINMAX
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../Config/Configs.h"
#include "../Config/ConfigManager.h"
#include "ConfigImGui.h"
#include "Configs_Menu.h"
#include "../Util/Util.h"
#include "../../external/SDK/SDK_Headers.hpp"
#include "../Language/LanguageManager.h"

namespace g_DrawImGui {
    void Configs_Menu() {
		const char* tabLabel = LanguageManager::Configs_Menu::TabLabel;
		const char* sectionTitle = LanguageManager::Configs_Menu::SectionTitle;
		const char* btnRefresh = LanguageManager::Configs_Menu::RefreshFileList;
		const char* btnOpenDir = LanguageManager::Configs_Menu::OpenDirectory;
		const char* inputHint = LanguageManager::Configs_Menu::InputHint;
		const char* btnCreate = LanguageManager::Configs_Menu::Create;
		const char* noConfigs = LanguageManager::Configs_Menu::NoConfigs;
		const char* btnLoad = LanguageManager::Configs_Menu::LoadConfig;
		const char* btnSave = LanguageManager::Configs_Menu::SaveConfig;

		if (ImGui::BeginTabItem(tabLabel)) {
			auto& mgr = ConfigManager::Get();
			auto& configs = mgr.GetConfigs();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(14.0f, 14.0f));
			BeginTabRegion("ConfigManagerRegion");

			ImGui::TextColored(ThemeColors::GetAccent(), sectionTitle);
			DrawAnimatedSeparator();

			if (DrawCustomButton(btnRefresh)) {
				mgr.RefreshFileList();
			}
			ImGui::SameLine();
			if (DrawCustomButton(btnOpenDir)) {
				std::string path = mgr.GetConfigDir();
				ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
			}

			ImGui::SameLine(0.0f, 20.0f);

			static char configNameBuf[65] = { 0 };
			ImGui::SetNextItemWidth(150.0f);
			ImGui::InputTextWithHint("##ConfigName", inputHint, configNameBuf, sizeof(configNameBuf));
			ImGui::SameLine();
			if (DrawCustomButton(btnCreate)) {
				std::string configName(configNameBuf);
				if (configName.empty()) {

				}
				else if (!ConfigManager::IsValidConfigName(configName)) {

				}
				else {
					if (mgr.CreateConfig(configName)) {
						configNameBuf[0] = '\0';
					}
					else {

					}
				}
			}

			ImGui::Spacing();

			static int selectedConfigIdx = -1;
	        if (configs.empty()) {
				ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - 10);
				ImGui::TextDisabled(noConfigs);
			}
			else {
				for (int i = 0; i < (int)configs.size(); i++) {
					auto& config = configs[i];
					ImGui::PushID(i);

					bool isSelected = (selectedConfigIdx == i);
					if (CustomSelectable(config.name.c_str(), isSelected, 8.0f) /*ImGui::Selectable(config.name.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns)*/) {
						selectedConfigIdx = i;
					}

					ImGui::PopID();
				}
			}

			ImGui::Spacing();

			ImGui::BeginDisabled(selectedConfigIdx < 0 || selectedConfigIdx >= (int)configs.size());

			if (DrawCustomButton(btnLoad, ImVec2(120, 0))) {
				if (selectedConfigIdx >= 0 && selectedConfigIdx < (int)configs.size()) {
					auto& selectedConfig = configs[selectedConfigIdx];
					if (mgr.LoadConfig(selectedConfig.name)) {

					}
					else {

					}
				}
			}

			ImGui::SameLine();

			if (DrawCustomButton(btnSave, ImVec2(120, 0))) {
				if (selectedConfigIdx >= 0 && selectedConfigIdx < (int)configs.size()) {
					auto& selectedConfig = configs[selectedConfigIdx];
					if (mgr.SaveConfig(selectedConfig.name)) {

					}
					else {

					}
				}
			}

			ImGui::EndDisabled();

			DrawAnimatedSeparator();
			EndTabRegion();
			ImGui::PopStyleVar();
			ImGui::EndTabItem();
		}
    }
}