#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include <cmath>

namespace widgets
{
    struct ReorderAnim
    {
        float hover[32] = {};
        float offset_y[32] = {};
        int drag_from = -1;
        int drag_to = -1;
        bool dragging = false;
        float drag_y = 0.0f;
        float drag_start_y = 0.0f;
    };

    static std::unordered_map<ImGuiID, ReorderAnim> reorder_anims_t;

    static ImU32 LerpColorReorder(ImU32 a, ImU32 b, float t)
    {
        int ra = (a >> 0) & 0xFF, ga = (a >> 8) & 0xFF, ba = (a >> 16) & 0xFF, aa = (a >> 24) & 0xFF;
        int rb = (b >> 0) & 0xFF, gb = (b >> 8) & 0xFF, bb = (b >> 16) & 0xFF, ab = (b >> 24) & 0xFF;
        return IM_COL32(
            (int)(ra + (rb - ra) * t),
            (int)(ga + (gb - ga) * t),
            (int)(ba + (bb - ba) * t),
            (int)(aa + (ab - aa) * t)
        );
    }

    bool ReorderList(const char* label, int* order, const char* const* items, int items_count)
    {
        if (items_count <= 0 || items_count > 32) return false;

        ImGui::PushID(label);
        ImGuiID id = ImGui::GetID("##reorder");
        ReorderAnim& anim = reorder_anims_t[id];

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImGuiIO& io = ImGui::GetIO();
        float dt = io.DeltaTime;
        float width = ImGui::GetContentRegionAvail().x;

        const float label_height = S(16.0f);
        const float item_height = S(18.0f);
        const float handle_w = S(14.0f);
        const float rounding = 2.0f;
        const float pad = S(3.0f);
        float total_height = label_height + (items_count * item_height) + pad * 2;

        ImGui::InvisibleButton("##reorder_area", ImVec2(width, total_height));

        DrawTextShadow(draw, pos, IM_COL32(180, 180, 180, 255), label);

        float list_y = pos.y + label_height;
        float list_h = items_count * item_height + pad * 2;
        draw->AddRectFilled(ImVec2(pos.x, list_y), ImVec2(pos.x + width, list_y + list_h),
            IM_COL32(18, 18, 18, 255), rounding);
        draw->AddRect(ImVec2(pos.x, list_y), ImVec2(pos.x + width, list_y + list_h),
            IM_COL32(40, 40, 40, 255), rounding);

        ImVec2 mouse = io.MousePos;
        bool changed = false;

        if (anim.dragging)
        {
            anim.drag_y = mouse.y;
            float rel_y = mouse.y - (list_y + pad);
            int target = (int)(rel_y / item_height);
            if (target < 0) target = 0;
            if (target >= items_count) target = items_count - 1;
            anim.drag_to = target;
        }

        for (int i = 0; i < items_count; i++)
        {
            int display_idx = i;
            float extra_y = 0.0f;

            if (anim.dragging && i != anim.drag_from)
            {
                if (anim.drag_from < anim.drag_to)
                {
                    if (i > anim.drag_from && i <= anim.drag_to)
                        extra_y = -item_height;
                }
                else if (anim.drag_from > anim.drag_to)
                {
                    if (i >= anim.drag_to && i < anim.drag_from)
                        extra_y = item_height;
                }
            }

            float target_offset = extra_y;
            anim.offset_y[i] = anim.offset_y[i] + (target_offset - anim.offset_y[i]) * Clamp(dt * 12.0f, 0.0f, 1.0f);
            if (fabsf(anim.offset_y[i] - target_offset) < 0.5f)
                anim.offset_y[i] = target_offset;

            float iy = list_y + pad + i * item_height + anim.offset_y[i];

            if (anim.dragging && i == anim.drag_from)
                continue;

            bool item_hovered = (mouse.x >= pos.x && mouse.x <= pos.x + width &&
                mouse.y >= iy && mouse.y <= iy + item_height) && !anim.dragging;

            float hover_target = item_hovered ? 1.0f : 0.0f;
            anim.hover[i] = anim.hover[i] + (hover_target - anim.hover[i]) * Clamp(dt * 10.0f, 0.0f, 1.0f);

            ImU32 bg_col = LerpColorReorder(IM_COL32(18, 18, 18, 0), IM_COL32(30, 30, 30, 255), anim.hover[i]);
            draw->AddRectFilled(ImVec2(pos.x + 1, iy), ImVec2(pos.x + width - 1, iy + item_height), bg_col, rounding);

            float hx = pos.x + pad + S(2.0f);
            float hy = iy + item_height * 0.5f;
            ImU32 handle_col = LerpColorReorder(IM_COL32(50, 50, 50, 200), IM_COL32(90, 90, 90, 255), anim.hover[i]);
            for (int line = -1; line <= 1; line++)
            {
                float ly = hy + line * S(3.0f);
                draw->AddLine(ImVec2(hx, ly), ImVec2(hx + S(8.0f), ly), handle_col, 1.0f);
            }

            ImU32 text_col = LerpColorReorder(IM_COL32(170, 170, 170, 255), IM_COL32(220, 220, 220, 255), anim.hover[i]);
            draw->AddText(ImVec2(pos.x + handle_w + pad, iy + (item_height - ImGui::GetTextLineHeight()) * 0.5f),
                text_col, items[order[i]]);

            bool on_handle = (mouse.x >= pos.x && mouse.x <= pos.x + handle_w &&
                mouse.y >= iy && mouse.y <= iy + item_height);
            if (on_handle && ImGui::IsMouseClicked(0) && !anim.dragging)
            {
                anim.dragging = true;
                anim.drag_from = i;
                anim.drag_to = i;
                anim.drag_start_y = iy;
            }
        }

        if (anim.dragging && anim.drag_from >= 0 && anim.drag_from < items_count)
        {
            float drag_offset = anim.drag_y - anim.drag_start_y;
            float iy = list_y + pad + anim.drag_from * item_height + drag_offset;

            draw->AddRectFilled(ImVec2(pos.x + 1, iy), ImVec2(pos.x + width - 1, iy + item_height),
                gui::colors::AccentBg(0.20f, 240), rounding);
            draw->AddRect(ImVec2(pos.x + 1, iy), ImVec2(pos.x + width - 1, iy + item_height),
                gui::colors::AccentA(200), rounding);

            float hx = pos.x + pad + S(2.0f);
            float hy_center = iy + item_height * 0.5f;
            for (int line = -1; line <= 1; line++)
            {
                float ly = hy_center + line * S(3.0f);
                draw->AddLine(ImVec2(hx, ly), ImVec2(hx + S(8.0f), ly), gui::colors::Get().accent, 1.0f);
            }

            draw->AddText(ImVec2(pos.x + handle_w + pad, iy + (item_height - ImGui::GetTextLineHeight()) * 0.5f),
                IM_COL32(240, 240, 240, 255), items[order[anim.drag_from]]);
        }

        if (anim.dragging && !ImGui::IsMouseDown(0))
        {
            if (anim.drag_from != anim.drag_to && anim.drag_from >= 0 && anim.drag_to >= 0)
            {
                int moved = order[anim.drag_from];
                if (anim.drag_from < anim.drag_to)
                {
                    for (int j = anim.drag_from; j < anim.drag_to; j++)
                        order[j] = order[j + 1];
                }
                else
                {
                    for (int j = anim.drag_from; j > anim.drag_to; j--)
                        order[j] = order[j - 1];
                }
                order[anim.drag_to] = moved;
                changed = true;
            }

            for (int i = 0; i < items_count; i++)
                anim.offset_y[i] = 0.0f;

            anim.dragging = false;
            anim.drag_from = -1;
            anim.drag_to = -1;
        }

        ImGui::PopID();
        return changed;
    }
}
