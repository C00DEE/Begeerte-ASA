#pragma once
#define NOMINMAX
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../Config/Configs.h"
#include "../Util/Util.h"
#include "../Lua/LuaManager.h"
#include "../Hack/Hack.h"
#include "../../external/SDK/SDK_Headers.hpp"
#include "../Log/LogManager.h"

namespace g_AActor {
	void TakeDamage(SDK::AActor* _this, float DamageAmount, SDK::FDamageEvent* DamageEvent, SDK::AController* Instigator, SDK::AActor* DamageCauser) {
        if (!_this || !DamageEvent || !Instigator || !DamageCauser) return;

		if (g_Config::bLogDamage) g_Hack::DamageLog(_this, DamageAmount, DamageEvent, Instigator, DamageCauser);
	}
}