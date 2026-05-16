#pragma once
#define NOMINMAX
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../Language/LanguageManager.h"
#include "ConfigImGui.h"
#include "Language_Menu.h"
#include "../Util/Util.h"

namespace g_DrawImGui {
    void Language_Menu() {
        const char* tabLabel = LanguageManager::Language_Menu::TabLabel;
        const char* sectionTitle = LanguageManager::Language_Menu::SectionTitle;
        const char* btnRefresh = LanguageManager::Language_Menu::RefreshFileList;
        const char* btnOpenDir = LanguageManager::Language_Menu::OpenDirectory;
        const char* btnCreate = LanguageManager::Language_Menu::Create;
        const char* btnLoad = LanguageManager::Language_Menu::LoadLanguage;
        const char* inputHint = LanguageManager::Language_Menu::InputHint;
        const char* noLanguages = LanguageManager::Language_Menu::NoLanguages;

        if (ImGui::BeginTabItem(tabLabel)) {
            LanguageManager& mgr = LanguageManager::Get();
            std::vector<LanguageFile>& languages = mgr.GetLanguages();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(14.0f, 14.0f));
            BeginTabRegion("LanguageManagerRegion");

            ImGui::TextColored(ThemeColors::GetAccent(), sectionTitle);
            DrawAnimatedSeparator();

            if (DrawCustomButton(btnRefresh)) {
                mgr.RefreshFileList();
            }
            ImGui::SameLine();
            if (DrawCustomButton(btnOpenDir)) {
                std::string path = mgr.GetLanguageDir();
                ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
            }

            ImGui::SameLine(0.0f, 20.0f);

            static char langNameBuf[65] = { 0 };
            ImGui::SetNextItemWidth(150.0f);
            ImGui::InputTextWithHint("##LangName", inputHint, langNameBuf, sizeof(langNameBuf));
            ImGui::SameLine();
            if (DrawCustomButton(btnCreate)) {
                std::string langName(langNameBuf);
                if (!langName.empty()) {
                    if (mgr.CreateLanguage(langName)) {
                        langNameBuf[0] = '\0';
                    }
                }
            }

            ImGui::Spacing();

            static int selectedLangIdx = -1;
            if (languages.empty()) {
                ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - 10);
                ImGui::TextDisabled(noLanguages);
            }
            else {
                bool hasWorkshop = false;
                for (int i = 0; i < (int)languages.size(); i++) {
                    if (languages[i].isWorkshop) { hasWorkshop = true; break; }
                }

                for (int i = 0; i < (int)languages.size(); i++) {
                    LanguageFile& lf = languages[i];
                    ImGui::PushID(i);
                    bool isSelected = (selectedLangIdx == i);
                    std::string displayName = lf.isWorkshop ? ("work_shop/" + lf.name) : lf.name;
                    if (CustomSelectable(displayName.c_str(), isSelected, 8.0f))
                        selectedLangIdx = i;
                    ImGui::PopID();
                }
            }

            ImGui::Spacing();

            ImGui::BeginDisabled(selectedLangIdx < 0 || selectedLangIdx >= (int)languages.size());
            if (DrawCustomButton(btnLoad, ImVec2(120, 0))) {
                if (selectedLangIdx >= 0 && selectedLangIdx < (int)languages.size()) {
                    LanguageFile& selected = languages[selectedLangIdx];
                    mgr.LoadLanguage(selected.name);
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