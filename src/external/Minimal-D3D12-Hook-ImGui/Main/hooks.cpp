#include "mdx12_api.h"
#include "hooks.h"
#include "../../AOBScan/AOBScan.hpp"
#include "../../../internal/CheatData/CheatData.hpp"
#include "../../../internal/Config/Configs.h"
// #include "../Font/Alibaba-PuHuiTi-Bold.h"
// #include "../Font/Alibaba-PuHuiTi-Heavy.h"
// #include "../Font/Alibaba-PuHuiTi-Light.h"
// #include "../Font/Alibaba-PuHuiTi-Medium.h"
// #include "../Font/Alibaba-PuHuiTi-Regular.h"
#include "../Font/HarmonyOS_Sans_SC_Regular.h"

#include "../MinHook/include/MinHook.h"

#include "../../../internal/Util/Util.h"
#include "../../../internal/Log/LogManager.h"

#include "../../../internal/Menu/ConfigImGui.h"

#pragma warning(push)
#pragma warning(disable: 26451)
#pragma warning(disable: 26812)
#pragma warning(disable: 6387)
#pragma warning(pop)

namespace g_Hook {
    typedef void* (__fastcall* tWorldFunction)(SDK::UWorld* rcx, void* rdx, void* r8, void* r9);
    tWorldFunction oWorldTick = nullptr;

    typedef void* (__fastcall* tHandleDisconnectFunction)(SDK::UNetConnection* rcx, void* rdx, void* r8, void* r9);
    tHandleDisconnectFunction oHandleDisconnect = nullptr;

    typedef void(__fastcall* tOutputTextLine)(SDK::UConsole* Console, SDK::FString* Message, void* r8, void* r9);
    tOutputTextLine oOutputTextLine = nullptr;

    typedef void(__fastcall* tPostRender)(SDK::UGameViewportClient* rcx, SDK::UCanvas* canvas, void* r8, void* r9);
    tPostRender oPostRender = nullptr;

    typedef void(__fastcall* tPhysicsRotation)(SDK::UMovementComponent* rcx, float DeltaTime);
    tPhysicsRotation oPhysicsRotation = nullptr;

    typedef float(__fastcall* tTakeDamage)(
        SDK::AActor* _this,             // RCX
        float DamageAmount,             // XMM1
        SDK::FDamageEvent* DamageEvent, // R8
        SDK::AController* Instigator,   // R9
        SDK::AActor* DamageCauser       // 栈上
        );

    tTakeDamage oTakeDamage = nullptr;

    void* __fastcall hkUWorldTick(SDK::UWorld* rcx, void* rdx, void* r8, void* r9) {
        g_MDX12::SetupUWorldTick(rcx);

        return oWorldTick(rcx, rdx, r8, r9);
    }

    void* __fastcall hkHandleDisconnect(SDK::UNetConnection* rcx, void* rdx, void* r8, void* r9) {
        g_MDX12::SetupHandleDisconnect(rcx);

        return oHandleDisconnect(rcx, rdx, r8, r9);
    }

    void __fastcall hkOutputTextLine(SDK::UConsole* rcx, SDK::FString* Message, void* r8, void* r9) {
        g_MDX12::SetupOutputTextLine(rcx, Message);

        return oOutputTextLine(rcx, Message, r8, r9);

        // 应该不会返回空指针 
        /*
        if (rcx && Message) {
            g_MDX12::SetupOutputTextLine(rcx, Message);
        }

        if (oOutputTextLine) {
            oOutputTextLine(rcx, Message, r8, r9);
        }
        */
    }

    void __fastcall hkPostRender(SDK::UGameViewportClient* rcx, SDK::UCanvas* canvas, void* r8, void* r9) {
        g_MDX12::SetupPostRender(rcx, canvas);

        return oPostRender(rcx, canvas, r8, r9);

        /*
        if (rcx && canvas) {
            g_MDX12::SetupPostRender(rcx, canvas);
        }
        */
    }

