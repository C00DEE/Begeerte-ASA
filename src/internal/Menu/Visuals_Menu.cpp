#pragma once
#define NOMINMAX
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/hooks.h"
#include "../Config/Configs.h"
#include "ConfigImGui.h"
#include "Visuals_Menu.h"
#include "../Util/Util.h"
#include "../../external/SDK/SDK_Headers.hpp"
#include "../Language/LanguageManager.h"

namespace g_DrawImGui {
    void Visuals_Menu() {
        const char* tabVisuals = LanguageManager::Visuals_Menu::TabLabelVisuals;
        const char* tabTeam = LanguageManager::Visuals_Menu::TabLabelTeam;
        const char* secGlobal = LanguageManager::Visuals_Menu::SectionGlobal;
        const char* secExtra = LanguageManager::Visuals_Menu::SectionExtra;
        const char* secWorld = LanguageManager::Visuals_Menu::SectionWorld;
        const char* secTeam = LanguageManager::Visuals_Menu::SectionTeam;
        const char* secTeamExtra = LanguageManager::Visuals_Menu::SectionTeamExtra;

        if (ImGui::BeginTabItem(tabVisuals)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(14.0f, 14.0f));
            BeginTabRegion("VisualsRegion");

            ImGui::BeginDisabled(!g_Hook::PostRenderOK);
            {

                ImGui::TextColored(ThemeColors::GetAccent(), secGlobal);
                DrawAnimatedSeparator();
                DrawColorPickerRow(LanguageManager::Visuals_Menu::Box, &g_Config::bDrawBox, "BoxCol1", g_Config::BoxColor);
                DrawColorPickerRow(LanguageManager::Visuals_Menu::Name, &g_Config::bDrawName, "NameCol1", g_Config::NameColor);
                DrawCustomCheckbox(LanguageManager::Visuals_Menu::HealthBar, &g_Config::bDrawHealthBar);
                DrawColorPickerRow(LanguageManager::Visuals_Menu::Torpor, &g_Config::bDrawTorpor, "TorporCol", g_Config::TorporColor);
                DrawColorPickerRow(LanguageManager::Visuals_Menu::Ragdoll, &g_Config::bDrawRagdoll, "RagdollCol", g_Config::RagdollColor);
                DrawAnimatedSeparator();

                ImGui::TextColored(ThemeColors::GetAccent(), secExtra);
                DrawAnimatedSeparator();
                DrawColorPickerRow(LanguageManager::Visuals_Menu::Distance, &g_Config::bDrawDistance, "DistCol1", g_Config::DistanceColor);
                DrawColorPickerRow(LanguageManager::Visuals_Menu::ShowAimPoints, &g_Config::bDrawAimPoints, "AimPointsCol1", g_Config::AimPointsColor);

                // 2026/4/11 @zetsr
                // 应该是之前移除aimbot的时候被注释掉了，我们可能并不需要分别做骨骼和瞄准点的cb，使用复选框或者单选框应该更好，以后可以加个glow类型的，不过骨骼确实很好看
                // DrawColorPickerRow(U8("显示瞄准骨骼"), &g_Config::bDrawAimSkeleton, "AimSkeletonCol1", g_Config::AimSkeletonColor);
                DrawAnimatedSeparator();

                ImGui::TextColored(ThemeColors::GetAccent(), secWorld);
                DrawAnimatedSeparator();

                DrawColorPickerRow(LanguageManager::Visuals_Menu::DroppedItems, &g_Config::bDrawDroppedItems, "DroppedItemNameCol", g_Config::DroppedItemNameColor);
                if (g_Config::bDrawDroppedItems) {
                    // DrawCustomColorPicker("DroppedItemDistanceCol", g_Config::DroppedItemDistanceColor, U8("物品距离"));
                    DrawCustomColorPicker("DroppedItemPiledCol", g_Config::DroppedItemPiledColor, LanguageManager::Visuals_Menu::DroppedItemPiled);
                    DrawCustomColorPicker("DroppedItemCryopodCol", g_Config::DroppedItemCryopodColor, LanguageManager::Visuals_Menu::DroppedItemCryopod);
                    DrawCustomColorPicker("DroppedItemEggCol", g_Config::DroppedItemEggColor, LanguageManager::Visuals_Menu::DroppedItemEgg);
                    DrawCustomColorPicker("DroppedItemMeatCol", g_Config::DroppedItemMeatColor, LanguageManager::Visuals_Menu::DroppedItemMeat);
                    DrawCustomColorPicker("DroppedItemSpoiledMeatCol", g_Config::DroppedItemSpoiledMeatColor, LanguageManager::Visuals_Menu::DroppedItemSpoiledMeat);
                    DrawCustomColorPicker("DroppedItemWoodCol", g_Config::DroppedItemWoodColor, LanguageManager::Visuals_Menu::DroppedItemWood);
                    DrawCustomColorPicker("DroppedItemThatchCol", g_Config::DroppedItemThatchColor, LanguageManager::Visuals_Menu::DroppedItemThatch);
                    DrawCustomColorPicker("DroppedItemMetalCol", g_Config::DroppedItemMetalColor, LanguageManager::Visuals_Menu::DroppedItemMetal);
                    DrawCustomColorPicker("DroppedItemStoneCol", g_Config::DroppedItemStoneColor, LanguageManager::Visuals_Menu::DroppedItemStone);
                    DrawCustomColorPicker("DroppedItemCrystalCol", g_Config::DroppedItemCrystalColor, LanguageManager::Visuals_Menu::DroppedItemCrystal);
                    DrawCustomColorPicker("DroppedItemGemCol", g_Config::DroppedItemGemColor, LanguageManager::Visuals_Menu::DroppedItemGem);
                    DrawCustomColorPicker("DroppedItemPearlCol", g_Config::DroppedItemPearlColor, LanguageManager::Visuals_Menu::DroppedItemPearl);
                    DrawCustomColorPicker("DroppedItemHideCol", g_Config::DroppedItemHideColor, LanguageManager::Visuals_Menu::DroppedItemHide);
                    DrawCustomColorPicker("DroppedItemPeltCol", g_Config::DroppedItemPeltColor, LanguageManager::Visuals_Menu::DroppedItemPelt);
                    DrawCustomColorPicker("DroppedItemKeratinCol", g_Config::DroppedItemKeratinColor, LanguageManager::Visuals_Menu::DroppedItemKeratin);
                    DrawCustomColorPicker("DroppedItemChitinCol", g_Config::DroppedItemChitinColor, LanguageManager::Visuals_Menu::DroppedItemChitin);
                    DrawCustomColorPicker("DroppedItemCorruptedPolymerCol", g_Config::DroppedItemCorruptedPolymerColor, LanguageManager::Visuals_Menu::DroppedItemCorruptedPolymer);
                    DrawCustomColorPicker("DroppedItemPolymer_OrganicCol", g_Config::DroppedItemPolymer_OrganicColor, LanguageManager::Visuals_Menu::DroppedItemPolymer_Organic);
                    DrawCustomColorPicker("DroppedItemPolymerCol", g_Config::DroppedItemPolymerColor, LanguageManager::Visuals_Menu::DroppedItemPolymer);
                    DrawCustomSliderFloat(LanguageManager::Visuals_Menu::DroppedItemMaxDistance, &g_Config::DroppedItemMaxDistance, 1.0f, 500.0f, "%.0f", 1.0f, "m");
                }
                DrawAnimatedSeparator();

                // DrawColorPickerRow(U8("显示建筑"), &g_Config::bDrawStructures, "StructureNameCol", g_Config::StructureNameColor);
                DrawCustomCheckbox(LanguageManager::Visuals_Menu::ShowStructures, &g_Config::bDrawStructures);
                if (g_Config::bDrawStructures) {
                    // DrawCustomColorPicker("StructureOwnerCol", g_Config::StructureOwnerColor, U8("建筑所有者"));
                    // DrawCustomColorPicker("StructureDistanceCol", g_Config::StructureDistanceColor, U8("建筑距离"));
                    DrawCustomCheckbox(LanguageManager::Visuals_Menu::OnlyEnemyStructures, &g_Config::bOnlyDrawStructuresEnemy);
                    DrawCustomSliderFloat(LanguageManager::Visuals_Menu::StructureMaxDistance, &g_Config::StructureMaxDistance, 1.0f, 10000.0f, "%.0f", 1.0f, "m");
                }
                DrawAnimatedSeparator();

                DrawColorPickerRow(LanguageManager::Visuals_Menu::ShowWater, &g_Config::bDrawWater, "WaterNameCol", g_Config::WaterNameColor);
                if (g_Config::bDrawWater) {
                    // DrawCustomColorPicker("WaterDistanceCol", g_Config::WaterDistanceColor, U8("水源距离"));
                    DrawCustomSliderFloat(LanguageManager::Visuals_Menu::WaterMaxCount, &g_Config::WaterMaxCount, 1.0f, 10.0f, "%.0f", 1.0f, LanguageManager::Visuals_Menu::WaterCountUnit);
                }
                DrawAnimatedSeparator();

                /*
                DrawColorPickerRow(U8("显示视野外的威胁"), &g_Config::bEnableOOF, "OOFCol1", g_Config::OOFColor);
                if (g_Config::bEnableOOF) {
                    float avail = ImGui::GetContentRegionAvail().x;
                    float frame_h = ImGui::GetFrameHeight();
                    float pad_y = ImGui::GetStyle().FramePadding.y;
                    float btn_sz = frame_h - pad_y * 2.0f;
                    if (btn_sz <= 0.0f) btn_sz = frame_h * 0.8f;
                    float target = ImGui::GetCursorPosX() + avail - btn_sz;
                    ImGui::SetCursorPosX(target);
                    ImGui::Dummy(ImVec2(0, 6.0f));
                    DrawCustomSliderFloat(U8("箭头尺寸"), &g_Config::OOFSize, 5.0f, 30.0f, "%.1f", 1.0f);
                    DrawCustomSliderFloat(U8("屏幕半径"), &g_Config::OOFRadius, 0.5f, 1.00f, "%.2f", 0.01f);
                    DrawCustomSliderFloat(U8("呼吸速度"), &g_Config::OOFBreathSpeed, 0.1f, 5.0f, "%.1f", 0.1f);
                    DrawCustomSliderFloat(U8("最小透明度"), &g_Config::OOFMinAlpha, 0.1f, 0.9f, "%.2f", 0.01f);
                    DrawCustomSliderFloat(U8("最大透明度"), &g_Config::OOFMaxAlpha, 0.2f, 1.0f, "%.2f", 0.01f);
                }
                */

                EndTabRegion();
                ImGui::PopStyleVar();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(tabTeam)) {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(14.0f, 14.0f));
                BeginTabRegion("TeamRegion");

                ImGui::TextColored(ThemeColors::GetAccent(), secTeam);
                DrawAnimatedSeparator();
                DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamBox, &g_Config::bDrawBoxTeam, "BoxColTeam", g_Config::BoxColorTeam);
                DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamName, &g_Config::bDrawNameTeam, "NameColTeam", g_Config::NameColorTeam);
                DrawCustomCheckbox(LanguageManager::Visuals_Menu::TeamHealthBar, &g_Config::bDrawHealthBarTeam);
                DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamTorpor, &g_Config::bDrawTorporTeam, "TorporColTeam", g_Config::TorporColorTeam);
                DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamRagdoll, &g_Config::bDrawRagdollTeam, "RagdollColTeam", g_Config::RagdollColorTeam);
                DrawAnimatedSeparator();

                ImGui::TextColored(ThemeColors::GetAccent(), secTeamExtra);
                DrawAnimatedSeparator();
                DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamDistance, &g_Config::bDrawDistanceTeam, "DistColTeam", g_Config::DistanceColorTeam);
                DrawAnimatedSeparator();

                EndTabRegion();
                ImGui::PopStyleVar();
                ImGui::EndTabItem();
            }
            ImGui::EndDisabled();
        }
    }
}