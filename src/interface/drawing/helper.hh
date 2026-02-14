#pragma once
#include "imgui.h"

namespace draw
{
    void RectFilled(ImVec2 min, ImVec2 max, ImU32 col);
    void Rect(ImVec2 min, ImVec2 max, ImU32 col, float thickness = 1.0f);
    void Line(ImVec2 a, ImVec2 b, ImU32 col, float thickness = 1.0f);
    void Text(ImVec2 pos, ImU32 col, const char* text);
    void TextShadow(ImVec2 pos, ImU32 col, const char* text);
    void Circle(ImVec2 center, float radius, ImU32 col, int segments = 12, float thickness = 1.0f);
    void CircleFilled(ImVec2 center, float radius, ImU32 col, int segments = 12);
}
