#pragma once
#define NOMINMAX
#include "../../external/Minimal-D3D12-Hook-ImGui/Main/mdx12_api.h"
#include "../Config/Configs.h"
#include "../Lua/LuaManager.h"
#include "ConfigImGui.h"
#include "Lua_Menu.h"
#include "../Util/Util.h"
#include "../../external/SDK/SDK_Headers.hpp"
#include "../Language/LanguageManager.h"

namespace g_DrawImGui {
    void Lua_Menu() {
        // 每一帧更新状态（处理重置逻辑）
        LuaManager::Get().Update();

        const char* tabLabel = LanguageManager::Lua_Menu::TabLabel;
        const char* enableMgr = LanguageManager::Lua_Menu::EnableManager;
        const char* secTitle = LanguageManager::Lua_Menu::SectionTitle;
        const char* btnRefresh = LanguageManager::Lua_Menu::RefreshFileList;
        const char* btnOpenDir = LanguageManager::Lua_Menu::OpenDirectory;
        const char* errorHint = LanguageManager::Lua_Menu::ErrorHint;
        const char* copyError = LanguageManager::Lua_Menu::CopyError;

        if (ImGui::BeginTabItem(tabLabel)) {
            auto& mgr = LuaManager::Get();
            auto& scripts = mgr.GetScripts();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(14.0f, 14.0f));
            BeginTabRegion("LuaManagerRegion");

            // --- 第一部分：全局总开关 ---
            bool isEnabled = mgr.IsEnabled();
            ImGui::PushStyleColor(ImGuiCol_Text, ThemeColors::GetAccent());
            if (DrawCustomCheckbox(enableMgr, &isEnabled)) {
                // 当点击 Checkbox 时触发管理器状态切换
                mgr.SetEnabled(isEnabled);
            }
            ImGui::PopStyleColor();

            // ImGui::SameLine();
            // ImGui::TextDisabled(U8("(关闭将卸载所有脚本)"));

            DrawAnimatedSeparator();

            // --- 第二部分：受控区域 (如果 Enabled 为 false，则全部变灰且不可交互) ---
            ImGui::BeginDisabled(!isEnabled);
            {
                ImGui::TextColored(ThemeColors::GetAccent(), secTitle);

                if (DrawCustomButton(btnRefresh)) {
                    mgr.RefreshFileList();
                }
                ImGui::SameLine();
                if (DrawCustomButton(btnOpenDir)) {
                    std::string path = mgr.GetScriptDir();
                    ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
                }

                ImGui::Spacing();

                // 脚本列表子窗口
                if (ImGui::BeginChild("##LuaScriptListChild", ImVec2(0, 300), true)) {
                    if (scripts.empty()) {
                        // ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.5f - 10);
                        // 可以显示提示文本
                    }
                    else {
                        for (int i = 0; i < (int)scripts.size(); i++) {
                            auto& script = scripts[i];
                            if (script.isLibrary) continue;
                            ImGui::PushID(i);

                            // 决定显示颜色：如果脚本已加载且总开关开启，则高亮，否则变灰
                            bool isActive = script.isLoaded;
                            if (isActive && isEnabled) {
                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                            }
                            else {
                                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                            }

                            std::string prefix = (isActive && isEnabled) ? U8("* ") : U8("");
                            std::string displayName = prefix + script.name;

                            // 选择脚本进行加载/卸载
                            if (CustomSelectable(displayName.c_str(), false, 8.0f)) {
                                bool targetState = !script.isLoaded;
                                mgr.SetScriptState(i, targetState);
                            }

                            ImGui::PopStyleColor();

                            // 错误状态图标
                            if (script.hasError) {
                                ImGui::SameLine(ImGui::GetWindowWidth() - 35);
                                ImGui::TextDisabled("[!] ");

                                if (ImGui::IsItemHovered()) {
                                    ImGui::BeginTooltip();
                                    ImGui::TextUnformatted(errorHint);
                                    ImGui::Separator();
                                    ImGui::TextUnformatted(script.lastError.c_str());
                                    ImGui::EndTooltip();
                                }

                                std::string popupId = "ErrorPopup_" + std::to_string(i);
                                if (ImGui::BeginPopupContextItem(popupId.c_str())) {
                                    if (CustomSelectable(copyError, false, 8.0f)) {
                                        ImGui::SetClipboardText(script.lastError.c_str());
                                    }
                                    ImGui::EndPopup();
                                }
                            }

                            ImGui::PopID();
                        }
                    }
                    ImGui::EndChild();
                }
            }
            ImGui::EndDisabled(); // 结束受控禁用区域

            DrawAnimatedSeparator();
            EndTabRegion();
            ImGui::PopStyleVar();
            ImGui::EndTabItem();
        }
    }
}