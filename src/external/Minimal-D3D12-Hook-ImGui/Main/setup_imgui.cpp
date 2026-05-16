#include "mdx12_api.h"

namespace g_MDX12 {
    void SetSetupPostDamageCallback(SetupPostDamageCallback callback) {
        g_Callbacks::g_setupPostDamageCallback = callback;
    }

    void SetupPostDamage(SDK::AActor* _this, float DamageAmount, SDK::FDamageEvent* DamageEvent, SDK::AController* Instigator, SDK::AActor* DamageCauser) {
        if (g_Callbacks::g_setupPostDamageCallback) {
            g_Callbacks::g_setupPostDamageCallback(_this, DamageAmount, DamageEvent, Instigator, DamageCauser);
        }
    }

    void SetSetupTakeDamageCallback(SetupTakeDamageCallback callback) {
        g_Callbacks::g_setupTakeDamageCallback = callback;
    }

    void SetupTakeDamage(SDK::AActor* _this, float DamageAmount, SDK::FDamageEvent* DamageEvent, SDK::AController* Instigator, SDK::AActor* DamageCauser) {
        if (g_Callbacks::g_setupTakeDamageCallback) {
            g_Callbacks::g_setupTakeDamageCallback(_this, DamageAmount, DamageEvent, Instigator, DamageCauser);
        }
    }

    void SetSetupPhysicsRotationCallback(SetupPhysicsRotationCallback callback) {
        g_Callbacks::g_setupPhysicsRotationCallback = callback;
    }

    void SetupPhysicsRotation(SDK::UMovementComponent* rcx, float DeltaTime) {
        if (g_Callbacks::g_setupPhysicsRotationCallback) {
            g_Callbacks::g_setupPhysicsRotationCallback(rcx, DeltaTime);
        }
    }

    void SetSetupPostRenderCallback(SetupPostRenderCallback callback) {
        g_Callbacks::g_setupPostRenderCallback = callback;
    }

    void SetupPostRender(SDK::UGameViewportClient* rcx, SDK::UCanvas* canvas) {
        if (g_Callbacks::g_setupPostRenderCallback) {
            g_Callbacks::g_setupPostRenderCallback(rcx, canvas);
        }
    }

    void SetSetupOutputTextLineCallback(SetupOutputTextLineCallback callback) {
        g_Callbacks::g_setupOutputTextLineCallback = callback;
    }

    void SetupOutputTextLine(SDK::UConsole* rcx, SDK::FString* Message) {
        if (g_Callbacks::g_setupOutputTextLineCallback) {
            g_Callbacks::g_setupOutputTextLineCallback(rcx, Message);
        }
    }

    void SetSetupUWorldTickCallback(SetupUWorldTickCallback callback) {
        g_Callbacks::g_setupUWorldTickCallback = callback;
    }

    void SetupUWorldTick(SDK::UWorld* rcx) {
        if (g_Callbacks::g_setupUWorldTickCallback) {
            g_Callbacks::g_setupUWorldTickCallback(rcx);
        }
    }

    void SetSetupHandleDisconnectCallback(SetupHandleDisconnectCallback callback) {
        g_Callbacks::g_setupHandleDisconnectCallback = callback;
    }

    void SetupHandleDisconnect(SDK::UNetConnection* rcx) {
        if (g_Callbacks::g_setupHandleDisconnectCallback) {
            g_Callbacks::g_setupHandleDisconnectCallback(rcx);
        }
    }

    void SetSetupImGuiCallback(SetupImGuiCallback callback) {
        g_Callbacks::g_setupImGuiCallback = callback;
    }

    void SetupImGui(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags) {
        // Call user-defined callback if set
        if (g_Callbacks::g_setupImGuiCallback) {
            g_Callbacks::g_setupImGuiCallback(pSwapChain, SyncInterval, Flags);
        }
    }
}