    void __fastcall hkPhysicsRotation(SDK::UMovementComponent* rcx, float DeltaTime)
    {
        g_MDX12::SetupPhysicsRotation(rcx, DeltaTime);

        return oPhysicsRotation(rcx, DeltaTime);
    }

    float __fastcall hkTakeDamage(
        SDK::AActor* _this,
        float DamageAmount,
        SDK::FDamageEvent* DamageEvent,
        SDK::AController* Instigator,
        SDK::AActor* DamageCauser)
    {
        g_MDX12::SetupTakeDamage(_this, DamageAmount, DamageEvent, Instigator, DamageCauser);

        return oTakeDamage(_this, DamageAmount, DamageEvent, Instigator, DamageCauser);
    }

    // 2026/3/29 @zetsr
    // 不再需要这种方法了，因为UWorld的虚表无法在游戏更新后稳定
    /*
    void initUWorldTick() {
        SDK::UWorld* pWorld = nullptr;
        while (!pWorld) {
            pWorld = SDK::UWorld::GetWorld();
            if (pWorld && pWorld->OwningGameInstance) break;
            Sleep(1);
        }

        void** vtable = *reinterpret_cast<void***>(pWorld);
        void* pTarget = vtable[336]; // 220

        if (pTarget) {
            if (MH_CreateHook(pTarget, &g_Hook::hkUWorldTick, reinterpret_cast<LPVOID*>(&g_Hook::oWorldTick)) == MH_OK) {
                // MH_EnableHook(pTarget);
            }
        }
    }
    */

    void initUWorldTick() {
        std::string pattern = g_CheatData::Signature::UWorld::Tick;
        AOB::Result ok = AOB::Scan(pattern);

        if (ok && ok.size() > 0) {
            void* targetAddr = ok[0];

            if (MH_CreateHook(targetAddr, &hkUWorldTick, reinterpret_cast<LPVOID*>(&oWorldTick)) == MH_OK) {
                MH_EnableHook(targetAddr);
                UWorldTickOK = true;
            }
        }
    }

    void initHandleDisconnect() {
        std::string pattern = g_CheatData::Signature::UWorld::UNetDriver::UNetConnection::HandleDisconnect;
        AOB::Result ok = AOB::Scan(pattern);

        if (ok && ok.size() > 0) {
            void* targetAddr = ok[0];

            if (MH_CreateHook(targetAddr, &hkHandleDisconnect, reinterpret_cast<LPVOID*>(&oHandleDisconnect)) == MH_OK) {
                MH_EnableHook(targetAddr);
				HandleDisconnectOK = true;
            }
        }
    }

    void initOutputTextLine() {
        std::string pattern = g_CheatData::Signature::UEngine::UGameViewportClient::UConsole::OutputTextLine;
        AOB::Result ok = AOB::Scan(pattern);

        if (ok && ok.size() > 0) {
            void* targetAddr = ok[0];

            if (MH_CreateHook(targetAddr, &hkOutputTextLine, reinterpret_cast<LPVOID*>(&oOutputTextLine)) == MH_OK) {
                MH_EnableHook(targetAddr);
				OutputTextLineOK = true;
            }
        }
    }

    void initPostRender() {
        std::string pattern = g_CheatData::Signature::UEngine::UGameViewportClient::PostRender;
        AOB::Result ok = AOB::Scan(pattern);

        if (ok && ok.size() > 0) {
            void* targetAddr = ok[0];

            if (MH_CreateHook(targetAddr, &hkPostRender, reinterpret_cast<LPVOID*>(&oPostRender)) == MH_OK) {
                MH_EnableHook(targetAddr);
                PostRenderOK = true;
            }
        }
    }

    void initPhysicsRotation() {
        std::string pattern = g_CheatData::Signature::UWorld::PhysicsRotation;
        AOB::Result ok = AOB::Scan(pattern);

        if (ok && ok.size() > 0) {
            void* targetAddr = ok[0];

            if (MH_CreateHook(targetAddr, &hkPhysicsRotation, reinterpret_cast<LPVOID*>(&oPhysicsRotation)) == MH_OK) {
                MH_EnableHook(targetAddr);
				PhysicsRotationOK = true;
            }
        }
    }

