// ESP.cpp
#include "../../external/SDK/SDK_Headers.hpp"
#include "ESP.h"
#include "../Config/Configs.h"
#include "../Util/Util.h"
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>

namespace {
    // 辅助绘制实心矩形的方法，利用Canvas的默认贴图
    void DrawFilledRect(SDK::UCanvas* Canvas, const SDK::FVector2D& Pos, const SDK::FVector2D& Size, const SDK::FLinearColor& Color) {
        if (!Canvas || !Canvas->DefaultTexture) return;
        Canvas->K2_DrawTexture(Canvas->DefaultTexture, Pos, Size, SDK::FVector2D{ 0, 0 }, SDK::FVector2D{ 1, 1 }, Color, SDK::EBlendMode::BLEND_Translucent, 0.0f, SDK::FVector2D{ 0, 0 });
    }
}

namespace g_ESP {
    RelationType GetRelation(SDK::APrimalCharacter* TargetChar, SDK::APrimalCharacter* LocalChar) {
        if (!TargetChar || !LocalChar) return RelationType::Enemy;

        int targetTeam = TargetChar->TargetingTeam;
        int localTeam = LocalChar->TargetingTeam;

        if (targetTeam != 0 && targetTeam == localTeam) {
            return RelationType::Team;
        }

        return RelationType::Enemy;
    }

