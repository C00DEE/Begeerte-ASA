#pragma once
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../../external/SDK/SDK_Headers.hpp"
#include "../Config/Configs.h"
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>

namespace g_Util {
    static const float inv255 = 1.0f / 255.0f; 

    inline const char* GetKeyName(UINT vk) {
        if (vk >= 'A' && vk <= 'Z') {
            static char buf[2] = { 0 };
            buf[0] = (char)vk;
            return buf;
        }
        if (vk >= '0' && vk <= '9') {
            static char buf[2] = { 0 };
            buf[0] = (char)vk;
            return buf;
        }
        switch (vk) {
        case VK_F1: return "F1"; case VK_F2: return "F2"; case VK_F3: return "F3"; case VK_F4: return "F4";
        case VK_F5: return "F5"; case VK_F6: return "F6"; case VK_F7: return "F7"; case VK_F8: return "F8";
        case VK_F9: return "F9"; case VK_F10: return "F10"; case VK_F11: return "F11"; case VK_F12: return "F12";
        case VK_INSERT: return "INSERT"; case VK_DELETE: return "DELETE"; case VK_HOME: return "HOME";
        case VK_END: return "END"; case VK_PRIOR: return "PAGE UP"; case VK_NEXT: return "PAGE DOWN";
        case VK_TAB: return "TAB"; case VK_LSHIFT: return "LSHIFT"; case VK_RSHIFT: return "RSHIFT";
        case VK_LCONTROL: return "LCTRL"; case VK_RCONTROL: return "RCTRL"; case VK_LMENU: return "LALT";
        case VK_RMENU: return "RALT"; case VK_CAPITAL: return "CAPSLOCK"; case VK_SPACE: return "SPACE";
        case VK_RETURN: return "ENTER"; case VK_ESCAPE: return "ESCAPE";
        default: return "UNKNOWN";
        }
    }

