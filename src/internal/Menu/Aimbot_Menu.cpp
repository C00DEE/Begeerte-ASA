#pragma once
#define NOMINMAX  
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/hooks.h"
#include "../Config/Configs.h"
#include "ConfigImGui.h"
#include "Aimbot_Menu.h"
#include "../Util/Util.h"
#include "../../external/SDK/SDK_Headers.hpp"
#include "../Language/LanguageManager.h"

namespace g_DrawImGui {
    void Aimbot_Menu() {
        const char* tabLabel = LanguageManager::Aimbot_Menu::TabLabel;
        const char* secTitle = LanguageManager::Aimbot_Menu::SectionTitle;
        const char* aimEnabled = LanguageManager::Aimbot_Menu::AimbotEnabled;
        const char* aimFOV = LanguageManager::Aimbot_Menu::AimbotFOV;
        const char* aimSmooth = LanguageManager::Aimbot_Menu::AimbotSmooth; 
        const char* aimRCX = LanguageManager::Aimbot_Menu::AimbotRCX;
        const char* aimRCY = LanguageManager::Aimbot_Menu::AimbotRCY;
        const char* trigEnabled = LanguageManager::Aimbot_Menu::TriggerbotEnabled;
        const char* trigHitChance = LanguageManager::Aimbot_Menu::TriggerHitChance;

        if (ImGui::BeginTabItem(tabLabel)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(14.0f, 14.0f));
            BeginTabRegion("AimBotRegion");

            ImGui::TextColored(ThemeColors::GetAccent(), secTitle);
            DrawAnimatedSeparator();

            ImGui::BeginDisabled(!g_Hook::UWorldTickOK);
            {
                DrawCustomCheckbox(aimEnabled, &g_Config::bAimbotEnabled);
                DrawCustomSliderFloat(aimFOV, &g_Config::AimbotFOV, 0.1f, 180.0f, "%.1f", 0.1f, U8("бу"));
                DrawCustomSliderFloat(aimSmooth, &g_Config::AimbotSmooth, 0.1f, 100.0f, "%.1f", 0.1f, "%");
                DrawCustomSliderFloat(aimRCX, &g_Config::AimbotRCX, 0.0f, 100.0f, "%.1f", 0.1f, "%");
                DrawCustomSliderFloat(aimRCY, &g_Config::AimbotRCY, 0.0f, 100.0f, "%.1f", 0.1f, "%");

                DrawCustomCheckbox(trigEnabled, &g_Config::bTriggerbotEnabled);
                DrawCustomSliderFloat(trigHitChance, &g_Config::TriggerHitChance, 0.0f, 100.0f, "%.0f", 1.0f, "%");
            }
            ImGui::EndDisabled();

            DrawAnimatedSeparator();

            EndTabRegion();
            ImGui::PopStyleVar();
            ImGui::EndTabItem();
        }
    }
}