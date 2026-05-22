// ESP.h
#pragma once
#include <vector>
#include <string>
#include "../../external/SDK/SDK_Headers.hpp"

namespace g_ESP {
    struct BoxRect {
        SDK::FVector2D topLeft;
        SDK::FVector2D bottomRight;
        bool valid = false;
    };

    enum class FlagPos {
        Left,
        Right,
        Top,
        Bottom
    };

    enum class BarPos {
        Left,
        Right,
        Top,
        Bottom
    };

    enum class BarOrientation {
        Vertical,
        Horizontal
    };

    class BarManager {
    private:
        float leftOffset = 0.0f;
        float rightOffset = 0.0f;
        float topOffset = 0.0f;
        float bottomOffset = 0.0f;

    public:
        void Reset() {
            leftOffset = 0.0f;
            rightOffset = 0.0f;
            topOffset = 0.0f;
            bottomOffset = 0.0f;
        }

        float GetLeftOffset() const { return leftOffset; }
        float GetRightOffset() const { return rightOffset; }
        float GetTopOffset() const { return topOffset; }
        float GetBottomOffset() const { return bottomOffset; }

        void AddBar(SDK::UCanvas* Canvas, BoxRect rect, float currentValue, float maxValue, SDK::FLinearColor color, BarPos pos, BarOrientation orientation, float a);
    };

    class FlagManager {
    private:
        float leftY = 0.0f;
        float rightY = 0.0f;
        float topY = 0.0f;
        float bottomY = 0.0f;

    public:
        void Reset() {
            leftY = 0.0f;
            rightY = 0.0f;
            topY = 0.0f;
            bottomY = 0.0f;
        }

        void AddFlag(SDK::UCanvas* Canvas, BoxRect rect, const std::string& text, SDK::FLinearColor color, FlagPos pos, float alphaMult, const BarManager* barMgr);
    };

    BoxRect DrawBox(SDK::UCanvas* Canvas, SDK::AActor* entity, float r, float g, float b, float a, float width_scale, bool bTestOnly);
    void DrawBox(SDK::UCanvas* Canvas, const BoxRect& rect, SDK::FLinearColor color, float alpha);

    void DrawHealthBar(SDK::UCanvas* Canvas, BoxRect rect, float healthPercent, float maxHealth, float a);

    struct OOFFlag {
        std::string text;
        SDK::FLinearColor color;
    };

    enum class RelationType {
        Enemy,
        Team,
    };

    RelationType GetRelation(SDK::APrimalCharacter* TargetChar, SDK::APrimalCharacter* LocalChar);
}