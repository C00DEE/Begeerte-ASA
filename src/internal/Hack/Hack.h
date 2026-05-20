#pragma once
#include "../../external/SDK/SDK_Headers.hpp"

namespace g_Hack {
    void DumpServerInfo();
    void Suicide(SDK::UWorld* World);
    void UnlockExplorerNotes(SDK::UWorld* World);
    void AutoFeed(SDK::UWorld* World);
    void SuperFlyer(SDK::UWorld* World);
    void ForceTurn(SDK::UMovementComponent* rcx, float DeltaTime);
    void DamageLog(SDK::AActor* _this, float DamageAmount, SDK::FDamageEvent* DamageEvent, SDK::AController* Instigator, SDK::AActor* DamageCauser);
    void AutoSwapBrokenEquipment();
    void OutBody();
}