    void BarManager::AddBar(SDK::UCanvas* Canvas, BoxRect rect, float currentValue, float maxValue, SDK::FLinearColor color, BarPos pos, BarOrientation orientation, float a) {
        if (!rect.valid || maxValue <= 0 || !Canvas) return;

        float percentage = std::clamp(currentValue / maxValue, 0.0f, 1.0f);

        SDK::FLinearColor bgColor = { 0.0f, 0.0f, 0.0f, a * 0.7f };
        color.A = a;

        if (pos == BarPos::Left && orientation == BarOrientation::Vertical) {
            float boxHeight = rect.bottomRight.Y - rect.topLeft.Y;
            float barWidth = 2.5f;
            float barMargin = 4.0f + leftOffset;

            SDK::FVector2D barBgTop = { rect.topLeft.X - barMargin - barWidth, rect.topLeft.Y };
            SDK::FVector2D barBgBottom = { rect.topLeft.X - barMargin, rect.bottomRight.Y };

            SDK::FVector2D bgPos = { barBgTop.X - 1.0f, barBgTop.Y - 1.0f };
            SDK::FVector2D bgSize = { (barBgBottom.X + 1.0f) - bgPos.X, (barBgBottom.Y + 1.0f) - bgPos.Y };
            DrawFilledRect(Canvas, bgPos, bgSize, bgColor);

            if (currentValue > 0) {
                float dynamicHeight = boxHeight * percentage;
                SDK::FVector2D fgPos = { barBgTop.X, barBgBottom.Y - dynamicHeight };
                SDK::FVector2D fgSize = { barWidth, dynamicHeight };
                DrawFilledRect(Canvas, fgPos, fgSize, color);
            }

            leftOffset += barWidth + 2.0f;
        }
        else if (pos == BarPos::Right && orientation == BarOrientation::Vertical) {
            float boxHeight = rect.bottomRight.Y - rect.topLeft.Y;
            float barWidth = 2.5f;
            float barMargin = 4.0f + rightOffset;

            SDK::FVector2D barBgTop = { rect.bottomRight.X + barMargin, rect.topLeft.Y };
            SDK::FVector2D barBgBottom = { rect.bottomRight.X + barMargin + barWidth, rect.bottomRight.Y };

            SDK::FVector2D bgPos = { barBgTop.X - 1.0f, barBgTop.Y - 1.0f };
            SDK::FVector2D bgSize = { (barBgBottom.X + 1.0f) - bgPos.X, (barBgBottom.Y + 1.0f) - bgPos.Y };
            DrawFilledRect(Canvas, bgPos, bgSize, bgColor);

            if (currentValue > 0) {
                float dynamicHeight = boxHeight * percentage;
                SDK::FVector2D fgPos = { barBgTop.X, barBgBottom.Y - dynamicHeight };
                SDK::FVector2D fgSize = { barWidth, dynamicHeight };
                DrawFilledRect(Canvas, fgPos, fgSize, color);
            }

            rightOffset += barWidth + 2.0f;
        }
        else if (pos == BarPos::Top && orientation == BarOrientation::Horizontal) {
            float boxWidth = rect.bottomRight.X - rect.topLeft.X;
            float barHeight = 2.5f;
            float barMargin = 4.0f + topOffset;

            SDK::FVector2D barBgLeft = { rect.topLeft.X, rect.topLeft.Y - barMargin - barHeight };
            SDK::FVector2D barBgRight = { rect.bottomRight.X, rect.topLeft.Y - barMargin };

            SDK::FVector2D bgPos = { barBgLeft.X - 1.0f, barBgLeft.Y - 1.0f };
            SDK::FVector2D bgSize = { (barBgRight.X + 1.0f) - bgPos.X, (barBgRight.Y + 1.0f) - bgPos.Y };
            DrawFilledRect(Canvas, bgPos, bgSize, bgColor);

            if (currentValue > 0) {
                float dynamicWidth = boxWidth * percentage;
                SDK::FVector2D fgPos = { barBgLeft.X, barBgLeft.Y };
                SDK::FVector2D fgSize = { dynamicWidth, barHeight };
                DrawFilledRect(Canvas, fgPos, fgSize, color);
            }

            topOffset += barHeight + 2.0f;
        }
        else if (pos == BarPos::Bottom && orientation == BarOrientation::Horizontal) {
            float boxWidth = rect.bottomRight.X - rect.topLeft.X;
            float barHeight = 2.5f;
            float barMargin = 4.0f + bottomOffset;

            SDK::FVector2D barBgLeft = { rect.topLeft.X, rect.bottomRight.Y + barMargin };
            SDK::FVector2D barBgRight = { rect.bottomRight.X, rect.bottomRight.Y + barMargin + barHeight };

            SDK::FVector2D bgPos = { barBgLeft.X - 1.0f, barBgLeft.Y - 1.0f };
            SDK::FVector2D bgSize = { (barBgRight.X + 1.0f) - bgPos.X, (barBgRight.Y + 1.0f) - bgPos.Y };
            DrawFilledRect(Canvas, bgPos, bgSize, bgColor);

            if (currentValue > 0) {
                float dynamicWidth = boxWidth * percentage;
                SDK::FVector2D fgPos = { barBgLeft.X, barBgLeft.Y };
                SDK::FVector2D fgSize = { dynamicWidth, barHeight };
                DrawFilledRect(Canvas, fgPos, fgSize, color);
            }

            bottomOffset += barHeight + 2.0f;
        }
    }