    void initTakeDamage() {
        std::string pattern = g_CheatData::Signature::AActor::TakeDamage;
        AOB::Result ok = AOB::Scan(pattern);

        if (ok && ok.size() > 0) {
            void* targetAddr = ok[0];

            if (MH_CreateHook(targetAddr, &hkTakeDamage, reinterpret_cast<LPVOID*>(&oTakeDamage)) == MH_OK) {
                MH_EnableHook(targetAddr);
                TakeDamageOK = true;
            }
        }
    }
}

// 应该不需要每个都单独检查一次
void g_Hook::StartAllHooks() {
    SDK::UWorld* pWorld = nullptr;
    while (!pWorld) {
        pWorld = SDK::UWorld::GetWorld();
        if (pWorld && pWorld->OwningGameInstance) break;
        Sleep(1);
    }

    if (pWorld) {
        // 理论上来说如果用户在UWorld非常不稳定的时候注入有极低概率发生检查后使用，但是这应该不是问题，因为如果环境不稳定，d3d12 hook会先崩溃
        g_Hook::initUWorldTick();
        g_Hook::initHandleDisconnect();
        g_Hook::initOutputTextLine();
        g_Hook::initPostRender();
        g_Hook::initPhysicsRotation();
        g_Hook::initTakeDamage();
    }
}

void g_Hook::StopAllHooks() {
    g_MDX12::g_MenuState::g_isOpen = false;
    g_MDX12::g_InputState::g_blockMouseInput = false;
    g_MDX12::g_InputState::g_blockKeyboardInput = false;

    while (ShowCursor(TRUE) < 0);
    ClipCursor(nullptr);

    if (g_MDX12::g_ProcessWindow::g_mainWindow != nullptr) {
        g_MDX12::inputhook::Remove(g_MDX12::g_ProcessWindow::g_mainWindow);
    }

    MH_DisableHook(MH_ALL_HOOKS);
}

namespace g_MDX12 {
    static DXGI_FORMAT g_LastRTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    void UpdateImGuiFont() {
        // 如果缩放比例没有发生改变，直接返回
        if (g_LoadedFontScaleIdx == g_Config::MenuScaleIdx) return;

        if (g_LoadedFontScaleIdx != -1) {
            // 等待 GPU 把前一帧画完
            if (g_D3D12Resources::g_fence) {
                for (UINT i = 0; i < g_D3D12Resources::g_bufferCount; ++i) {
                    UINT64 fenceVal = g_D3D12Resources::g_FrameContexts[i].FenceValue;
                    if (fenceVal != 0 && g_D3D12Resources::g_fence->GetCompletedValue() < fenceVal) {
                        g_D3D12Resources::g_fence->SetEventOnCompletion(fenceVal, g_D3D12Resources::g_fenceEvent);
                        WaitForSingleObject(g_D3D12Resources::g_fenceEvent, g_InitState::g_waitTimeoutMs);
                    }
                }
            }

            ImGui_ImplDX12_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
            ImGui::CreateContext();
            ImGui_ImplWin32_Init(g_ProcessWindow::g_mainWindow);
            g_DrawImGui::SetupCustomImGuiStyle();
        }

        ImGuiIO& io = ImGui::GetIO();
        const ImWchar* range = io.Fonts->GetGlyphRangesChineseFull();

        // 75%, 100%, 125%, 150%, 200% 对应的字体大小
        float fontSizes[] = { 14.0f, 18.0f, 22.0f, 26.0f, 34.0f };
        int safeIdx = g_Config::MenuScaleIdx;

        ImFontConfig font_cfg;
        font_cfg.FontDataOwnedByAtlas = false;

        g_HarmonyOS_Sans_SC_Regular = io.Fonts->AddFontFromMemoryTTF(
            (void*)g_Fonts::HarmonyOS_Sans_SC_Regular,
            sizeof(g_Fonts::HarmonyOS_Sans_SC_Regular),
            fontSizes[safeIdx],
            &font_cfg,
            range
        );

        io.Fonts->Build(); // 建立字体数据结构

        // 如果是运行时切换(非首次)，立即重建 DX12 对象
        if (g_LoadedFontScaleIdx != -1) {
            ImGui_ImplDX12_Init(g_D3D12Resources::g_pd3dDevice, g_D3D12Resources::g_bufferCount, g_LastRTVFormat, g_D3D12Resources::g_pd3dSrvDescHeap, g_D3D12Resources::g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), g_D3D12Resources::g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
        }

        g_LoadedFontScaleIdx = safeIdx; // 更新记录
    }

