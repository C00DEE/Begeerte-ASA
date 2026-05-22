#pragma once
#define NOMINMAX
#include "../../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../../../external/SDK/SDK_Headers.hpp"
#include "../../ESP/ESP.h"
#include "../../Config/Configs.h"
#include "../../ESP/DrawESP.h"
#include "../../Util/Util.h"
#include "../../Config/ConfigManager.h"
#include "../../Lua/LuaManager.h"
#include "../../Hack/Hack.h"
#include "PostRender.h"
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

namespace g_UEngine {
    namespace UGameViewportClient {
        void PostRender(SDK::UGameViewportClient* rcx, SDK::UCanvas* canvas){
            g_DrawESP::DrawESP(canvas);
            LuaManager::Get().Lua_OnPostRender();
        }
    }
}