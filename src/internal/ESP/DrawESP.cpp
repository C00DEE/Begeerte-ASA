// DrawESP.cpp
#include "../../external/SDK/SDK_Headers.hpp"
#include "ESP.h"
#include "../Config/Configs.h"
#include "DrawESP.h"
#include "../Util/Util.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <memory>
#include <cstdio>
#include <chrono>

namespace {
    // 将 g_Util 中的 ImU32 等价格式转换为 FLinearColor
    SDK::FLinearColor U32ToFLinearColor(uint32_t color) {
        float r = (float)(color & 0xFF) / 255.0f;
        float g = (float)((color >> 8) & 0xFF) / 255.0f;
        float b = (float)((color >> 16) & 0xFF) / 255.0f;
        float a = (float)((color >> 24) & 0xFF) / 255.0f;
        return SDK::FLinearColor{ r, g, b, a };
    }

    SDK::FLinearColor GetHealthColorLinear(float percentage, float* colorMax, float* colorMin) {
        percentage = std::clamp(percentage, 0.0f, 1.0f);

        float r = colorMin[0] + (colorMax[0] - colorMin[0]) * percentage;
        float g = colorMin[1] + (colorMax[1] - colorMin[1]) * percentage;
        float b = colorMin[2] + (colorMax[2] - colorMin[2]) * percentage;
        float a = colorMin[3] + (colorMax[3] - colorMin[3]) * percentage;

        return SDK::FLinearColor{ r, g, b, a };
    }
}

namespace g_DrawESP {
    static constexpr float FADE_IN_TIME = 0.10f;
    static constexpr float FADE_OUT_TIME = 0.20f;

    struct CachedFlag {
        std::string       text;
        SDK::FLinearColor color;
        g_ESP::FlagPos    pos;
    };

    struct CachedBar {
        float                 currentValue;
        float                 maxValue;
        SDK::FLinearColor     color;
        g_ESP::BarPos         pos;
        g_ESP::BarOrientation orientation;
    };

    // -----------------------------------------------------------------------
    // 实体类型枚举，加入 ShooterCharacter(玩家) 和 PrimalDinoCharacter(生物) 分离
    // -----------------------------------------------------------------------
    enum class ActorType : uint8_t {
        Unknown = 0,
        ShooterCharacter,
        PrimalDinoCharacter,
        PrimalCharacter, // Fallback对于其它不可预知的基础生物/NPC
        PhysicsVolume,
        DroppedItem,
        PrimalStructure,
    };

    struct ESPEntry {
        uintptr_t      actorKey = 0;
        ActorType      actorType = ActorType::Unknown;
        SDK::FVector   lastWorldLoc{};
        g_ESP::BoxRect cachedRect;
        std::string    name;

        std::vector<CachedFlag> flags;
        std::vector<CachedBar>  bars;

        SDK::FLinearColor boxColor{ 0,0,0,0 };
        SDK::FLinearColor nameColor{ 0,0,0,0 };
        SDK::FLinearColor distanceColor{ 0,0,0,0 };
        float  configBoxAlpha = 1.0f;
        float  targetAlpha = 0.0f;
        float  alpha = 0.0f;
        float  lastSeenTime = 0.0f;
        bool   aliveThisFrame = false;
        bool   isOOF = false;
        bool   isItem = false;
        float  cachedHP = 0.0f;
        float  cachedMaxHP = 0.0f;
        float  cachedTorpor = 0.0f;
        float  cachedMaxTorpor = 0.0f;

        bool shouldDrawBox = false;
        bool shouldDrawHealthBar = false;
        bool shouldDrawName = false;
        bool shouldDrawDistance = false;
        bool shouldDrawTorpor = false;
    };

    static std::unordered_map<uintptr_t, ESPEntry> s_entries;

    struct WaterCandidate {
        SDK::AActor* actor;
        float           dist;
        SDK::FVector2D  screenPos;
        SDK::FVector    surfaceLoc;
    };

    static std::vector<WaterCandidate> waterCandidates;
    static std::vector<uintptr_t>      s_toErase;

