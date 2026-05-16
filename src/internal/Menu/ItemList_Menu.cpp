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
        SDK::FString OriginalFName; // 用于丢弃时的 Filter
        int TotalQuantity = 0;
        SDK::UPrimalItem* LastItemPtr = nullptr; // 用于获取描述和使用物品（取最后一组）
    };

    void ItemList_Menu() {
        const char* tabLabel = LanguageManager::ItemList_Menu::TabLabel;
        const char* searchHint = LanguageManager::ItemList_Menu::SearchHint;
        const char* ttItem = LanguageManager::ItemList_Menu::TooltipItem;
        const char* ttTotal = LanguageManager::ItemList_Menu::TooltipTotal;
        const char* ttDesc = LanguageManager::ItemList_Menu::TooltipDesc;
        const char* ttCrafter = LanguageManager::ItemList_Menu::TooltipCrafter;
        const char* ttTribe = LanguageManager::ItemList_Menu::TooltipTribe;
        const char* actUseOne = LanguageManager::ItemList_Menu::ActionUseOne;
        const char* actDrop = LanguageManager::ItemList_Menu::ActionDrop;
        const char* noItems = LanguageManager::ItemList_Menu::NoItems;

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

                    // 搜索过滤判断
                    if (strlen(g_Config::itemSearchBuf) > 0 && name.find(g_Config::itemSearchBuf) == std::string::npos)
                        continue;

                    auto& m = mergedMap[name];
                    m.DisplayName = name;
                    m.OriginalFName = Item->DescriptiveNameBase;
                    m.TotalQuantity += Item->ItemQuantity;
                    m.LastItemPtr = Item; // 存一个指针用于获取描述
                }

                // 2. 渲染归并后的列表
                int idx = 0;
                for (auto const& [name, data] : mergedMap) {
                    idx++;
                    std::string label = data.DisplayName + " x" + std::to_string(data.TotalQuantity);

                    if (ImGui::Selectable((label + "##" + std::to_string(idx)).c_str())) {
                        ImGui::OpenPopup(("ItemPopup" + std::to_string(idx)).c_str());
                    }

                    // 3. 悬浮窗显示详细信息
                    if (ImGui::IsItemHovered() && data.LastItemPtr && data.LastItemPtr->CustomItemDescription) {
                        ImGui::BeginTooltip();
                        SDK::UPrimalItem* p = data.LastItemPtr;

                        ImGui::TextColored(ThemeColors::GetAccent(), ttItem, data.DisplayName.c_str());
                        ImGui::Text(ttTotal, data.TotalQuantity);

                        // 描述信息
                        std::string desc = p->CustomItemDescription.ToString();
                        if (desc.empty() || desc == "None") desc = p->ItemDescription.ToString();
                        if (!desc.empty() && desc != "None") {
                            ImGui::Separator();
                            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.0f);
                            ImGui::TextDisabled(ttDesc, desc.c_str());
                            ImGui::PopTextWrapPos();
                        }

                        // 制造者信息
                        if (!p->CrafterCharacterName.ToString().empty() && p->CrafterCharacterName.ToString() != "None") {
                            ImGui::Separator();
                            ImGui::Text(ttCrafter, p->CrafterCharacterName.ToString().c_str());
                            ImGui::Text(ttTribe, p->CrafterTribeName.ToString().c_str());
                        }
                        ImGui::EndTooltip();
                    }

                    // 4. 交互菜单
                    if (ImGui::BeginPopup(("ItemPopup" + std::to_string(idx)).c_str())) {
                        if (ImGui::MenuItem(actUseOne)) {
                            if (data.LastItemPtr) {
                                // 不再直接调用 PC->ServerRequestInventoryUseItem
                                g_Config::useItemID = data.LastItemPtr->ItemID;
                                g_Config::bUseItem = true;
                            }
                        }

                        if (ImGui::MenuItem(actDrop)) {
                            // 不再直接调用 PC->ServerRequestRemoteDropAllItems
                            g_Config::dropItemID = data.LastItemPtr->ItemID;
                            g_Config::bDropItem = true;
                        }
                        ImGui::EndPopup();
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