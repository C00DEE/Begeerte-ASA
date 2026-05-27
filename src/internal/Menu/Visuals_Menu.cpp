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
    static int ESP_Type_idx = 0;

    std::vector<const char*> ESP_Types = {
        LanguageManager::Visuals_Menu::TYPEPlayer,
        LanguageManager::Visuals_Menu::TYPEDino,
        LanguageManager::Visuals_Menu::TYPEStructures,
        LanguageManager::Visuals_Menu::TYPEDroppedItems,
        LanguageManager::Visuals_Menu::TYPEWorld
    };

    std::vector<const char*> ESP_Scale = {
        "50%",
        "75%",
        "100%",
        "125%",
        "150%"
    };

    std::vector<CustomComboItem> dropped_items_combo = {
        { LanguageManager::Visuals_Menu::DroppedItemPiled,            false, g_Config::DroppedItemPiledColor,            &g_Config::bDroppedItemPiled },
        { LanguageManager::Visuals_Menu::DroppedItemCryopod,          false, g_Config::DroppedItemCryopodColor,          &g_Config::bDroppedItemCryopod },
        { LanguageManager::Visuals_Menu::DroppedItemEgg,              false, g_Config::DroppedItemEggColor,              &g_Config::bDroppedItemEgg },
        { LanguageManager::Visuals_Menu::DroppedItemMeat,             false, g_Config::DroppedItemMeatColor,             &g_Config::bDroppedItemMeat },
        { LanguageManager::Visuals_Menu::DroppedItemSpoiledMeat,      false, g_Config::DroppedItemSpoiledMeatColor,      &g_Config::bDroppedItemSpoiledMeat },
        { LanguageManager::Visuals_Menu::DroppedItemWood,             false, g_Config::DroppedItemWoodColor,             &g_Config::bDroppedItemWood },
        { LanguageManager::Visuals_Menu::DroppedItemThatch,           false, g_Config::DroppedItemThatchColor,           &g_Config::bDroppedItemThatch },
        { LanguageManager::Visuals_Menu::DroppedItemMetal,            false, g_Config::DroppedItemMetalColor,            &g_Config::bDroppedItemMetal },
        { LanguageManager::Visuals_Menu::DroppedItemStone,            false, g_Config::DroppedItemStoneColor,            &g_Config::bDroppedItemStone },
        { LanguageManager::Visuals_Menu::DroppedItemCrystal,          false, g_Config::DroppedItemCrystalColor,          &g_Config::bDroppedItemCrystal },
        { LanguageManager::Visuals_Menu::DroppedItemGem,              false, g_Config::DroppedItemGemColor,              &g_Config::bDroppedItemGem },
        { LanguageManager::Visuals_Menu::DroppedItemPearl,            false, g_Config::DroppedItemPearlColor,            &g_Config::bDroppedItemPearl },
        { LanguageManager::Visuals_Menu::DroppedItemHide,             false, g_Config::DroppedItemHideColor,             &g_Config::bDroppedItemHide },
        { LanguageManager::Visuals_Menu::DroppedItemPelt,             false, g_Config::DroppedItemPeltColor,             &g_Config::bDroppedItemPelt },
        { LanguageManager::Visuals_Menu::DroppedItemKeratin,          false, g_Config::DroppedItemKeratinColor,          &g_Config::bDroppedItemKeratin },
        { LanguageManager::Visuals_Menu::DroppedItemChitin,           false, g_Config::DroppedItemChitinColor,           &g_Config::bDroppedItemChitin },
        { LanguageManager::Visuals_Menu::DroppedItemCorruptedPolymer, false, g_Config::DroppedItemCorruptedPolymerColor,  &g_Config::bDroppedItemCorruptedPolymer },
        { LanguageManager::Visuals_Menu::DroppedItemPolymer_Organic,  false, g_Config::DroppedItemPolymer_OrganicColor,  &g_Config::bDroppedItemPolymer_Organic },
        { LanguageManager::Visuals_Menu::DroppedItemPolymer,          false, g_Config::DroppedItemPolymerColor,          &g_Config::bDroppedItemPolymer }
    };

    void Visuals_Menu() {
        const char* tabVisuals = LanguageManager::Visuals_Menu::TabLabelVisuals;
        const char* tabTeam = LanguageManager::Visuals_Menu::TabLabelTeam;
        const char* secGlobal = LanguageManager::Visuals_Menu::SectionGlobal;
        const char* secExtra = LanguageManager::Visuals_Menu::SectionExtra;
        const char* secWorld = LanguageManager::Visuals_Menu::SectionWorld;
        const char* secTeam = LanguageManager::Visuals_Menu::SectionTeam;
        const char* secTeamExtra = LanguageManager::Visuals_Menu::SectionTeamExtra;

        ImGui::BeginDisabled(!g_Hook::PostRenderOK);
        {

            if (ImGui::BeginTabItem(tabVisuals)) {
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(14.0f, 14.0f));
                BeginTabRegion("VisualsRegion");
                ImGui::TextColored(ThemeColors::GetAccent(), secGlobal);
                DrawAnimatedSeparator();

                DrawCustomCheckbox(LanguageManager::Visuals_Menu::ESPEnabled, &g_Config::bESPEnabled);
                DrawCustomCombo(U8("##ESP_Type"), &ESP_Type_idx, ESP_Types);
                ImGui::SameLine();
                DrawCustomCombo(U8("##ESP_Scale"), &g_Config::ESPScaleIdx, ESP_Scale);

                switch (ESP_Type_idx)
                {
                case 0:
                    // 生存者
                    DrawAnimatedSeparator();
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::Box, &g_Config::bDrawBox, "BoxCol1", g_Config::BoxColor);
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::Name, &g_Config::bDrawName, "NameCol1", g_Config::NameColor);
                    DrawCustomCheckbox(LanguageManager::Visuals_Menu::HealthBar, &g_Config::bDrawHealthBar);
					ImGui::SameLine();
                    DrawMultiColorPicker("##HealthBarCol", true, { {"##HealthBarColor1", g_Config::HealthBarColor1},{"##HealthBarColor2", g_Config::HealthBarColor2}});

                    DrawColorPickerRow(LanguageManager::Visuals_Menu::Torpor, &g_Config::bDrawTorpor, "TorporCol", g_Config::TorporColor);
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::Ragdoll, &g_Config::bDrawRagdoll, "RagdollCol", g_Config::RagdollColor);
                    DrawAnimatedSeparator();

                    ImGui::TextColored(ThemeColors::GetAccent(), secExtra);
                    DrawAnimatedSeparator();
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::Distance, &g_Config::bDrawDistance, "DistCol1", g_Config::DistanceColor);
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::ShowAimPoints, &g_Config::bDrawAimPoints, "AimPointsCol1", g_Config::AimPointsColor);
                    DrawAnimatedSeparator();
                    break;

                case 1:
                    // 生物
                    DrawAnimatedSeparator();
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::DinoBox, &g_Config::bDinoDrawBox, "BoxCol1", g_Config::DinoBoxColor);
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::DinoName, &g_Config::bDinoDrawName, "NameCol1", g_Config::DinoNameColor);
                    DrawCustomCheckbox(LanguageManager::Visuals_Menu::DinoHealthBar, &g_Config::bDinoDrawHealthBar);
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::DinoTorpor, &g_Config::bDinoDrawTorpor, "TorporCol", g_Config::DinoTorporColor);
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::DinoRagdoll, &g_Config::bDinoDrawRagdoll, "RagdollCol", g_Config::DinoRagdollColor);
                    DrawAnimatedSeparator();

                    ImGui::TextColored(ThemeColors::GetAccent(), secExtra);
                    DrawAnimatedSeparator();
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::DinoDistance, &g_Config::bDinoDrawDistance, "DistCol1", g_Config::DinoDistanceColor);
                    DrawAnimatedSeparator();
                    break;

                case 2:
                    // 建筑
                    DrawAnimatedSeparator();
                    // DrawColorPickerRow(U8("显示建筑"), &g_Config::bDrawStructures, "StructureNameCol", g_Config::StructureNameColor);
                    DrawCustomCheckbox(LanguageManager::Visuals_Menu::ShowStructures, &g_Config::bDrawStructures);
                    ImGui::SameLine();
                    DrawMultiColorPicker("##StructureHealthCol", true, { {"##StructureHealthColor1", g_Config::StructureHealthColor1},{"##StructureHealthColor2", g_Config::StructureHealthColor2} });

                    if (g_Config::bDrawStructures) {
                        // DrawCustomColorPicker("StructureOwnerCol", g_Config::StructureOwnerColor, U8("建筑所有者"));
                        // DrawCustomColorPicker("StructureDistanceCol", g_Config::StructureDistanceColor, U8("建筑距离"));

                        // 不再需要这个东西了，现在分别设置全局建筑和队友建筑
                        // DrawCustomCheckbox(LanguageManager::Visuals_Menu::OnlyEnemyStructures, &g_Config::bOnlyDrawStructuresEnemy);
                        DrawCustomSliderFloat(LanguageManager::Visuals_Menu::StructureMaxDistance, &g_Config::StructureMaxDistance, 1.0f, 10000.0f, "%.0f", 1.0f, "m");
                    }
                    DrawAnimatedSeparator();
                    break;

                case 3:
                    // 物品
                    DrawAnimatedSeparator();
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::DroppedItems, &g_Config::bDrawDroppedItems, "DroppedItemNameCol", g_Config::DroppedItemNameColor);
                    if (g_Config::bDrawDroppedItems) {
                        DrawCustomMultiCombo(U8("##掉落物过滤"), dropped_items_combo, LanguageManager::Visuals_Menu::DroppedItemsCombo);
                    }
                    DrawAnimatedSeparator();
                    break;

                case 4:
                    // 世界
                    DrawAnimatedSeparator();
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::ShowWater, &g_Config::bDrawWater, "WaterNameCol", g_Config::WaterNameColor);
                    if (g_Config::bDrawWater) {
                        // DrawCustomColorPicker("WaterDistanceCol", g_Config::WaterDistanceColor, U8("水源距离"));
                        DrawCustomSliderFloat(LanguageManager::Visuals_Menu::WaterMaxCount, &g_Config::WaterMaxCount, 1.0f, 10.0f, "%.0f", 1.0f, LanguageManager::Visuals_Menu::WaterCountUnit);
                    }
                    DrawAnimatedSeparator();
                    break;
                }

                // 2026/4/11 @zetsr
                // 应该是之前移除aimbot的时候被注释掉了，我们可能并不需要分别做骨骼和瞄准点的cb，使用复选框或者单选框应该更好，以后可以加个glow类型的，不过骨骼确实很好看
                // DrawColorPickerRow(U8("显示瞄准骨骼"), &g_Config::bDrawAimSkeleton, "AimSkeletonCol1", g_Config::AimSkeletonColor);
                
                // DrawAnimatedSeparator();
                // ImGui::TextColored(ThemeColors::GetAccent(), secWorld);

                /*
                * 
                * 完全无用，粘贴自另一个游戏，无法正常工作
                * 
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

                static int Team_Type_idx = 0;
                std::vector<const char*> Team_Types = {
                    LanguageManager::Visuals_Menu::TYPEPlayer,
                    LanguageManager::Visuals_Menu::TYPEDino,
                    LanguageManager::Visuals_Menu::TYPEStructures,
                };

                DrawCustomCheckbox(LanguageManager::Visuals_Menu::ESPTeamEnabled, &g_Config::bESPTeamEnabled);
                DrawCustomCombo(U8("##ESP_Type"), &Team_Type_idx, Team_Types);
                // ImGui::SameLine();

                switch (Team_Type_idx)
                {
                case 0:
                    // 生存者
                    DrawAnimatedSeparator();
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamBox, &g_Config::bDrawBoxTeam, "BoxColTeam", g_Config::BoxColorTeam);
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamName, &g_Config::bDrawNameTeam, "NameColTeam", g_Config::NameColorTeam);
                    DrawCustomCheckbox(LanguageManager::Visuals_Menu::TeamHealthBar, &g_Config::bDrawHealthBarTeam);
                    ImGui::SameLine();
                    DrawMultiColorPicker("##TeamHealthCol", true, { {"##TeamHealthColor1", g_Config::TeamHealthColor1},{"##TeamHealthColor2", g_Config::TeamHealthColor2} });
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamTorpor, &g_Config::bDrawTorporTeam, "TorporColTeam", g_Config::TorporColorTeam);
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamRagdoll, &g_Config::bDrawRagdollTeam, "RagdollColTeam", g_Config::RagdollColorTeam);
                    DrawAnimatedSeparator();

                    ImGui::TextColored(ThemeColors::GetAccent(), secTeamExtra);
                    DrawAnimatedSeparator();
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamDistance, &g_Config::bDrawDistanceTeam, "DistColTeam", g_Config::DistanceColorTeam);
                    DrawAnimatedSeparator();
                    break;
                case 1:
                    // 生物
                    DrawAnimatedSeparator();
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamDinoBox, &g_Config::bTeamDinoDrawBox, "BoxColTeam", g_Config::TeamDinoBoxColor);
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamDinoName, &g_Config::bTeamDinoDrawName, "NameColTeam", g_Config::TeamDinoNameColor);
                    DrawCustomCheckbox(LanguageManager::Visuals_Menu::TeamDinoHealthBar, &g_Config::bTeamDinoDrawHealthBar);
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamDinoTorpor, &g_Config::bTeamDinoDrawTorpor, "TorporColTeam", g_Config::TeamDinoTorporColor);
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamDinoRagdoll, &g_Config::bTeamDinoDrawRagdoll, "RagdollColTeam", g_Config::TeamDinoRagdollColor);
                    DrawAnimatedSeparator();

                    ImGui::TextColored(ThemeColors::GetAccent(), secTeamExtra);
                    DrawAnimatedSeparator();
                    DrawColorPickerRow(LanguageManager::Visuals_Menu::TeamDinoDistance, &g_Config::bTeamDinoDrawDistance, "DistColTeam", g_Config::TeamDinoDistanceColor);
                    DrawAnimatedSeparator();
                    break;
                case 2:
                    // 建筑
                    DrawAnimatedSeparator();
                    DrawCustomCheckbox(LanguageManager::Visuals_Menu::TeamShowStructures, &g_Config::bTeamDrawStructures);
                    ImGui::SameLine();
                    DrawMultiColorPicker("##TeamStructureHealthCol", true, { {"##TeamStructureHealthColor1", g_Config::TeamStructureHealthColor1},{"##TeamStructureHealthColor2", g_Config::TeamStructureHealthColor2} });
                    if (g_Config::bTeamDrawStructures) {
                        DrawCustomSliderFloat(LanguageManager::Visuals_Menu::TeamStructureMaxDistance, &g_Config::TeamStructureMaxDistance, 1.0f, 10000.0f, "%.0f", 1.0f, "m");
                    }
                    DrawAnimatedSeparator();
                    break;
                }

                EndTabRegion();
                ImGui::PopStyleVar();
                ImGui::EndTabItem();
            }
            ImGui::EndDisabled();
        }
    }
}