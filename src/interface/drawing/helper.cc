#include "helper.hh"

namespace draw
{
    void RectFilled(ImVec2 min, ImVec2 max, ImU32 col)
    {
        ImGui::GetWindowDrawList()->AddRectFilled(min, max, col);
    }

    void Rect(ImVec2 min, ImVec2 max, ImU32 col, float thickness)
    {
        ImGui::GetWindowDrawList()->AddRect(min, max, col, 0.0f, 0, thickness);
    }

    void Line(ImVec2 a, ImVec2 b, ImU32 col, float thickness)
    {
        ImGui::GetWindowDrawList()->AddLine(a, b, col, thickness);
    }

    void Text(ImVec2 pos, ImU32 col, const char* text)
    {
        ImGui::GetWindowDrawList()->AddText(pos, col, text);
    }

    void TextShadow(ImVec2 pos, ImU32 col, const char* text)
    {
        ImDrawList* draw = ImGui::GetWindowDrawList();
        draw->AddText(ImVec2(pos.x + 1, pos.y + 1), IM_COL32(0, 0, 0, 150), text);
        draw->AddText(pos, col, text);
    }

    void Circle(ImVec2 center, float radius, ImU32 col, int segments, float thickness)
    {
        ImGui::GetWindowDrawList()->AddCircle(center, radius, col, segments, thickness);
    }

    void CircleFilled(ImVec2 center, float radius, ImU32 col, int segments)
    {
        ImGui::GetWindowDrawList()->AddCircleFilled(center, radius, col, segments);
    }
}
