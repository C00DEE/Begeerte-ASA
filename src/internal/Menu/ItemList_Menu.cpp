#pragma once
#define NOMINMAX  
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../Config/Configs.h"
#include "ConfigImGui.h"
#include "ItemList_Menu.h"
#include "../Util/Util.h"
#include "../../external/SDK/SDK_Headers.hpp"
#include "../Language/LanguageManager.h"
#include <map>
#include <string>
#include <vector>

namespace g_DrawImGui {

    struct MergedItem {
        std::string DisplayName;
        SDK::FString OriginalFName;
        int TotalQuantity = 0;
        SDK::UPrimalItem* LastItemPtr = nullptr;
    };

    void ItemList_Menu() {
        const char* tabLabel = LanguageManager::ItemList_Menu::TabLabel;
        const char* searchHint = LanguageManager::ItemList_Menu::SearchHint;
        const char* ttItem = LanguageManager::ItemList_Menu::TooltipItem;
        const char* ttTotal = LanguageManager::ItemList_Menu::TooltipTotal;
        const char* ttDesc = LanguageManager::ItemList_Menu::TooltipDesc;
        const char* ttCrafter = LanguageManager::ItemList_Menu::TooltipCrafter;
        const char* ttTribe = LanguageManager::ItemList_Menu::TooltipTribe;
        const char* noItems = LanguageManager::ItemList_Menu::NoItems;

        // 【新增】用于划过式连发的全局时间控制
        static double s_lastActionTime = 0.0;
        const double actionDelay = 0.1; // 连发间隔（秒）

        if (ImGui::BeginTabItem(tabLabel)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(14.0f, 14.0f));
            BeginTabRegion("ItemListRegion");

            ImGui::InputTextWithHint("##ItemSearch", searchHint, g_Config::itemSearchBuf, IM_ARRAYSIZE(g_Config::itemSearchBuf));

            DrawAnimatedSeparator();

            SDK::AShooterPlayerController* PC = (SDK::AShooterPlayerController*)g_Util::GetLocalPC();
            SDK::APawn* LocalPawn = PC ? PC->Pawn : nullptr;
            SDK::APrimalCharacter* Character = (SDK::APrimalCharacter*)LocalPawn;
            SDK::UPrimalInventoryComponent* Inv = Character ? Character->MyInventoryComponent : nullptr;

            if (PC && LocalPawn && Character && Inv->InventoryItems.Num() > 0) {

                // 1. 数据归并处理
                std::map<std::string, MergedItem> mergedMap;
                SDK::TArray<SDK::UPrimalItem*>& Items = Inv->InventoryItems;

                for (int i = 0; i < Items.Num(); i++) {
                    SDK::UPrimalItem* Item = Items[i];
                    if (!Item || !Item->DescriptiveNameBase) continue;

                    std::string name = Item->DescriptiveNameBase.ToString();
                    if (name.empty() || name == "None") continue;

                    if (strlen(g_Config::itemSearchBuf) > 0 && name.find(g_Config::itemSearchBuf) == std::string::npos)
                        continue;

                    auto& m = mergedMap[name];
                    m.DisplayName = name;
                    m.OriginalFName = Item->DescriptiveNameBase;
                    m.TotalQuantity += Item->ItemQuantity;
                    m.LastItemPtr = Item;
                }

                // 2. 渲染归并后的列表
                int idx = 0;
                double currentTime = ImGui::GetTime();

                for (auto const& [name, data] : mergedMap) {
                    idx++;
                    std::string label = data.DisplayName + " x" + std::to_string(data.TotalQuantity);

                    // 渲染物品条目
                    ImGui::Selectable((label + "##" + std::to_string(idx)).c_str());

                    // --- 核心修改：基于 Hover 状态的划过式长按自适应连发 ---
                    if (ImGui::IsItemHovered() && data.LastItemPtr) {

                        // 检查连发冷却时间是否到了
                        if (currentTime - s_lastActionTime >= actionDelay) {

                            // 只要鼠标悬停在当前行，且【左键正被按住】
                            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                                g_Config::useItemID = data.LastItemPtr->ItemID;
                                g_Config::bUseItem = true;
                                s_lastActionTime = currentTime; // 更新时间锚点
                            }
                            // 或者是【右键正被按住】
                            else if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
                                g_Config::dropItemID = data.LastItemPtr->ItemID;
                                g_Config::bDropItem = true;
                                s_lastActionTime = currentTime; // 更新时间锚点
                            }
                        }
                    }

                    // 3. 悬浮窗显示详细信息
                    if (ImGui::IsItemHovered() && data.LastItemPtr) {
                        ImGui::BeginTooltip();
                        SDK::UPrimalItem* p = data.LastItemPtr;

                        ImGui::TextColored(ThemeColors::GetAccent(), ttItem, data.DisplayName.c_str());
                        ImGui::Text(ttTotal, data.TotalQuantity);

                        std::string desc = "";
                        if (p->CustomItemDescription.IsValid() && !p->CustomItemDescription.ToString().empty()) {
                            desc = p->CustomItemDescription.ToString();
                        }
                        if (desc.empty() || desc == "None") {
                            if (p->ItemDescription.IsValid()) {
                                desc = p->ItemDescription.ToString();
                            }
                        }

                        if (!desc.empty() && desc != "None") {
                            ImGui::Separator();
                            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.0f);
                            ImGui::TextDisabled(ttDesc, desc.c_str());
                            ImGui::PopTextWrapPos();
                        }

                        if (p->CrafterCharacterName.IsValid() && !p->CrafterCharacterName.ToString().empty() && p->CrafterCharacterName.ToString() != "None") {
                            ImGui::Separator();
                            ImGui::Text(ttCrafter, p->CrafterCharacterName.ToString().c_str());
                            if (p->CrafterTribeName.IsValid()) {
                                ImGui::Text(ttTribe, p->CrafterTribeName.ToString().c_str());
                            }
                        }
                        ImGui::EndTooltip();
                    }
                }
            }
            else {
                ImGui::TextDisabled(noItems);
            }

            DrawAnimatedSeparator();
            EndTabRegion();
            ImGui::PopStyleVar();
            ImGui::EndTabItem();
        }
    }
}