    void FlagManager::AddFlag(SDK::UCanvas* Canvas, BoxRect rect, const std::string& text, SDK::FLinearColor color, FlagPos pos, float alphaMult, const BarManager* barMgr) {
        if (!rect.valid || text.empty() || !Canvas) return;

        static SDK::UFont* font = nullptr;
        if (!font)
        {
            static SDK::UObject* _Font = SDK::UObject::FindObject("Font SansationBold18.SansationBold18");
            if (_Font && _Font->IsA(SDK::UFont::StaticClass())) {
                font = (SDK::UFont*)_Font;
            }
        }
        if (!font) return;

        // 【关键修复1】：直接在函数作用域内保留 wstr，让FString拥有生命周期安全的指针！
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring wstr = converter.from_bytes(text);
        SDK::FString fText(wstr.c_str());

        // 缩放字体
        SDK::FVector2D scaleSize = SDK::FVector2D{ 1.0f, 1.0f };

        // 获取精确宽高
        SDK::FVector2D textSize = Canvas->K2_TextSize(font, fText, scaleSize);
        SDK::FVector2D drawPos;

        // 【关键修复2】：补全全部四个方向的 barOffset（解决文字和血条重叠的问题）
        if (pos == FlagPos::Right) {
            float barOffset = barMgr ? barMgr->GetRightOffset() : 0.0f;
            drawPos = { (float)(rect.bottomRight.X + 5.0f + barOffset), (float)(rect.topLeft.Y + rightY) };
            rightY += textSize.Y + 1.0f;
        }
        else if (pos == FlagPos::Left) {
            float barOffset = barMgr ? barMgr->GetLeftOffset() : 0.0f;
            drawPos = { (float)(rect.topLeft.X - 6.0f - barOffset - textSize.X), (float)(rect.topLeft.Y + leftY) };
            leftY += textSize.Y + 1.0f;
        }
        else if (pos == FlagPos::Top) {
            float barOffset = barMgr ? barMgr->GetTopOffset() : 0.0f;
            float centerX = (rect.topLeft.X + rect.bottomRight.X) * 0.5f;
            drawPos = { (float)(centerX - textSize.X * 0.5f), (float)(rect.topLeft.Y - barOffset - topY - textSize.Y - 3.0f) };
            topY += textSize.Y + 1.0f;
        }
        else if (pos == FlagPos::Bottom) {
            float barOffset = barMgr ? barMgr->GetBottomOffset() : 0.0f;
            float centerX = (rect.topLeft.X + rect.bottomRight.X) * 0.5f;
            drawPos = { (float)(centerX - textSize.X * 0.5f), (float)(rect.bottomRight.Y + barOffset + bottomY + 3.0f) };
            bottomY += textSize.Y + 1.0f;
        }

        color.A *= alphaMult;
        SDK::FLinearColor shadowCol = { 0.0f, 0.0f, 0.0f, color.A };

        // 渲染文字
        Canvas->K2_DrawText(font, fText, drawPos, scaleSize, color, 0.0f, shadowCol, SDK::FVector2D{ 1.0f, 1.0f }, false, false, true, shadowCol);
    }

    BoxRect DrawBox(SDK::UCanvas* Canvas, SDK::AActor* entity, float r, float g, float b, float a, float width_scale, bool bTestOnly) {
        BoxRect rect;
        rect.valid = false;
        if (!entity || entity->bHidden) return rect;
        auto PC = g_Util::GetLocalPC();
        if (!PC) return rect;

        SDK::FVector origin, extent;
        entity->GetActorBounds(true, &origin, &extent, false);

        SDK::FVector worldTop = { origin.X, origin.Y, origin.Z + extent.Z };
        SDK::FVector worldBottom = { origin.X, origin.Y, origin.Z - extent.Z };
        SDK::FVector2D screenTop, screenBottom;

        if (PC->ProjectWorldLocationToScreen(worldTop, &screenTop, false) &&
            PC->ProjectWorldLocationToScreen(worldBottom, &screenBottom, false)) {

            float height = std::abs(screenBottom.Y - screenTop.Y);
            float width = height * width_scale;
            rect.topLeft = { (float)(screenTop.X - width * 0.5f), (float)screenTop.Y };
            rect.bottomRight = { (float)(screenTop.X + width * 0.5f), (float)screenBottom.Y };
            rect.valid = true;

            if (!bTestOnly && a > 0.1f && Canvas) {
                SDK::FVector2D boxSize = { (float)(rect.bottomRight.X - rect.topLeft.X), (float)(rect.bottomRight.Y - rect.topLeft.Y) };

                // 阴影边框
                SDK::FVector2D outPos = { (float)(rect.topLeft.X - 1.0f), (float)(rect.topLeft.Y - 1.0f) };
                SDK::FVector2D outSize = { (float)(boxSize.X + 2.0f), (float)(boxSize.Y + 2.0f) };
                Canvas->K2_DrawBox(outPos, outSize, 1.5f, SDK::FLinearColor{ 0.0f, 0.0f, 0.0f, a });

                // 主边框
                Canvas->K2_DrawBox(rect.topLeft, boxSize, 1.0f, SDK::FLinearColor{ r, g, b, a });
            }
        }
        return rect;
    }