    inline std::string ToLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return std::tolower(c);
            });
        return s;
    }

    inline ImU32 GetU32Color(float color[4]) {
        return ImGui::ColorConvertFloat4ToU32(*(ImVec4*)color);
    }

    inline ImU32 ToImColor(float r, float g, float b, float a) {
        return ImGui::ColorConvertFloat4ToU32(ImVec4(r * inv255, g * inv255, b * inv255, a * inv255));
    }

    inline ImU32 GetHealthColor(float healthPercent) {
        healthPercent = fmaxf(0.0f, fminf(healthPercent, 1.0f));

        float r = fminf(1.0f, 2.0f - 2.0f * healthPercent);
        float g = fminf(1.0f, 2.0f * healthPercent);

        return ToImColor(r * 255.0f, g * 255.0f, 0.0f, 255.0f);
    }

    inline bool IsCookedMeat(SDK::UPrimalItem* Item) {
        if (!Item || !Item->Class) return false;
        std::string name = Item->Class->GetName();
        return name.find("PrimalItemConsumable_CookedMeat") != std::string::npos;
    }

    inline bool IsRawMeat(SDK::UPrimalItem* Item) {
        if (!Item || !Item->Class) return false;
        std::string name = Item->Class->GetName();
        return name.find("PrimalItemConsumable_RawMeat") != std::string::npos;
    }

    inline bool IsEntityMatch(std::string displayName, std::string filter) {
        if (filter.empty()) return true;

        std::string nameLower = g_Util::ToLower(displayName);
        std::string filterLower = g_Util::ToLower(filter);

        size_t lastPos = 0;
        for (size_t j = 0; j < filterLower.length(); ) {
            unsigned char c = static_cast<unsigned char>(filterLower[j]);
            int charLen = 1;
            if (c >= 0xf0) charLen = 4;
            else if (c >= 0xe0) charLen = 3;
            else if (c >= 0xc0) charLen = 2;

            std::string sub = filterLower.substr(j, charLen);
            size_t foundPos = nameLower.find(sub, lastPos);

            if (foundPos == std::string::npos) return false;

            lastPos = foundPos + charLen;
            j += charLen;
        }
        return true;
    }

    inline static std::vector<std::string> SplitFilterTokens(const std::string& filter) {
        std::vector<std::string> tokens;
        std::stringstream ss(filter);
        std::string token;
        while (std::getline(ss, token, ',')) {
            size_t start = token.find_first_not_of(" \t");
            size_t end = token.find_last_not_of(" \t");
            if (start != std::string::npos && end != std::string::npos) {
                tokens.push_back(token.substr(start, end - start + 1));
            }
            else if (start != std::string::npos) {
                tokens.push_back(token.substr(start));
            }
        }
        return tokens;
    }

    inline static bool IsEntityMatchMulti(const std::string& displayName, const char* filterBuf) {
        std::string filter = filterBuf;
        if (filter.empty()) return true;

        std::vector<std::string> tokens = SplitFilterTokens(filter);
        if (tokens.empty()) return true;

        for (const std::string& token : tokens) {
            if (token.empty()) continue;
            if (g_Util::IsEntityMatch(displayName, token.c_str())) return true;
        }
        return false;
    }

    inline bool IsStructureMatch(const std::string& structureName, const std::string& filter) {
        if (filter.empty()) return true;

        std::string lowerName = structureName;
        std::string lowerFilter = filter;

        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), ::tolower);

        return lowerName.find(lowerFilter) != std::string::npos;
    }

    static bool IsStructureMatchMulti(const std::string& structureName, const char* filterBuf) {
        std::string filter = filterBuf;
        if (filter.empty()) return true;

        std::vector<std::string> tokens = SplitFilterTokens(filter);
        if (tokens.empty()) return true;

        for (const std::string& token : tokens) {
            if (token.empty()) continue;
            if (g_Util::IsStructureMatch(structureName, token.c_str())) return true;
        }
        return false;
    }

    inline SDK::APlayerController* GetLocalPC() {
        SDK::UWorld* World = SDK::UWorld::GetWorld();

        if (!World) {
            return nullptr;
        }

        if (!World->OwningGameInstance) {
            return nullptr;
        }

        if (World->OwningGameInstance->LocalPlayers.Num() == 0) {
            return nullptr;
        }

        auto LocalPlayer = World->OwningGameInstance->LocalPlayers[0];
        if (!LocalPlayer) {
            return nullptr;
        }

        auto PC = LocalPlayer->PlayerController;
        if (!PC) {
            return nullptr;
        }

        return PC;
    }

    inline float GetAngleDistance(SDK::FVector CamLoc, SDK::FVector TargetLoc, SDK::FRotator CamRot) {
        SDK::FVector Diff = { TargetLoc.X - CamLoc.X, TargetLoc.Y - CamLoc.Y, TargetLoc.Z - CamLoc.Z };
        SDK::FVector DirToTarget = SDK::UKismetMathLibrary::Normal(Diff, 0.0001f);
        SDK::FVector CamForward = SDK::UKismetMathLibrary::GetForwardVector(CamRot);
        float Dot = SDK::UKismetMathLibrary::Dot_VectorVector(DirToTarget, CamForward);
        Dot = SDK::UKismetMathLibrary::FClamp(Dot, -1.0f, 1.0f);
        return SDK::UKismetMathLibrary::DegAcos(Dot);
    }

    inline void MimicMouseClick(bool bPress) {
        INPUT input = { 0 };
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = bPress ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));
    }

    inline void ProcessDinoFeed(SDK::AShooterPlayerController* PC, SDK::APrimalDinoCharacter* Dino) {
        if (!Dino || Dino->IsDead()) return;

        float curHP = Dino->GetHealth();
        float maxHP = Dino->GetMaxHealth();
        if (maxHP <= 0.0f || curHP >= (maxHP - 1.0f)) return;

        SDK::UPrimalInventoryComponent* Inv = Dino->MyInventoryComponent;
        if (!Inv) return;

        SDK::TArray<SDK::UPrimalItem*>& Items = Inv->InventoryItems;
        SDK::UPrimalItem* BestMeat = nullptr;

        // 寻找食物：优先熟肉，其次生肉
        for (int i = 0; i < Items.Num(); i++) {
            SDK::UPrimalItem* Item = Items[i];
            if (!Item || !Item->Class) continue;

            if (IsCookedMeat(Item)) {
                BestMeat = Item;
                break; // 熟肉回血多，找到直接跳出
            }
            if (IsRawMeat(Item)) {
                BestMeat = Item; // 先记下生肉，继续看有没有熟肉
            }
        }

        if (BestMeat) {
            // 使用针对龙的 RPC
            PC->ServerRequestInventoryUseItem(Inv, BestMeat->ItemID);
        }
    }

    inline static std::string IntToStr(int v)
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d", v);
        return buf; // NRVO / SSO，比 std::to_string 开销低
    }

    inline static float ApproachAlpha(float cur, float target, float deltaSeconds, float fadeTime)
    {
        if (fadeTime <= 0.0f) return target;
        float diff = target - cur;
        float maxStep = deltaSeconds / fadeTime;
        if (fabsf(diff) <= maxStep) return target;
        return cur + (diff > 0.0f ? maxStep : -maxStep);
    }

    inline static ImU32 ResolveDroppedItemColor(const std::string& className, float itemRating, int quantity)
    {
        const char* cn = className.c_str();

        if (strstr(cn, "PrimalItem_WeaponEmptyCryopod"))
            return g_Util::GetU32Color(g_Config::DroppedItemCryopodColor);
        if (strstr(cn, "Egg"))
            return g_Util::GetU32Color(g_Config::DroppedItemEggColor);
        if (quantity >= 1000)
            return g_Util::GetU32Color(g_Config::DroppedItemPiledColor);
        if (strstr(cn, "PrimalItemResource_FungalWood") || strstr(cn, "PrimalItemResource_Wood"))
            return g_Util::GetU32Color(g_Config::DroppedItemWoodColor);
        if (strstr(cn, "PrimalItemResource_Thatch"))
            return g_Util::GetU32Color(g_Config::DroppedItemThatchColor);
        if (strstr(cn, "PrimalItemResource_Hide"))
            return g_Util::GetU32Color(g_Config::DroppedItemHideColor);
        if (strstr(cn, "PrimalItemResource_Pelt"))
            return g_Util::GetU32Color(g_Config::DroppedItemPeltColor);
        if (strstr(cn, "PrimalItemResource_Keratin"))
            return g_Util::GetU32Color(g_Config::DroppedItemKeratinColor);
        if (strstr(cn, "PrimalItemResource_Chitin"))
            return g_Util::GetU32Color(g_Config::DroppedItemChitinColor);
        if (strstr(cn, "PrimalItemResource_CorruptedPolymer"))
            return g_Util::GetU32Color(g_Config::DroppedItemCorruptedPolymerColor);
        if (strstr(cn, "PrimalItemResource_Polymer_Organic"))
            return g_Util::GetU32Color(g_Config::DroppedItemPolymer_OrganicColor);
        if (strstr(cn, "PrimalItemResource_Polymer"))
            return g_Util::GetU32Color(g_Config::DroppedItemPolymerColor);
        if (strstr(cn, "PrimalItemResource_ScrapMetalIngot") ||
            strstr(cn, "PrimalItemResource_MetalIngot") ||
            strstr(cn, "PrimalItemResource_ScrapMetal") ||
            strstr(cn, "PrimalItemResource_Metal"))
            return g_Util::GetU32Color(g_Config::DroppedItemMetalColor);
        if (strstr(cn, "PrimalItemResource_Stone"))
            return g_Util::GetU32Color(g_Config::DroppedItemStoneColor);
        if (strstr(cn, "PrimalItemResource_Crystal"))
            return g_Util::GetU32Color(g_Config::DroppedItemCrystalColor);
        if (strstr(cn, "PrimalItemResource_Gem_Fertile") ||
            strstr(cn, "PrimalItemResource_Gem_BioLum") ||
            strstr(cn, "PrimalItemResource_Gem_Element") ||
            strstr(cn, "PrimalItemResource_BlueSap") ||
            strstr(cn, "PrimalItemResource_RedSap"))
            return g_Util::GetU32Color(g_Config::DroppedItemGemColor);
        if (strstr(cn, "PrimalItemResource_Silicon") ||
            strstr(cn, "PrimalItemResource_BlackPearl"))
            return g_Util::GetU32Color(g_Config::DroppedItemPearlColor);
        if (strstr(cn, "PrimalItemConsumable_SpoiledMeat"))
            return g_Util::GetU32Color(g_Config::DroppedItemSpoiledMeatColor);
        if (strstr(cn, "PrimalItemConsumable_RawMeat") ||
            strstr(cn, "PrimalItemConsumable_RawPrimeMeat") ||
            strstr(cn, "PrimalItemConsumable_RawMutton") ||
            strstr(cn, "PrimalItemConsumable_RawPrimeMeat_Fish") ||
            strstr(cn, "PrimalItemConsumable_RawMeat_Fish") ||
            strstr(cn, "PrimalItemConsumable_CookedMeat") ||
            strstr(cn, "PrimalItemConsumable_CookedPrimeMeat") ||
            strstr(cn, "PrimalItemConsumable_CookedLambChop") ||
            strstr(cn, "PrimalItemConsumable_CookedPrimeMeat_Fish") ||
            strstr(cn, "PrimalItemConsumable_CookedMeat_Fish") ||
            strstr(cn, "PrimalItemConsumable_CookedMeat_Jerky") ||
            strstr(cn, "PrimalItemConsumable_CookedPrimeMeat_Jerky"))
            return g_Util::GetU32Color(g_Config::DroppedItemMeatColor);

        // 按品质评级上色
        if (itemRating >= 10.0f) return g_Util::ToImColor(0, 255, 255, 255);
        if (itemRating >= 7.0f) return g_Util::ToImColor(255, 255, 0, 255);
        if (itemRating >= 4.5f) return g_Util::ToImColor(160, 32, 240, 255);
        if (itemRating >= 2.5f) return g_Util::ToImColor(0, 191, 255, 255);
        if (itemRating >= 1.25f) return g_Util::ToImColor(50, 205, 50, 255);

        return g_Util::GetU32Color(g_Config::DroppedItemNameColor);
    }

    inline bool IsDroppedItemAllowed(const std::string& className, int quantity)
    {
        const char* cn = className.c_str();

        if (strstr(cn, "PrimalItem_WeaponEmptyCryopod"))
            return g_Config::bDroppedItemCryopod;
        if (strstr(cn, "Egg"))
            return g_Config::bDroppedItemEgg;
        if (quantity >= 1000)
            return g_Config::bDroppedItemPiled;
        if (strstr(cn, "PrimalItemResource_FungalWood") || strstr(cn, "PrimalItemResource_Wood"))
            return g_Config::bDroppedItemWood;
        if (strstr(cn, "PrimalItemResource_Thatch"))
            return g_Config::bDroppedItemThatch;
        if (strstr(cn, "PrimalItemResource_Hide"))
            return g_Config::bDroppedItemHide;
        if (strstr(cn, "PrimalItemResource_Pelt"))
            return g_Config::bDroppedItemPelt;
        if (strstr(cn, "PrimalItemResource_Keratin"))
            return g_Config::bDroppedItemKeratin;
        if (strstr(cn, "PrimalItemResource_Chitin"))
            return g_Config::bDroppedItemChitin;
        if (strstr(cn, "PrimalItemResource_CorruptedPolymer"))
            return g_Config::bDroppedItemCorruptedPolymer;
        if (strstr(cn, "PrimalItemResource_Polymer_Organic"))
            return g_Config::bDroppedItemPolymer_Organic;
        if (strstr(cn, "PrimalItemResource_Polymer"))
            return g_Config::bDroppedItemPolymer;
        if (strstr(cn, "PrimalItemResource_ScrapMetalIngot") ||
            strstr(cn, "PrimalItemResource_MetalIngot") ||
            strstr(cn, "PrimalItemResource_ScrapMetal") ||
            strstr(cn, "PrimalItemResource_Metal"))
            return g_Config::bDroppedItemMetal;
        if (strstr(cn, "PrimalItemResource_Stone"))
            return g_Config::bDroppedItemStone;
        if (strstr(cn, "PrimalItemResource_Crystal"))
            return g_Config::bDroppedItemCrystal;
        if (strstr(cn, "PrimalItemResource_Gem_Fertile") ||
            strstr(cn, "PrimalItemResource_Gem_BioLum") ||
            strstr(cn, "PrimalItemResource_Gem_Element") ||
            strstr(cn, "PrimalItemResource_BlueSap") ||
            strstr(cn, "PrimalItemResource_RedSap"))
            return g_Config::bDroppedItemGem;
        if (strstr(cn, "PrimalItemResource_Silicon") ||
            strstr(cn, "PrimalItemResource_BlackPearl"))
            return g_Config::bDroppedItemPearl;
        if (strstr(cn, "PrimalItemConsumable_SpoiledMeat"))
            return g_Config::bDroppedItemSpoiledMeat;
        if (strstr(cn, "PrimalItemConsumable_RawMeat") ||
            strstr(cn, "PrimalItemConsumable_RawPrimeMeat") ||
            strstr(cn, "PrimalItemConsumable_RawMutton") ||
            strstr(cn, "PrimalItemConsumable_RawPrimeMeat_Fish") ||
            strstr(cn, "PrimalItemConsumable_RawMeat_Fish") ||
            strstr(cn, "PrimalItemConsumable_CookedMeat") ||
            strstr(cn, "PrimalItemConsumable_CookedPrimeMeat") ||
            strstr(cn, "PrimalItemConsumable_CookedLambChop") ||
            strstr(cn, "PrimalItemConsumable_CookedPrimeMeat_Fish") ||
            strstr(cn, "PrimalItemConsumable_CookedMeat_Fish") ||
            strstr(cn, "PrimalItemConsumable_CookedMeat_Jerky") ||
            strstr(cn, "PrimalItemConsumable_CookedPrimeMeat_Jerky"))
            return g_Config::bDroppedItemMeat;

        // 如果没有被上述特定分类命中，默认允许绘制（例如武器、装备等普通物品）
        return true;
    }
}