    void DrawESP(SDK::UCanvas* Canvas)
    {
        if (!Canvas) return;

        switch (g_Config::ESPScaleIdx)
        {
        case 0:
            g_Config::ESPScale = 0.75f;
            break;
        case 1:
            g_Config::ESPScale = 1.00f;
            break;
        case 2:
            g_Config::ESPScale = 1.25f;
            break;
        case 3:
            g_Config::ESPScale = 1.5f;
            break;
        case 4:
            g_Config::ESPScale = 2.0f;
            break;
        }

        if (!g_Config::bESPEnabled) {
            s_entries.clear();
            waterCandidates.clear();
            return;
        }

        SDK::UWorld* World = SDK::UWorld::GetWorld();
        if (!World || !World->GameState || !World->PersistentLevel) return;

        SDK::APlayerController* LocalPC = g_Util::GetLocalPC();
        if (!LocalPC || !LocalPC->Pawn) {
            for (auto& kv : s_entries) {
                kv.second.targetAlpha = 0.0f;
                kv.second.aliveThisFrame = false;
            }
            return;
        }

        SDK::APlayerState* LocalPS = LocalPC->PlayerState;
        if (!LocalPS) {
            for (auto& kv : s_entries) {
                kv.second.targetAlpha = 0.0f;
                kv.second.aliveThisFrame = false;
            }
            return;
        }

        const float screenW = Canvas->SizeX;
        const float screenH = Canvas->SizeY;

        static auto s_lastTime = std::chrono::high_resolution_clock::now();
        auto s_currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(s_currentTime - s_lastTime).count();
        s_lastTime = s_currentTime;

        SDK::APrimalCharacter* LocalChar = static_cast<SDK::APrimalCharacter*>(LocalPC->Pawn);

        SDK::TArray<SDK::AActor*>& Actors = World->PersistentLevel->Actors;
        const int actorCount = Actors.Num();

        waterCandidates.clear();

        const char* rawEntityFilter = g_Config::entitySearchBuf;
        const bool  hasEntityFilter = g_Config::bEnableFilter && rawEntityFilter[0] != '\0';
        const char* rawStructFilter = g_Config::structureSearchBuf;
        const bool  hasStructureFilter = g_Config::bEnableStructureFilter && rawStructFilter[0] != '\0';

        for (auto& kv : s_entries)
            kv.second.aliveThisFrame = false;

        for (int i = 0; i < actorCount; i++) {
            SDK::AActor* TargetActor = Actors[i];
            if (!TargetActor || TargetActor == LocalPC->Pawn) continue;

            uintptr_t  key = reinterpret_cast<uintptr_t>(TargetActor);
            ESPEntry& entry = s_entries[key];

            entry.actorKey = key;
            entry.lastSeenTime = 0.0f;

            if (TargetActor->bHidden) {
                entry.targetAlpha = 0.0f;
                entry.aliveThisFrame = false;
                continue;
            }

            // ---- 细化实体类型的判定（玩家与恐龙生物剥离） ----
            if (entry.actorType == ActorType::Unknown) {
                if (TargetActor->IsA(SDK::AShooterCharacter::StaticClass())) entry.actorType = ActorType::ShooterCharacter;
                else if (TargetActor->IsA(SDK::APrimalDinoCharacter::StaticClass())) entry.actorType = ActorType::PrimalDinoCharacter;
                else if (TargetActor->IsA(SDK::APrimalCharacter::StaticClass())) entry.actorType = ActorType::PrimalCharacter;
                else if (TargetActor->IsA(SDK::APhysicsVolume::StaticClass())) entry.actorType = ActorType::PhysicsVolume;
                else if (TargetActor->IsA(SDK::ADroppedItem::StaticClass())) entry.actorType = ActorType::DroppedItem;
                else if (TargetActor->IsA(SDK::APrimalStructure::StaticClass())) entry.actorType = ActorType::PrimalStructure;
            }

            const SDK::FVector actorLoc = TargetActor->K2_GetActorLocation();
            entry.lastWorldLoc = actorLoc;
            entry.aliveThisFrame = true;

            // ============================================================
            // Branch: PrimalCharacter (包含了 ShooterCharacter, PrimalDinoCharacter 以及其它派生)
            // ============================================================
            if (entry.actorType == ActorType::ShooterCharacter || entry.actorType == ActorType::PrimalDinoCharacter || entry.actorType == ActorType::PrimalCharacter) {
                SDK::APrimalCharacter* TargetChar = static_cast<SDK::APrimalCharacter*>(TargetActor);
                SDK::APlayerState* TargetPS = TargetChar->PlayerState;

                const float dist = (LocalPC && LocalPC->Pawn && TargetActor) ? LocalPC->Pawn->GetDistanceTo(TargetActor) * 0.01f : 0.0f;
                const bool isDead = TargetChar->IsDead();

                // 将生物类型判定作为一个布尔值，用于区分全局ESP应用规则
                const bool isDino = (entry.actorType == ActorType::PrimalDinoCharacter);

                if (isDead) {
                    g_ESP::RelationType relation = g_ESP::GetRelation(TargetChar, LocalChar);

                    bool   bShowRagdoll = false;
                    float* RagdollCol = nullptr;
                    float* DistCol = nullptr;

                    if (isDino) {
                        if (relation == g_ESP::RelationType::Team) {
                            bShowRagdoll = g_Config::bTeamDinoDrawRagdoll;
                            RagdollCol = g_Config::TeamDinoRagdollColor;
                            DistCol = g_Config::TeamDinoDistanceColor;
                        }
                        else {
                            bShowRagdoll = g_Config::bDinoDrawRagdoll;
                            RagdollCol = g_Config::DinoRagdollColor;
                            DistCol = g_Config::DinoDistanceColor;
                        }
                    }
                    else {
                        // ShooterCharacter (Player) 逻辑保持
                        if (relation == g_ESP::RelationType::Team) {
                            if (!g_Config::bESPTeamEnabled) {
                                entry.targetAlpha = 0.0f;
                                entry.aliveThisFrame = false;
                                continue;
                            }
                            bShowRagdoll = g_Config::bDrawRagdollTeam;
                            RagdollCol = g_Config::RagdollColorTeam;
                            DistCol = g_Config::RagdollColorTeam;
                        }
                        else {
                            bShowRagdoll = g_Config::bDrawRagdoll;
                            RagdollCol = g_Config::RagdollColor;
                            DistCol = g_Config::RagdollColor;
                        }
                    }

                    if (!bShowRagdoll) {
                        entry.targetAlpha = 0.0f;
                        entry.aliveThisFrame = false;
                        continue;
                    }

                    g_ESP::BoxRect rect = g_ESP::DrawBox(Canvas, TargetActor,
                        RagdollCol[0], RagdollCol[1],
                        RagdollCol[2], RagdollCol[3], 0.5f, true);

                    if (!rect.valid) {
                        entry.targetAlpha = 0.0f;
                        continue;
                    }

                    entry.cachedRect = rect;
                    entry.boxColor = SDK::FLinearColor{ RagdollCol[0], RagdollCol[1], RagdollCol[2], RagdollCol[3] };
                    entry.distanceColor = SDK::FLinearColor{ DistCol[0], DistCol[1], DistCol[2], DistCol[3] };
                    entry.configBoxAlpha = RagdollCol[3];
                    entry.targetAlpha = 1.0f;
                    entry.aliveThisFrame = true;

                    if (isDino) {
                        if (relation == g_ESP::RelationType::Team) {
                            entry.shouldDrawBox = g_Config::bTeamDinoDrawBox;
                            entry.shouldDrawName = g_Config::bTeamDinoDrawName;
                            entry.shouldDrawDistance = g_Config::bTeamDinoDrawDistance;
                        }
                        else {
                            entry.shouldDrawBox = g_Config::bDinoDrawBox;
                            entry.shouldDrawName = g_Config::bDinoDrawName;
                            entry.shouldDrawDistance = g_Config::bDinoDrawDistance;
                        }
                    }
                    else {
                        if (relation == g_ESP::RelationType::Team) {
                            entry.shouldDrawBox = g_Config::bDrawBoxTeam;
                            entry.shouldDrawName = g_Config::bDrawNameTeam;
                            entry.shouldDrawDistance = g_Config::bDrawDistanceTeam;
                        }
                        else {
                            entry.shouldDrawBox = g_Config::bDrawBox;
                            entry.shouldDrawName = g_Config::bDrawName;
                            entry.shouldDrawDistance = g_Config::bDrawDistance;
                        }
                    }

                    entry.shouldDrawHealthBar = false;
                    entry.shouldDrawTorpor = false;
                    entry.shouldDrawDistance = false;
                    entry.flags.clear();
                    entry.bars.clear();

                    if (entry.shouldDrawName) {
                        std::string deadName = TargetPS
                            ? TargetPS->GetPlayerName().ToString()
                            : TargetChar->GetDescriptiveName().ToString();
                        entry.flags.push_back({ std::move(deadName), entry.boxColor, g_ESP::FlagPos::Top });
                    }
                    if (entry.shouldDrawDistance) {
                        entry.flags.push_back({
                            std::to_string((int)dist) + "m",
                            entry.distanceColor,
                            g_ESP::FlagPos::Right
                            });
                    }
                    continue;
                }

                if (hasEntityFilter) {
                    std::string nameForESP = TargetPS
                        ? TargetPS->GetPlayerName().ToString()
                        : TargetChar->GetDescriptiveName().ToString();
                    if (!g_Util::IsEntityMatchMulti(nameForESP, rawEntityFilter)) {
                        entry.targetAlpha = 0.0f;
                        entry.aliveThisFrame = false;
                        continue;
                    }
                }

                g_ESP::RelationType relation = g_ESP::GetRelation(TargetChar, LocalChar);

                bool   bDrawBox = false, bDrawHealthBar = false, bDrawName = false;
                bool   bDrawGrowth = false, bDrawDistance = false, bDrawTorpor = false;
                float* BoxColor = nullptr;
                float* NameColor = nullptr;
                float* DistanceColor = nullptr;
                float* TorporColor = nullptr;

                if (isDino) {
                    if (relation == g_ESP::RelationType::Team) {
                        bDrawBox = g_Config::bTeamDinoDrawBox;
                        BoxColor = g_Config::TeamDinoBoxColor;
                        bDrawHealthBar = g_Config::bTeamDinoDrawHealthBar;
                        bDrawName = g_Config::bTeamDinoDrawName;
                        NameColor = g_Config::TeamDinoNameColor;
                        bDrawDistance = g_Config::bTeamDinoDrawDistance;
                        DistanceColor = g_Config::TeamDinoDistanceColor;
                        bDrawTorpor = g_Config::bTeamDinoDrawTorpor;
                        TorporColor = g_Config::TeamDinoTorporColor;
                        bDrawGrowth = false; // 生物通常不应用玩家类型的Growth特征
                    }
                    else {
                        bDrawBox = g_Config::bDinoDrawBox;
                        BoxColor = g_Config::DinoBoxColor;
                        bDrawHealthBar = g_Config::bDinoDrawHealthBar;
                        bDrawName = g_Config::bDinoDrawName;
                        NameColor = g_Config::DinoNameColor;
                        bDrawDistance = g_Config::bDinoDrawDistance;
                        DistanceColor = g_Config::DinoDistanceColor;
                        bDrawTorpor = g_Config::bDinoDrawTorpor;
                        TorporColor = g_Config::DinoTorporColor;
                        bDrawGrowth = false;
                    }
                }
                else {
                    if (relation == g_ESP::RelationType::Team) {
                        if (!g_Config::bESPTeamEnabled) {
                            entry.targetAlpha = 0.0f;
                            entry.aliveThisFrame = false;
                            continue;
                        }
                        bDrawBox = g_Config::bDrawBoxTeam;
                        BoxColor = g_Config::BoxColorTeam;
                        bDrawHealthBar = g_Config::bDrawHealthBarTeam;
                        bDrawName = g_Config::bDrawNameTeam;
                        NameColor = g_Config::NameColorTeam;
                        bDrawGrowth = g_Config::bDrawGrowthTeam;
                        bDrawDistance = g_Config::bDrawDistanceTeam;
                        DistanceColor = g_Config::DistanceColorTeam;
                        bDrawTorpor = g_Config::bDrawTorporTeam;
                        TorporColor = g_Config::TorporColorTeam;
                    }
                    else {
                        bDrawBox = g_Config::bDrawBox;
                        BoxColor = g_Config::BoxColor;
                        bDrawHealthBar = g_Config::bDrawHealthBar;
                        bDrawName = g_Config::bDrawName;
                        NameColor = g_Config::NameColor;
                        bDrawGrowth = g_Config::bDrawGrowth;
                        bDrawDistance = g_Config::bDrawDistance;
                        DistanceColor = g_Config::DistanceColor;
                        bDrawTorpor = g_Config::bDrawTorpor;
                        TorporColor = g_Config::TorporColor;
                    }
                }

                g_ESP::BoxRect rect = g_ESP::DrawBox(Canvas, TargetActor,
                    BoxColor[0], BoxColor[1],
                    BoxColor[2], BoxColor[3], 0.5f, true);

                entry.cachedRect = rect;
                entry.boxColor = SDK::FLinearColor{ BoxColor[0], BoxColor[1], BoxColor[2], BoxColor[3] };
                entry.nameColor = SDK::FLinearColor{ NameColor[0], NameColor[1], NameColor[2], NameColor[3] };
                entry.configBoxAlpha = BoxColor[3];
                entry.isItem = false;
                entry.isOOF = false;
                entry.cachedHP = TargetChar->GetHealth();
                entry.cachedMaxHP = TargetChar->GetMaxHealth();

                SDK::UPrimalCharacterStatusComponent* StatusComp = TargetChar->GetCharacterStatusComponent();
                if (StatusComp) {
                    entry.cachedTorpor = StatusComp->CurrentStatusValues[(int)SDK::EPrimalCharacterStatusValue::Torpidity];
                    entry.cachedMaxTorpor = StatusComp->MaxStatusValues[(int)SDK::EPrimalCharacterStatusValue::Torpidity];
                }
                else {
                    entry.cachedTorpor = 0.0f;
                    entry.cachedMaxTorpor = 0.0f;
                }

                entry.shouldDrawBox = bDrawBox;
                entry.shouldDrawHealthBar = bDrawHealthBar;
                entry.shouldDrawName = bDrawName;
                entry.shouldDrawDistance = bDrawDistance;
                entry.shouldDrawTorpor = bDrawTorpor;

                if (bDrawName) {
                    const char* genderSuffix = TargetActor->IsFemale() ? "-F" : "-M";
                    entry.name = TargetPS
                        ? TargetPS->GetPlayerName().ToString() + genderSuffix
                        : TargetChar->GetDescriptiveName().ToString() + genderSuffix;
                }
                else {
                    entry.name.clear();
                }

                entry.flags.clear();
                entry.bars.clear();

                if (bDrawName && !entry.name.empty())
                    entry.flags.push_back({ entry.name, entry.nameColor, g_ESP::FlagPos::Top });

                if (bDrawHealthBar) {
                    const float healthPct = (entry.cachedMaxHP > 0.0f) ? (entry.cachedHP / entry.cachedMaxHP) : 0.0f;
                    float* colMax = (relation == g_ESP::RelationType::Team) ? g_Config::TeamHealthColor1 : g_Config::HealthBarColor1;
                    float* colMin = (relation == g_ESP::RelationType::Team) ? g_Config::TeamHealthColor2 : g_Config::HealthBarColor2;

                    const SDK::FLinearColor hpCol = GetHealthColorLinear(healthPct, colMax, colMin);

                    entry.flags.push_back({ std::to_string((int)entry.cachedHP), hpCol, g_ESP::FlagPos::Left });
                    entry.bars.push_back({ entry.cachedHP, entry.cachedMaxHP, hpCol, g_ESP::BarPos::Left, g_ESP::BarOrientation::Vertical });
                }

                if (bDrawTorpor && entry.cachedMaxTorpor > 0.0f) {
                    const SDK::FLinearColor torporCol = SDK::FLinearColor{ TorporColor[0], TorporColor[1], TorporColor[2], TorporColor[3] };
                    entry.flags.push_back({
                        std::to_string((int)entry.cachedTorpor) + "/" + std::to_string((int)entry.cachedMaxTorpor),
                        torporCol, g_ESP::FlagPos::Bottom
                        });
                    entry.bars.push_back({ entry.cachedTorpor, entry.cachedMaxTorpor, torporCol, g_ESP::BarPos::Bottom, g_ESP::BarOrientation::Horizontal });
                }

                if (bDrawDistance)
                    entry.flags.push_back({ std::to_string((int)dist) + "m", SDK::FLinearColor{DistanceColor[0], DistanceColor[1], DistanceColor[2], DistanceColor[3]}, g_ESP::FlagPos::Right });

                SDK::FVector2D screenPos;
                if (LocalPC->ProjectWorldLocationToScreen(actorLoc, &screenPos, false)) {
                    const bool onScreen = screenPos.X > 0 && screenPos.X < screenW
                        && screenPos.Y > 0 && screenPos.Y < screenH;
                    if (onScreen) {
                        entry.targetAlpha = 1.0f;
                        entry.isOOF = false;
                    }
                    else {
                        entry.targetAlpha = 0.0f;
                        entry.aliveThisFrame = false;
                    }
                }
                else {
                    entry.targetAlpha = 0.0f;
                    entry.aliveThisFrame = false;
                }
            }

            // ============================================================
            // Branch: WaterVolume
            // ============================================================
            else if (entry.actorType == ActorType::PhysicsVolume && g_Config::bDrawWater) {
                SDK::APhysicsVolume* PV = static_cast<SDK::APhysicsVolume*>(TargetActor);
                if (!PV->bWaterVolume && !PV->bDynamicWaterVolume) {
                    entry.targetAlpha = 0.0f;
                    entry.aliveThisFrame = false;
                    continue;
                }

                SDK::FVector Origin, Extend;
                TargetActor->GetActorBounds(false, &Origin, &Extend, false);
                const SDK::FVector WaterSurfaceLoc = { Origin.X, Origin.Y, Origin.Z + Extend.Z };
                const float dist = (LocalPC && LocalPC->Pawn && TargetActor) ? LocalPC->Pawn->GetDistanceTo(TargetActor) * 0.01f : 0.0f;

                entry.aliveThisFrame = true;
                entry.targetAlpha = 1.0f;

                waterCandidates.push_back({ TargetActor, dist, SDK::FVector2D{0,0}, WaterSurfaceLoc });
            }

            // ============================================================
            // Branch: DroppedItem
            // ============================================================
            else if (entry.actorType == ActorType::DroppedItem && g_Config::bDrawDroppedItems) {
                SDK::ADroppedItem* DroppedItem = static_cast<SDK::ADroppedItem*>(TargetActor);
                SDK::UPrimalItem* Item = DroppedItem ? DroppedItem->MyItem : nullptr;

                if (!Item) {
                    entry.targetAlpha = 0.0f;
                    entry.aliveThisFrame = false;
                    continue;
                }

                const float dist = (LocalPC && LocalPC->Pawn && TargetActor) ? LocalPC->Pawn->GetDistanceTo(TargetActor) * 0.01f : 0.0f;
                if (dist > g_Config::DroppedItemMaxDistance) {
                    entry.targetAlpha = 0.0f;
                    entry.aliveThisFrame = false;
                    continue;
                }

                // 进行新加入的物品类别过滤拦截
                const std::string className = Item->Class ? Item->Class->GetName() : "";
                const int quantity = Item->ItemQuantity;

                if (!g_Util::IsDroppedItemAllowed(className, quantity)) {
                    entry.targetAlpha = 0.0f;
                    entry.aliveThisFrame = false;
                    continue;
                }

                SDK::FVector2D screenPos;
                bool bIsProjected = LocalPC && LocalPC->ProjectWorldLocationToScreen(actorLoc, &screenPos, false);
                bool bOnScreen = bIsProjected && (screenPos.X > 0 && screenPos.X < screenW && screenPos.Y > 0 && screenPos.Y < screenH);

                if (bIsProjected) {
                    entry.cachedRect.topLeft = SDK::FVector2D{ (float)(screenPos.X - 5), (float)(screenPos.Y - 5) };
                    entry.cachedRect.bottomRight = SDK::FVector2D{ (float)(screenPos.X + 5), (float)(screenPos.Y + 5) };
                    entry.cachedRect.valid = true;
                }

                if (!bOnScreen) {
                    entry.targetAlpha = 0.0f;
                    entry.aliveThisFrame = false;
                }
                else {
                    entry.targetAlpha = 1.0f;
                    entry.aliveThisFrame = true;
                    entry.isItem = true;
                }

                std::string itemName;
                if (Item->CustomItemName.IsValid() && !Item->CustomItemName.ToString().empty()) {
                    itemName = Item->CustomItemName.ToString();
                }
                else if (Item->DescriptiveNameBase.IsValid()) {
                    itemName = Item->DescriptiveNameBase.ToString();
                }
                else {
                    itemName = Item->Class ? Item->Class->GetName() : "Unknown Item";
                }

                const SDK::FLinearColor finalCol = U32ToFLinearColor(g_Util::ResolveDroppedItemColor(className, Item->ItemRating, quantity));

                entry.flags.clear();
                entry.bars.clear();

                std::string label = "[" + itemName + "";
                if (quantity > 1) label += " x" + std::to_string(quantity);
                if (Item->bIsBlueprint) label = "[BP] " + label;

                entry.flags.push_back({ std::move(label) + "] (" + std::to_string((int)dist) + "m" + ")", finalCol, g_ESP::FlagPos::Right });

                entry.boxColor = finalCol;
                entry.nameColor = SDK::FLinearColor{ g_Config::DroppedItemNameColor[0], g_Config::DroppedItemNameColor[1], g_Config::DroppedItemNameColor[2], g_Config::DroppedItemNameColor[3] };
                entry.shouldDrawBox = false;
                entry.shouldDrawHealthBar = false;
                entry.shouldDrawName = false;
                entry.shouldDrawDistance = true;
                entry.shouldDrawTorpor = false;
            }

            // ============================================================
            // Branch: PrimalStructure
            // ============================================================
            else if (entry.actorType == ActorType::PrimalStructure) {
                // 如果两项结构开关都没开，则直接跳过拦截
                if (!g_Config::bDrawStructures && !g_Config::bTeamDrawStructures) {
                    entry.targetAlpha = 0.0f;
                    entry.aliveThisFrame = false;
                    continue;
                }

                SDK::APrimalStructure* Structure = static_cast<SDK::APrimalStructure*>(TargetActor);

                if (Structure->Health <= 0.0f) {
                    entry.targetAlpha = 0.0f;
                    entry.aliveThisFrame = false;
                    continue;
                }

                // 判断是否是队友建筑并独立进行对应的配置检测
                bool isTeam = LocalChar && (LocalChar->TribeName.ToString() == Structure->OwnerName.ToString());
                if (isTeam && !g_Config::bTeamDrawStructures) {
                    entry.targetAlpha = 0.0f;
                    entry.aliveThisFrame = false;
                    continue;
                }
                if (!isTeam && !g_Config::bDrawStructures) {
                    entry.targetAlpha = 0.0f;
                    entry.aliveThisFrame = false;
                    continue;
                }

                if (hasStructureFilter) {
                    std::string sName = Structure->GetDescriptiveName().ToString();
                    if (sName.empty() || sName == "None") sName = "Structure";
                    if (!g_Util::IsStructureMatchMulti(sName, rawStructFilter)) {
                        entry.targetAlpha = 0.0f;
                        entry.aliveThisFrame = false;
                        continue;
                    }
                }

                // 获取并使用不同的距离过滤
                float maxDist = isTeam ? g_Config::TeamStructureMaxDistance : g_Config::StructureMaxDistance;
                const float dist = (LocalPC && LocalPC->Pawn && TargetActor) ? LocalPC->Pawn->GetDistanceTo(TargetActor) * 0.01f : 0.0f;
                if (dist > maxDist) {
                    entry.targetAlpha = 0.0f;
                    entry.aliveThisFrame = false;
                    continue;
                }

                SDK::FVector2D screenPos;
                bool bProjected = LocalPC && LocalPC->ProjectWorldLocationToScreen(actorLoc, &screenPos, false);

                if (bProjected) {
                    entry.cachedRect.topLeft = SDK::FVector2D{ (float)(screenPos.X - 2), (float)(screenPos.Y - 2) };
                    entry.cachedRect.bottomRight = SDK::FVector2D{ (float)(screenPos.X + 2), (float)(screenPos.Y + 2) };
                    entry.cachedRect.valid = true;
                }

                bool bOnScreen = bProjected && (screenPos.X > 0 && screenPos.X < screenW && screenPos.Y > 0 && screenPos.Y < screenH);

                if (bOnScreen) {
                    entry.aliveThisFrame = true;
                    entry.targetAlpha = 1.0f;
                    entry.isItem = true;
                }
                else {
                    entry.targetAlpha = 0.0f;
                    entry.aliveThisFrame = false;
                }

                std::string sName = Structure->GetDescriptiveName().ToString();
                if (sName.empty() || sName == "None") sName = "Structure";

                const float curHP = Structure->Health;
                const float maxHP = Structure->MaxHealth;
                const float healthPct = (maxHP > 0.0f) ? (curHP / maxHP) : 0.0f;
                const int   hpPctInt = (int)(healthPct * 100.0f);

                float* sColMax = isTeam ? g_Config::TeamStructureHealthColor1 : g_Config::StructureHealthColor1;
                float* sColMin = isTeam ? g_Config::TeamStructureHealthColor2 : g_Config::StructureHealthColor2;

                const SDK::FLinearColor hpColor = GetHealthColorLinear(healthPct, sColMax, sColMin);

                std::string owner = Structure->OwnerName.ToString();
                std::string ownerStf = (owner.empty() || owner == "None") ? "" : " [" + owner + "]";

                entry.flags.clear();
                entry.bars.clear();

                entry.flags.push_back({
                    "[" + sName + "]" + std::move(ownerStf) + " [" + std::to_string(hpPctInt) + "%] (" + std::to_string((int)dist) + "m" + ")",
                    hpColor,
                    g_ESP::FlagPos::Right
                    });

                entry.shouldDrawTorpor = false;
            }
            else {
                entry.targetAlpha = 0.0f;
                entry.aliveThisFrame = false;
            }
        } // end actor loop

        for (auto& kv : s_entries) {
            if (!kv.second.aliveThisFrame)
                kv.second.targetAlpha = 0.0f;
        }

        // ----------------------------------------------------------------
        // 水源过滤：只显示最近的 WaterMaxCount 个
        // ----------------------------------------------------------------
        if (g_Config::bDrawWater && !waterCandidates.empty()) {
            std::sort(waterCandidates.begin(), waterCandidates.end(),
                [](const WaterCandidate& a, const WaterCandidate& b) {
                    return a.dist < b.dist;
                });

            for (auto& wc : waterCandidates) {
                uintptr_t key = reinterpret_cast<uintptr_t>(wc.actor);
                if (s_entries.count(key)) {
                    s_entries[key].targetAlpha = 0.0f;
                    s_entries[key].aliveThisFrame = false;
                }
            }

            const int showCount = ((int)waterCandidates.size() < g_Config::WaterMaxCount)
                ? (int)waterCandidates.size()
                : g_Config::WaterMaxCount;

            const SDK::FLinearColor waterColor = SDK::FLinearColor{ g_Config::WaterNameColor[0], g_Config::WaterNameColor[1], g_Config::WaterNameColor[2], g_Config::WaterNameColor[3] };
            const SDK::FLinearColor waterDistColor = SDK::FLinearColor{ g_Config::WaterDistanceColor[0], g_Config::WaterDistanceColor[1], g_Config::WaterDistanceColor[2], g_Config::WaterDistanceColor[3] };

            static const std::string kWaterLabel = SDK::FString(L"[水源").ToString();

            for (int wi = 0; wi < showCount; wi++) {
                const WaterCandidate& wc = waterCandidates[wi];
                uintptr_t             wkey = reinterpret_cast<uintptr_t>(wc.actor);
                ESPEntry& wEntry = s_entries[wkey];

                SDK::FVector2D currentScreenPos;
                if (LocalPC && LocalPC->ProjectWorldLocationToScreen(wc.surfaceLoc, &currentScreenPos, false)) {
                    wEntry.cachedRect.topLeft = SDK::FVector2D{ (float)(currentScreenPos.X - 2), (float)(currentScreenPos.Y - 2) };
                    wEntry.cachedRect.bottomRight = SDK::FVector2D{ (float)(currentScreenPos.X + 2), (float)(currentScreenPos.Y + 2) };
                    wEntry.cachedRect.valid = true;

                    const bool onScreen = currentScreenPos.X > 0 && currentScreenPos.X < screenW
                        && currentScreenPos.Y > 0 && currentScreenPos.Y < screenH;

                    if (onScreen) {
                        wEntry.aliveThisFrame = true;
                        wEntry.targetAlpha = 1.0f;
                        wEntry.isItem = true;

                        wEntry.flags.clear();
                        wEntry.bars.clear();
                        wEntry.flags.push_back({ kWaterLabel + "] (" + std::to_string((int)wc.dist) + "m" + ")", waterColor, g_ESP::FlagPos::Right });

                        wEntry.shouldDrawBox = false;
                        wEntry.shouldDrawHealthBar = false;
                        wEntry.shouldDrawName = false;
                        wEntry.shouldDrawDistance = true;
                        wEntry.shouldDrawTorpor = false;
                    }
                    else {
                        wEntry.targetAlpha = 0.0f;
                        wEntry.aliveThisFrame = false;
                    }
                }
                else {
                    wEntry.targetAlpha = 0.0f;
                    wEntry.aliveThisFrame = false;
                }
            }
        }

        // ----------------------------------------------------------------
        // 渲染 & 淡入淡出 & 清理
        // ----------------------------------------------------------------
        s_toErase.clear();

        for (auto& kv : s_entries) {
            ESPEntry& entry = kv.second;

            const float fadeTime = (entry.targetAlpha > entry.alpha) ? FADE_IN_TIME : FADE_OUT_TIME;
            entry.alpha = g_Util::ApproachAlpha(entry.alpha, entry.targetAlpha, deltaTime, fadeTime);

            if (entry.alpha <= 0.001f && entry.targetAlpha <= 0.001f && !entry.aliveThisFrame) {
                s_toErase.push_back(kv.first);
                continue;
            }

            if (entry.alpha > 0.001f) {
                if (!entry.isItem && entry.shouldDrawBox && entry.cachedRect.valid) {
                    float boxConfigAlpha = entry.configBoxAlpha;
                    SDK::FLinearColor boxCol = entry.boxColor;

                    g_ESP::DrawBox(Canvas, entry.cachedRect, boxCol, entry.alpha);
                }

                g_ESP::BarManager bm;
                bm.Reset();
                for (const auto& bar : entry.bars)
                    bm.AddBar(Canvas, entry.cachedRect, bar.currentValue, bar.maxValue, bar.color, bar.pos, bar.orientation, entry.alpha);

                g_ESP::FlagManager fm;
                fm.Reset();
                for (const auto& f : entry.flags)
                    fm.AddFlag(Canvas, entry.cachedRect, f.text, f.color, f.pos, entry.alpha, &bm);
            }
        }

        for (uintptr_t k : s_toErase)
            s_entries.erase(k);
    }
} // namespace g_DrawESP