    void DrawBox(SDK::UCanvas* Canvas, const BoxRect& rect, SDK::FLinearColor color, float alpha) {
        if (!rect.valid || alpha < 0.001f || !Canvas) return;

        color.A *= alpha;

        float x = rect.topLeft.X;
        float y = rect.topLeft.Y;
        float w = rect.bottomRight.X - rect.topLeft.X;
        float h = rect.bottomRight.Y - rect.topLeft.Y;
        float thickness = 1.0f; // 边框线条粗细

        // 1. 绘制黑色外边框阴影
        SDK::FLinearColor shadowCol = { 0.0f, 0.0f, 0.0f, color.A };
        float s_t = thickness + 2.0f; // 阴影比主线稍微粗一点点
        DrawFilledRect(Canvas, { x - 1.0f, y - 1.0f }, { w + 2.0f, s_t }, shadowCol); // 顶
        DrawFilledRect(Canvas, { x - 1.0f, y + h - thickness - 1.0f }, { w + 2.0f, s_t }, shadowCol); // 底
        DrawFilledRect(Canvas, { x - 1.0f, y - 1.0f }, { s_t, h + 2.0f }, shadowCol); // 左
        DrawFilledRect(Canvas, { x + w - thickness - 1.0f, y - 1.0f }, { s_t, h + 2.0f }, shadowCol); // 右

        // 2. 绘制彩色主边框
        DrawFilledRect(Canvas, { x, y }, { w, thickness }, color); // 顶
        DrawFilledRect(Canvas, { x, y + h - thickness }, { w, thickness }, color); // 底
        DrawFilledRect(Canvas, { x, y }, { thickness, h }, color); // 左
        DrawFilledRect(Canvas, { x + w - thickness, y }, { thickness, h }, color); // 右
    }

    void DrawHealthBar(SDK::UCanvas* Canvas, BoxRect rect, float healthPercent, float maxHealth, float a) {
        if (!rect.valid || maxHealth <= 0 || !Canvas) return;

        float boxHeight = rect.bottomRight.Y - rect.topLeft.Y;
        float barWidth = 2.5f;
        float barMargin = 4.0f;
        SDK::FVector2D barBgTop = { (float)(rect.topLeft.X - barMargin - barWidth), (float)rect.topLeft.Y };
        SDK::FVector2D barBgBottom = { (float)(rect.topLeft.X - barMargin), (float)rect.bottomRight.Y };

        SDK::FVector2D bgPos = { (float)(barBgTop.X - 1.0f), (float)(barBgTop.Y - 1.0f) };
        SDK::FVector2D bgSize = { (float)((barBgBottom.X + 1.0f) - bgPos.X), (float)((barBgBottom.Y + 1.0f) - bgPos.Y) };
        DrawFilledRect(Canvas, bgPos, bgSize, SDK::FLinearColor{ 0.0f, 0.0f, 0.0f, a * 0.7f });

        float percentage = std::clamp(healthPercent / maxHealth, 0.0f, 1.0f);

        SDK::FLinearColor col;
        if (percentage > 0.5f)
            col = SDK::FLinearColor{ (1.0f - percentage) * 2.0f, 1.0f, 0.0f, a };
        else
            col = SDK::FLinearColor{ 1.0f, percentage * 2.0f, 0.0f, a };

        if (healthPercent > 0) {
            float dynamicHeight = boxHeight * percentage;
            SDK::FVector2D fgPos = { (float)barBgTop.X, (float)(barBgBottom.Y - dynamicHeight) };
            SDK::FVector2D fgSize = { barWidth, dynamicHeight };
            DrawFilledRect(Canvas, fgPos, fgSize, col);
        }
    }
}