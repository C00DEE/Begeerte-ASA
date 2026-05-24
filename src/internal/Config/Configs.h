#pragma once
#define NOMINMAX  
#include "../../external/SDK/SDK_Headers.hpp"

namespace g_Config {
    // 菜单设置
    inline bool bMenuNeedReset = false;
    inline bool bMenuLockResize = false;
    inline float MenuColor[4] = { 0.431f, 0.906f, 0.718f, 1.000f };

    // 其他功能
    inline bool bSuperFlyer = false;
    inline bool bForceTurn = false;
    inline bool bAutoFeed = false;
    inline bool bUnlockExplorerNotes = false;
    inline bool bSuicide = false;
    inline bool bDropItem = false;
    inline SDK::FItemNetID dropItemID;
    inline bool bUseItem = false;
    inline SDK::FItemNetID useItemID;

    inline bool bLogDamage = true;
    inline float LogDamageColor[4] = { 1.0f, 1.0f, 1.0f, 0.80f };

    // 生物列表
    inline char entitySearchBuf[256] = { 0 };
    inline bool bEnableFilter = false;

    // 建筑列表
    inline char structureSearchBuf[256] = "";
    inline bool bEnableStructureFilter = false;

    // 物品列表
    inline char itemSearchBuf[256] = "";

    // 自瞄
    inline bool bAimbotEnabled = false;
    inline float AimbotFOV = 30.0f;
    inline float AimbotSmooth = 50.0f;
    inline float AimbotRCX = 100.0f;
    inline float AimbotRCY = 100.0f;

    // 扳机
    inline bool bTriggerbotEnabled = false;
    inline float TriggerDelay = 0.0f;
    inline float TriggerRandomPercent = 0.0f;
    inline float TriggerHitChance = 50.0f;

    // 掉落物
    inline bool bDrawDroppedItems = true;
    inline float DroppedItemMaxDistance = 500.0f;

    inline float DroppedItemNameColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    inline float DroppedItemDistanceColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    inline float DroppedItemMeatColor[4] = { 1.0f, 0.62f, 0.34f, 1.0f };
    inline float DroppedItemCryopodColor[4] = { 1.0f, 0.39f, 1.0f, 1.0f };
    inline float DroppedItemEggColor[4] = { 1.0f, 0.96f, 0.92f, 1.0f };
    inline float DroppedItemPiledColor[4] = { 0.19f, 1.0f, 0.19f, 1.0f };
    inline float DroppedItemWoodColor[4] = { 0.92f, 0.53f, 0.2f, 1.0f };
    inline float DroppedItemThatchColor[4] = { 0.94f, 0.94f, 0.71f, 1.0f };
    inline float DroppedItemHideColor[4] = { 1.0f, 0.82f, 0.67f, 1.0f };
    inline float DroppedItemPeltColor[4] = { 0.69f, 0.69f, 0.69f, 1.0f };
    inline float DroppedItemKeratinColor[4] = { 0.57f, 1.0f, 0.88f, 1.0f };
    inline float DroppedItemChitinColor[4] = { 0.57f, 1.0f, 0.88f, 1.0f };
    inline float DroppedItemCorruptedPolymerColor[4] = { 0.57f, 1.0f, 0.88f, 1.0f };
    inline float DroppedItemPolymer_OrganicColor[4] = { 0.57f, 1.0f, 0.88f, 1.0f };
    inline float DroppedItemPolymerColor[4] = { 0.57f, 1.0f, 0.88f, 1.0f };
    inline float DroppedItemMetalColor[4] = { 0.75f, 0.75f, 0.75f, 1.0f };
    inline float DroppedItemStoneColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    inline float DroppedItemCrystalColor[4] = { 0.78f, 0.96f, 1.0f, 1.0f };
    inline float DroppedItemGemColor[4] = { 0.0f, 1.0f, 0.5f, 1.0f };
    inline float DroppedItemPearlColor[4] = { 0.29f, 0.0f, 0.51f, 1.0f };
    inline float DroppedItemSpoiledMeatColor[4] = { 0.39f, 0.69f, 0.39f, 1.0f };

    inline bool bDroppedItemMeat = false;
    inline bool bDroppedItemCryopod = false;
    inline bool bDroppedItemEgg = false;
    inline bool bDroppedItemPiled = false;
    inline bool bDroppedItemWood = false;
    inline bool bDroppedItemThatch = false;
    inline bool bDroppedItemHide = false;
    inline bool bDroppedItemPelt = false;
    inline bool bDroppedItemKeratin = false;
    inline bool bDroppedItemChitin = false;
    inline bool bDroppedItemCorruptedPolymer = false;
    inline bool bDroppedItemPolymer_Organic = false;
    inline bool bDroppedItemPolymer = false;
    inline bool bDroppedItemMetal = false;
    inline bool bDroppedItemStone = false;
    inline bool bDroppedItemCrystal = false;
    inline bool bDroppedItemGem = false;
    inline bool bDroppedItemPearl = false;
    inline bool bDroppedItemSpoiledMeat = false;