    void STDMETHODCALLTYPE hkExecuteCommandLists(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists) {
        if (!g_D3D12Resources::g_pd3dCommandQueue && g_InitState::g_AfterFirstPresent && queue) {
            D3D12_COMMAND_QUEUE_DESC desc = queue->GetDesc();

            if (desc.Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
                ID3D12Device* tempDevice = nullptr;

                if (SUCCEEDED(queue->GetDevice(IID_PPV_ARGS(&tempDevice)))) {
                    tempDevice->Release();
                    queue->AddRef();
                    g_D3D12Resources::g_pd3dCommandQueue = queue;
                }
            }
        }

        if (g_HookFunctions::g_oExecuteCommandLists) {
            g_HookFunctions::g_oExecuteCommandLists(queue, NumCommandLists, ppCommandLists);
        }
    }

    HRESULT STDMETHODCALLTYPE hkPresent(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags) {
        g_InitState::g_AfterFirstPresent = true;

        if (!g_D3D12Resources::g_pd3dCommandQueue) {
            if (g_HookFunctions::g_oPresent) return g_HookFunctions::g_oPresent(pSwapChain, SyncInterval, Flags);
            return S_OK;
        }

        // 不需要持续使用锁
        // std::lock_guard<std::mutex> lock(g_InitState::g_InitMutex);

        if (!g_InitState::g_Initialized) {
            std::lock_guard<std::mutex> lock(g_InitState::g_InitMutex);
            ID3D12Device* deviceFromSwap = nullptr;

            if (FAILED(pSwapChain->GetDevice(IID_PPV_ARGS(&deviceFromSwap)))) {
                if (g_HookFunctions::g_oPresent) return g_HookFunctions::g_oPresent(pSwapChain, SyncInterval, Flags);
                return S_OK;
            }

            if (!g_D3D12Resources::g_pd3dDevice) g_D3D12Resources::g_pd3dDevice = deviceFromSwap;
            else deviceFromSwap->Release();

            DXGI_SWAP_CHAIN_DESC desc{};
            pSwapChain->GetDesc(&desc);
            g_D3D12Resources::g_bufferCount = desc.BufferCount;
            HWND newWindow = desc.OutputWindow;
            GetWindowRect(newWindow, &g_ProcessWindow::g_windowRect);

            if (g_ProcessWindow::g_mainWindow != newWindow) {
                if (g_ProcessWindow::g_mainWindow) {
                    inputhook::Remove(g_ProcessWindow::g_mainWindow);
                }

                g_ProcessWindow::g_mainWindow = newWindow;
                inputhook::Init(g_ProcessWindow::g_mainWindow);
            }
            else if (!g_ProcessWindow::g_mainWindow) {
                g_ProcessWindow::g_mainWindow = newWindow;
                inputhook::Init(g_ProcessWindow::g_mainWindow);
            }

            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
            rtvHeapDesc.NumDescriptors = g_D3D12Resources::g_bufferCount;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            if (FAILED(g_D3D12Resources::g_pd3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_D3D12Resources::g_pd3dRtvDescHeap)))) {
                if (g_HookFunctions::g_oPresent) return g_HookFunctions::g_oPresent(pSwapChain, SyncInterval, Flags);
                return S_OK;
            }

            D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
            srvHeapDesc.NumDescriptors = 1;
            srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

