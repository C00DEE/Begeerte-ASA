#pragma once
#define NOMINMAX
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../Config/Configs.h"
#include "../Util/Util.h"
#include "../Config/ConfigManager.h"
#include "../Lua/LuaManager.h"
#include "../Hack/Hack.h"
#include "../../external/SDK/SDK_Headers.hpp"
#include "../Legit/Aimbot/Aimbot.h"

namespace g_UWorld {
	void Tick(SDK::UWorld* world) {
        g_Aimbot::Tick();
        g_Hack::OutBody();
		if (g_Config::bSuicide) {
			g_Hack::Suicide(world);
			g_Config::bSuicide = false;
		}

		if (g_Config::bUnlockExplorerNotes) {
			g_Hack::UnlockExplorerNotes(world);
		}

		if (g_Config::bAutoFeed) {
			g_Hack::AutoFeed(world);
		}

        if (g_Config::bSuperFlyer) {
			g_Hack::SuperFlyer(world);
		}

        if (g_Config::bAutomatic) {
            g_Hack::AutoSwapBrokenEquipment();
        }

        SDK::AShooterPlayerController* PC = (SDK::AShooterPlayerController*)g_Util::GetLocalPC();
        SDK::APrimalCharacter* Character = PC ? (SDK::APrimalCharacter*)PC->Pawn : nullptr;
        SDK::UPrimalInventoryComponent* Inv = Character ? Character->MyInventoryComponent : nullptr;

        if (PC && Character && Inv) {
            // 执行丢弃逻辑
            if (g_Config::bDropItem) {
                PC->ServerDropFromRemoteInventory(Inv, g_Config::dropItemID);
                // 重置状态
                g_Config::bDropItem = false;
            }

            // 执行使用逻辑
            if (g_Config::bUseItem) {
                // 执行操作
                PC->ServerRequestInventoryUseItem(Inv, g_Config::useItemID);

                // 重置状态
                g_Config::bUseItem = false;
                // SDK::FItemNetID 通常是结构体，不需要像字符串一样手动清理，下次会被覆盖
            }
        }

		LuaManager::Get().Lua_OnWorldTick();
	}
}