    // 宝箱
    inline bool bDrawSupplyDrops = true;
    inline float SupplyDropMaxDistance = 10000.0f;

    // 建筑
    inline bool bDrawStructures = true;
    inline bool bOnlyDrawStructuresEnemy = false;
    inline float StructureMaxDistance = 10000.0f;
    inline float StructureNameColor[4] = { 1.0f, 1.0f, 0.7f, 1.0f };
    inline float StructureOwnerColor[4] = { 0.4f, 1.0f, 1.0f, 1.0f };
    inline float StructureDistanceColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // 建筑 - 队友
    inline bool bTeamDrawStructures = true;
    inline float TeamStructureMaxDistance = 10000.0f;

    // 水源
    inline bool bDrawWater = true;
    inline float WaterMaxCount = 3;
    inline float WaterMaxDistance = 500.0f;
    inline float WaterNameColor[4] = { 0.0f, 0.75f, 1.0f, 1.0f };
    inline float WaterDistanceColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // 全局
    inline bool bESPEnabled = true;

    inline bool bDrawBox = true;
    inline float BoxColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    inline bool bDrawHealthBar = true;
    inline bool bDrawName = true;
    inline float NameColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    inline bool bDrawSpecies = false;
    inline bool bDrawGrowth = true;
    inline bool bDrawTorpor = true;
    inline float TorporColor[4] = { 0.95f, 0.5f, 0.95f, 1.0f };
    inline bool bDrawRagdoll = true;
    inline float RagdollColor[4] = { 1.0f, 0.0f, 0.25f, 0.75f };
    inline bool bDrawDistance = true;
    inline float DistanceColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    inline bool bDrawAimPoints = true;
    inline float AimPointsColor[4] = { 1.0f, 0.0f, 0.0f, 0.5f };
    inline bool bDrawAimSkeleton = true;
    inline float AimSkeletonColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // 生物
    inline bool bDinoDrawBox = true;
    inline float DinoBoxColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    inline bool bDinoDrawHealthBar = true;
    inline bool bDinoDrawName = true;
    inline float DinoNameColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    inline bool bDinoDrawTorpor = true;
    inline float DinoTorporColor[4] = { 0.95f, 0.5f, 0.95f, 1.0f };
    inline bool bDinoDrawRagdoll = true;
    inline float DinoRagdollColor[4] = { 1.0f, 0.0f, 0.25f, 0.75f };
    inline bool bDinoDrawDistance = true;
    inline float DinoDistanceColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    // 队友
    inline bool bESPTeamEnabled = true;

    inline bool bDrawBoxTeam = true;
    inline float BoxColorTeam[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
    inline bool bDrawHealthBarTeam = true;
    inline bool bDrawNameTeam = true;
    inline float NameColorTeam[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
    inline bool bDrawSpeciesTeam = false;
    inline bool bDrawGrowthTeam = true;
    inline bool bDrawTorporTeam = true;
    inline float TorporColorTeam[4] = { 0.0f, 0.9f, 1.0f, 1.0f };
    inline bool bDrawRagdollTeam = true;
    inline float RagdollColorTeam[4] = { 1.0f, 0.0f, 0.25f, 0.75f };
    inline bool bDrawDistanceTeam = true;
    inline float DistanceColorTeam[4] = { 0.0f, 0.5f, 1.0f, 1.0f };

    // 队友 - 生物
    inline bool bTeamDinoDrawBox = true;
    inline float TeamDinoBoxColor[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
    inline bool bTeamDinoDrawHealthBar = true;
    inline bool bTeamDinoDrawName = true;
    inline float TeamDinoNameColor[4] = { 0.0f, 0.5f, 1.0f, 1.0f };
    inline bool bTeamDinoDrawTorpor = true;
    inline float TeamDinoTorporColor[4] = { 0.0f, 0.9f, 1.0f, 1.0f };
    inline bool bTeamDinoDrawRagdoll = true;
    inline float TeamDinoRagdollColor[4] = { 1.0f, 0.0f, 0.25f, 0.75f };
    inline bool bTeamDinoDrawDistance = true;
    inline float TeamDinoDistanceColor[4] = { 0.0f, 0.5f, 1.0f, 1.0f };

    // OOF
    inline bool bEnableOOF = false;
    inline float OOFColor[4] = { 1.0f, 1.0f, 1.0f, 0.75f };
    inline float OOFRadius = 1.00f;
    inline float OOFSize = 12.0f;
    inline float OOFBreathSpeed = 0.1f;
    inline float OOFMinAlpha = 0.25f;
    inline float OOFMaxAlpha = 1.0f;

    //自动换甲
    inline bool bAutomatic = false;
    inline float ArmorRange = 20.0f;

    //灵魂出窍
    inline bool bOutBodyChecked = false;
}