            if (FAILED(g_D3D12Resources::g_pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&g_D3D12Resources::g_pd3dSrvDescHeap)))) {
                if (g_HookFunctions::g_oPresent) return g_HookFunctions::g_oPresent(pSwapChain, SyncInterval, Flags);
                return S_OK;
            }

            g_D3D12Resources::g_FrameContexts.resize(g_D3D12Resources::g_bufferCount);
            UINT rtvIncrementSize = g_D3D12Resources::g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_D3D12Resources::g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();

            for (UINT i = 0; i < g_D3D12Resources::g_bufferCount; ++i) {
                if (FAILED(g_D3D12Resources::g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_D3D12Resources::g_FrameContexts[i].CommandAllocator)))) {
                    if (g_HookFunctions::g_oPresent) return g_HookFunctions::g_oPresent(pSwapChain, SyncInterval, Flags);
                    return S_OK;
                }

                if (FAILED(pSwapChain->GetBuffer(i, IID_PPV_ARGS(&g_D3D12Resources::g_FrameContexts[i].Resource)))) {
                    if (g_HookFunctions::g_oPresent) return g_HookFunctions::g_oPresent(pSwapChain, SyncInterval, Flags);
                    return S_OK;
                }

                g_D3D12Resources::g_FrameContexts[i].Descriptor = rtvHandle;
                g_D3D12Resources::g_pd3dDevice->CreateRenderTargetView(g_D3D12Resources::g_FrameContexts[i].Resource, nullptr, rtvHandle);
                rtvHandle.ptr += static_cast<SIZE_T>(rtvIncrementSize);
            }

            if (FAILED(g_D3D12Resources::g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_D3D12Resources::g_FrameContexts[0].CommandAllocator, nullptr, IID_PPV_ARGS(&g_D3D12Resources::g_pd3dCommandList)))) {
                if (g_HookFunctions::g_oPresent) return g_HookFunctions::g_oPresent(pSwapChain, SyncInterval, Flags);
                return S_OK;
            }

            g_D3D12Resources::g_pd3dCommandList->Close();

            if (FAILED(g_D3D12Resources::g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_D3D12Resources::g_fence)))) {
                if (g_HookFunctions::g_oPresent) return g_HookFunctions::g_oPresent(pSwapChain, SyncInterval, Flags);
                return S_OK;
            }

            g_D3D12Resources::g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

            if (!g_D3D12Resources::g_fenceEvent) {
                if (g_HookFunctions::g_oPresent) return g_HookFunctions::g_oPresent(pSwapChain, SyncInterval, Flags);
                return S_OK;
            }

            // ---------------------------------------------------------
            // [FIX BEGIN] 修复 ImGui 风格在 Resize 时被重置的问题
            // 逻辑：只有当 ImGui 上下文不存在时（第一次运行），才创建上下文和应用默认风格。
            // ---------------------------------------------------------
            if (!ImGui::GetCurrentContext()) {
                ImGui::CreateContext();

                // 下面这行代码就是罪魁祸首，只有首次初始化才跑它
                // 不应该调用它
                // ImGui::StyleColorsDark();

                g_DrawImGui::SetupCustomImGuiStyle();

                ImGui_ImplWin32_Init(g_ProcessWindow::g_mainWindow);
            }
            // ---------------------------------------------------------
            // [FIX END]
            // ---------------------------------------------------------

            ImGuiIO& io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

            ImFontAtlas* atlas = io.Fonts;
            const ImWchar* range = atlas->GetGlyphRangesChineseFull();

            // 默认字体
            // Alibaba-PuHuiTi-Regular
            // g_MDX12::g_Alibaba_PuHuiTi_Regular = io.Fonts->AddFontFromMemoryTTF(g_Fonts::Alibaba_PuHuiTi_Regular, sizeof(g_Fonts::Alibaba_PuHuiTi_Regular), 18.0f, NULL, range);

            // Alibaba-PuHuiTi-Bold
            // g_MDX12::g_Alibaba_PuHuiTi_Bold = io.Fonts->AddFontFromMemoryTTF(g_Fonts::Alibaba_PuHuiTi_Bold, sizeof(g_Fonts::Alibaba_PuHuiTi_Bold), 18.0f, NULL, range);
            
            // Alibaba-PuHuiTi-Heavy
            // g_MDX12::g_Alibaba_PuHuiTi_Heavy = io.Fonts->AddFontFromMemoryTTF(g_Fonts::Alibaba_PuHuiTi_Heavy, sizeof(g_Fonts::Alibaba_PuHuiTi_Heavy), 18.0f, NULL, range);
            
            // Alibaba-PuHuiTi-Light
            // g_MDX12::g_Alibaba_PuHuiTi_Light = io.Fonts->AddFontFromMemoryTTF(g_Fonts::Alibaba_PuHuiTi_Light, sizeof(g_Fonts::Alibaba_PuHuiTi_Light), 18.0f, NULL, range);
            
            // Alibaba-PuHuiTi-Medium
            // g_MDX12::g_Alibaba_PuHuiTi_Medium = io.Fonts->AddFontFromMemoryTTF(g_Fonts::Alibaba_PuHuiTi_Medium, sizeof(g_Fonts::Alibaba_PuHuiTi_Medium), 18.0f, NULL, range);
            
            // HarmonyOS_Sans_SC_Regular
            // g_MDX12::g_HarmonyOS_Sans_SC_Regular = io.Fonts->AddFontFromMemoryTTF(g_Fonts::HarmonyOS_Sans_SC_Regular, sizeof(g_Fonts::HarmonyOS_Sans_SC_Regular), 10.0f, NULL, range);

            // 记录 RTV Format，以供后续运行时切换字体时重启 DX12 后端使用
            g_LastRTVFormat = desc.BufferDesc.Format;

            // DPI切换
            UpdateImGuiFont();

            // DX12 后端必须重新初始化，因为 resize 可能会让之前的 backend 对象失效
            ImGui_ImplDX12_Init(g_D3D12Resources::g_pd3dDevice, g_D3D12Resources::g_bufferCount, desc.BufferDesc.Format, g_D3D12Resources::g_pd3dSrvDescHeap, g_D3D12Resources::g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), g_D3D12Resources::g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

            // unsigned char* pixels;
            // int width, height;

            // io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
            rawinputhook::Init();
            cursorhook::Init();

            g_InitState::g_Initialized = true;
        }

        UINT backBufferIdx = pSwapChain->GetCurrentBackBufferIndex();
        g_D3D12Resources::FrameContext& frameCtx = g_D3D12Resources::g_FrameContexts[backBufferIdx];

        if (g_D3D12Resources::g_fence && frameCtx.FenceValue != 0) {
            UINT64 completed = g_D3D12Resources::g_fence->GetCompletedValue();

            if (completed < frameCtx.FenceValue) {
                g_D3D12Resources::g_fence->SetEventOnCompletion(frameCtx.FenceValue, g_D3D12Resources::g_fenceEvent);
                WaitForSingleObject(g_D3D12Resources::g_fenceEvent, g_InitState::g_waitTimeoutMs);
            }
        }

        frameCtx.CommandAllocator->Reset();
        g_D3D12Resources::g_pd3dCommandList->Reset(frameCtx.CommandAllocator, nullptr);

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = frameCtx.Resource;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        g_D3D12Resources::g_pd3dCommandList->ResourceBarrier(1, &barrier);
        g_D3D12Resources::g_pd3dCommandList->OMSetRenderTargets(1, &frameCtx.Descriptor, FALSE, nullptr);

        ID3D12DescriptorHeap* ppHeaps[] = { g_D3D12Resources::g_pd3dSrvDescHeap };
        g_D3D12Resources::g_pd3dCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

        UpdateImGuiFont();
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGuiIO& io = ImGui::GetIO();
        bool cursorStateChanged = (g_MenuState::g_isOpen != g_MenuState::g_wasOpenLastFrame);

        g_MenuState::g_wasOpenLastFrame = g_MenuState::g_isOpen;
        inputhook::UpdateInputBlockState();
        cursorhook::UpdateCursorState();

        if (g_MenuState::g_isOpen) {
            io.MouseDrawCursor = true;
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
        }
        else {
            io.MouseDrawCursor = false;
            io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        }

        if (cursorStateChanged) {
            if (g_MenuState::g_isOpen) {
                while (ShowCursor(FALSE) >= 0);
                SetCursor(nullptr);
                GetCursorPos(&g_MenuState::g_lastMousePos);
                RECT rect;
                GetWindowRect(g_ProcessWindow::g_mainWindow, &rect);
                int centerX = rect.left + (rect.right - rect.left) / 2;
                int centerY = rect.top + (rect.bottom - rect.top) / 2;
                SetCursorPos(centerX, centerY);
            }
        }

        // 2026/4/1 @zetsr
        // 当时以为Windows的光标会和imgui的打架所以加的这个功能
        // 实际上是一个愚蠢的操作
        /*
        if (g_MenuState::g_isOpen) {
            static int frameCounter = 0;
            frameCounter++;

            if (frameCounter % 30 == 0) {
                RECT rect;
                GetWindowRect(g_ProcessWindow::g_mainWindow, &rect);
                int centerX = rect.left + (rect.right - rect.left) / 2;
                int centerY = rect.top + (rect.bottom - rect.top) / 2;
                SetCursorPos(centerX, centerY);
            }
        }
         */

        SetupImGui(pSwapChain, SyncInterval, Flags);
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_D3D12Resources::g_pd3dCommandList);

        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        g_D3D12Resources::g_pd3dCommandList->ResourceBarrier(1, &barrier);
        g_D3D12Resources::g_pd3dCommandList->Close();

        if (g_D3D12Resources::g_pd3dCommandQueue) {
            if (g_HookFunctions::g_oExecuteCommandLists) {
                g_HookFunctions::g_oExecuteCommandLists(g_D3D12Resources::g_pd3dCommandQueue, 1, (ID3D12CommandList* const*)&g_D3D12Resources::g_pd3dCommandList);
            }

            UINT64 frameFence = ++g_D3D12Resources::g_fenceValue;
            frameCtx.FenceValue = frameFence;
            g_D3D12Resources::g_pd3dCommandQueue->Signal(g_D3D12Resources::g_fence, frameFence);
        }

        if (g_HookFunctions::g_oPresent) return g_HookFunctions::g_oPresent(pSwapChain, SyncInterval, Flags);

        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE hkResizeBuffers(
        IDXGISwapChain* pSwapChain,
        UINT BufferCount,
        UINT Width,
        UINT Height,
        DXGI_FORMAT NewFormat,
        UINT SwapChainFlags)
    {
        // 不需要持续使用锁
        // std::lock_guard<std::mutex> lock(g_InitState::g_InitMutex);

        if (!g_InitState::g_Initialized) {
            std::lock_guard<std::mutex> lock(g_InitState::g_InitMutex);
        }

        if (g_InitState::g_Initialized) {
            CleanupRenderResources_NoInput();
        }

        HRESULT hr = g_HookFunctions::g_oResizeBuffers ? g_HookFunctions::g_oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags) : S_OK;

        g_InputState::g_blockMouseInput = false;
        g_InputState::g_blockKeyboardInput = false;

        return hr;
    }

    DWORD WINAPI MainThread(LPVOID lpParam) {
        if (MH_Initialize() != MH_OK) return 0;

        // 等待目标进程自然加载 d3d12.dll 与 dxgi.dll
        if (!g_RuntimeModules::WaitAndLoad()) {
            // WaitAndLoad 目前是死循环直到成功，不会返回 false，此处作为保险
            return 0;
        }

        g_Hook::StartAllHooks();
        // g_Hook::initUWorldTick();
        // g_Hook::initHandleDisconnect();
        // g_Hook::initOutputTextLine();
        // g_Hook::initPostRender();

        // 此时 d3d12.dll 与 dxgi.dll 已被目标进程加载，可以安全操作
        while (true) {
            WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DefWindowProcW, 0, 0, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"TempDX12", nullptr };
            RegisterClassEx(&wc);
            HWND tempWnd = CreateWindow(wc.lpszClassName, L"Temp", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);
            if (!tempWnd) { UnregisterClass(wc.lpszClassName, wc.hInstance); Sleep(1); continue; }

            ID3D12Device* tempDevice = nullptr;
            ID3D12CommandQueue* tempQueue = nullptr;
            IDXGIFactory4* factory = nullptr;
            IDXGISwapChain* tempSwapChain = nullptr;

            // 使用运行时加载的函数指针，而非编译期链接的符号
            bool ok = SUCCEEDED(g_RuntimeModules::g_pD3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&tempDevice)));

            if (ok) {
                D3D12_COMMAND_QUEUE_DESC qdesc = {};
                qdesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
                qdesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
                qdesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
                qdesc.NodeMask = 0;
                ok = SUCCEEDED(tempDevice->CreateCommandQueue(&qdesc, IID_PPV_ARGS(&tempQueue)));
            }

            if (ok) {
                // 使用运行时加载的函数指针
                ok = SUCCEEDED(g_RuntimeModules::g_pCreateDXGIFactory1(IID_PPV_ARGS(&factory)));
            }

            if (ok) {
                DXGI_SWAP_CHAIN_DESC sd = {};
                sd.BufferCount = 2;
                sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                sd.OutputWindow = tempWnd;
                sd.SampleDesc.Count = 1;
                sd.Windowed = TRUE;
                sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                ok = SUCCEEDED(factory->CreateSwapChain(tempQueue, &sd, &tempSwapChain));
            }

            if (ok) {
                void** swapVTable = *reinterpret_cast<void***>(tempSwapChain);
                void** queueVTable = *reinterpret_cast<void***>(tempQueue);

                if (swapVTable && swapVTable[g_CheatData::VTable::D3D12::Present]) {
                    MH_CreateHook(swapVTable[g_CheatData::VTable::D3D12::Present], reinterpret_cast<LPVOID>(hkPresent), reinterpret_cast<LPVOID*>(&g_HookFunctions::g_oPresent));
                }

                if (swapVTable && swapVTable[g_CheatData::VTable::D3D12::ResizeBuffers]) {
                    MH_CreateHook(swapVTable[g_CheatData::VTable::D3D12::ResizeBuffers], reinterpret_cast<LPVOID>(hkResizeBuffers), reinterpret_cast<LPVOID*>(&g_HookFunctions::g_oResizeBuffers));
                }

                if (queueVTable && queueVTable[g_CheatData::VTable::D3D12::ExecuteCommandLists]) {
                    MH_CreateHook(queueVTable[g_CheatData::VTable::D3D12::ExecuteCommandLists], reinterpret_cast<LPVOID>(hkExecuteCommandLists), reinterpret_cast<LPVOID*>(&g_HookFunctions::g_oExecuteCommandLists));
                }

                MH_EnableHook(MH_ALL_HOOKS);
                tempSwapChain->Release();
                factory->Release();
                tempQueue->Release();
                tempDevice->Release();
                DestroyWindow(tempWnd);
                UnregisterClass(wc.lpszClassName, wc.hInstance);

                break;
            }

            if (tempSwapChain) tempSwapChain->Release();
            if (factory) factory->Release();
            if (tempQueue) tempQueue->Release();
            if (tempDevice) tempDevice->Release();

            DestroyWindow(tempWnd);
            UnregisterClass(wc.lpszClassName, wc.hInstance);

            // 2026/3/29 @zetsr
            // 才发现不知道啥时候初始化了两次，初始化的时候居然一直没崩溃，minhook确实厉害
            // 好吧，它其实正在循环初始化，简直是狗屎
            // g_Hook::initUWorldTick();

            Sleep(1);
        }

        return 0;
    }
}