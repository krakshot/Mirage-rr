#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include <cmath>

namespace widgets
{
    static float EaseOutQuad(float t)
    {
        return 1.0f - (1.0f - t) * (1.0f - t);
    }

    static float EaseOutBack(float t)
    {
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;
        return 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
    }

    static ImU32 LerpColor(ImU32 a, ImU32 b, float t)
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

    bool Combo(const char* label, int* current, const char* const* items, int items_count, const char* hint, Keybind* bind)
    {
        ImGui::PushID(label);
        ImGuiID id = ImGui::GetID("##combo");

        ComboAnim& anim = combo_anims_t[id];

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float width = ImGui::GetContentRegionAvail().x;
        const float row_height = S(18.0f);
        const float label_width = S(65.0f);
        const float combo_height = S(16.0f);
        const float rounding = 2.0f;

        float combo_x = pos.x + label_width;
        float right_reserved = (hint && hint[0] ? S(14.0f) : 0.0f) + (bind ? S(32.0f) : 0.0f);
        float combo_w = width - label_width - right_reserved;
        float combo_y = pos.y + (row_height - combo_height) * 0.5f;

        ImGui::Dummy(ImVec2(width, row_height));

        ImVec2 mouse = ImGui::GetIO().MousePos;

        bool blocked_by_dropdown = false;
        if (state_t.open_combo != 0 && state_t.open_combo != id)
        {
            if (mouse.x >= state_t.dropdown_x1 && mouse.x <= state_t.dropdown_x2 &&
                mouse.y >= state_t.dropdown_y1 && mouse.y <= state_t.dropdown_y2)
            {
                blocked_by_dropdown = true;
            }
        }

        bool combo_hovered = !blocked_by_dropdown && (mouse.x >= combo_x && mouse.x <= combo_x + combo_w &&
            mouse.y >= combo_y && mouse.y <= combo_y + combo_height);
        bool row_hovered = !blocked_by_dropdown && ImGui::IsItemHovered();

        bool is_open = (state_t.open_combo == id);
        bool changed = false;

        if (combo_hovered && ImGui::IsMouseClicked(0))
        {
            if (is_open)
            {
                state_t.open_combo = 0;
                is_open = false;
            }
            else
            {
                state_t.open_combo = id;
                is_open = true;
                state_t.combo_hover_idx = *current;
            }
        }

        float dt = ImGui::GetIO().DeltaTime;
        float open_target = is_open ? 1.0f : 0.0f;
        float hover_target = (combo_hovered || row_hovered) ? 1.0f : 0.0f;
        float arrow_target = is_open ? 1.0f : 0.0f;
        float glow_target = is_open ? 1.0f : 0.0f;

        float open_speed = is_open ? 12.0f : 8.0f;
        anim.open = Lerp(anim.open, open_target, dt * open_speed);
        anim.hover = Lerp(anim.hover, hover_target, dt * 10.0f);
        anim.arrow_rot = Lerp(anim.arrow_rot, arrow_target, dt * 12.0f);
        anim.glow = Lerp(anim.glow, glow_target, dt * 8.0f);

        if (anim.open < 0.01f) anim.open = 0.0f;
        if (anim.open > 0.99f) anim.open = 1.0f;

        ImDrawList* draw = ImGui::GetWindowDrawList();

        const auto& pal = gui::colors::Get();
        ImU32 col_bg_idle = IM_COL32(18, 18, 18, 255);
        ImU32 col_bg_hover = IM_COL32(22, 22, 22, 255);
        ImU32 col_bg_open = IM_COL32(25, 25, 25, 255);
        ImU32 col_border_idle = IM_COL32(40, 40, 40, 255);
        ImU32 col_border_hover = IM_COL32(60, 60, 60, 255);
        ImU32 col_border_open = pal.accent_dim;
        ImU32 col_label_idle = IM_COL32(150, 150, 150, 255);
        ImU32 col_label_hover = IM_COL32(200, 200, 200, 255);
        ImU32 col_label_open = pal.accent;
        ImU32 col_arrow_idle = IM_COL32(80, 80, 80, 255);
        ImU32 col_arrow_hover = IM_COL32(120, 120, 120, 255);
        ImU32 col_arrow_open = pal.accent;

        ImU32 bg_col = LerpColor(col_bg_idle, col_bg_hover, anim.hover);
        bg_col = LerpColor(bg_col, col_bg_open, anim.glow);

        ImU32 border_col = LerpColor(col_border_idle, col_border_hover, anim.hover);
        border_col = LerpColor(border_col, col_border_open, anim.glow);

        ImU32 label_col = LerpColor(col_label_idle, col_label_hover, anim.hover);
        label_col = LerpColor(label_col, col_label_open, anim.glow);

        ImU32 arrow_col = LerpColor(col_arrow_idle, col_arrow_hover, anim.hover);
        arrow_col = LerpColor(arrow_col, col_arrow_open, anim.glow);

        ImVec2 label_pos(pos.x, pos.y + (row_height - ImGui::GetTextLineHeight()) * 0.5f);
        DrawTextShadow(draw, label_pos, label_col, label);

        if (anim.glow > 0.01f)
        {
            ImU32 glow_col = gui::colors::AccentDimA((int)(25 * anim.glow));
            draw->AddRectFilled(
                ImVec2(combo_x - 2, combo_y - 2),
                ImVec2(combo_x + combo_w + 2, combo_y + combo_height + 2),
                glow_col, rounding + 2
            );
        }

        draw->AddRectFilled(
            ImVec2(combo_x, combo_y),
            ImVec2(combo_x + combo_w, combo_y + combo_height),
            bg_col, rounding
        );

        draw->AddRect(
            ImVec2(combo_x, combo_y),
            ImVec2(combo_x + combo_w, combo_y + combo_height),
            border_col, rounding
        );

        float arrow_size = S(4.0f);
        float arrow_x = combo_x + combo_w - S(10.0f);
        float arrow_y_center = combo_y + combo_height * 0.5f;

        float rot = anim.arrow_rot * 3.14159f;
        float cos_r = cosf(rot);
        float sin_r = sinf(rot);

        ImVec2 p1(-arrow_size, -arrow_size * 0.4f);
        ImVec2 p2(arrow_size, -arrow_size * 0.4f);
        ImVec2 p3(0, arrow_size * 0.6f);

        auto rotate_point = [&](ImVec2& p) {
            float x = p.x * cos_r - p.y * sin_r;
            float y = p.x * sin_r + p.y * cos_r;
            p.x = arrow_x + x;
            p.y = arrow_y_center + y;
            };
        rotate_point(p1);
        rotate_point(p2);
        rotate_point(p3);

        draw->AddTriangleFilled(p1, p2, p3, arrow_col);

        const char* current_item = (items_count > 0 && *current >= 0 && *current < items_count) ? items[*current] : "";
        ImU32 value_col = LerpColor(IM_COL32(180, 180, 180, 255), IM_COL32(255, 255, 255, 255), anim.glow);
        ImVec2 val_pos(combo_x + S(6.0f), combo_y + (combo_height - ImGui::GetTextLineHeight()) * 0.5f);
        DrawTextShadow(draw, val_pos, value_col, current_item);

        if (anim.open > 0.01f && items_count > 0)
        {
            const float item_height = S(18.0f);
            float dropdown_h = items_count * item_height + S(6.0f);
            float dropdown_y = combo_y + combo_height + 2.0f;

            if (is_open)
            {
                state_t.dropdown_x1 = combo_x;
                state_t.dropdown_y1 = dropdown_y;
                state_t.dropdown_x2 = combo_x + combo_w;
                state_t.dropdown_y2 = dropdown_y + dropdown_h;
            }

            float eased_open = EaseOutQuad(anim.open);
            float current_h = dropdown_h * eased_open;

            ImDrawList* fg = ImGui::GetForegroundDrawList();

            if (anim.open > 0.1f)
            {
                ImU32 shadow_col = IM_COL32(0, 0, 0, (int)(60 * anim.open));
                fg->AddRectFilled(
                    ImVec2(combo_x + 3, dropdown_y + 3),
                    ImVec2(combo_x + combo_w + 3, dropdown_y + current_h + 3),
                    shadow_col, rounding
                );
            }

            fg->AddRectFilled(
                ImVec2(combo_x, dropdown_y),
                ImVec2(combo_x + combo_w, dropdown_y + current_h),
                IM_COL32(18, 18, 18, (int)(250 * anim.open)), rounding
            );

            fg->AddRect(
                ImVec2(combo_x, dropdown_y),
                ImVec2(combo_x + combo_w, dropdown_y + current_h),
                IM_COL32(50, 50, 50, (int)(255 * anim.open)), rounding
            );

            fg->PushClipRect(
                ImVec2(combo_x, dropdown_y),
                ImVec2(combo_x + combo_w, dropdown_y + current_h), true);

            for (int i = 0; i < items_count && i < 32; i++)
            {
                float item_y = dropdown_y + S(3.0f) + i * item_height;
                bool item_hovered = is_open && (mouse.x >= combo_x && mouse.x <= combo_x + combo_w &&
                    mouse.y >= item_y && mouse.y <= item_y + item_height);

                float item_hover_target = item_hovered ? 1.0f : 0.0f;
                anim.item_hovers[i] = Lerp(anim.item_hovers[i], item_hover_target, dt * 12.0f);

                if (item_hovered)
                {
                    anim.hover_idx = i;
                    if (ImGui::IsMouseClicked(0))
                    {
                        *current = i;
                        state_t.open_combo = 0;
                        changed = true;
                    }
                }

                float highlight = anim.item_hovers[i];
                if (i == *current && highlight < 0.3f) highlight = 0.3f;

                if (highlight > 0.01f)
                {
                    ImU32 highlight_col = (i == *current)
                        ? gui::colors::AccentBg(0.55f, (int)(180 * highlight * anim.open))
                        : gui::colors::AccentBg(0.45f, (int)(200 * highlight * anim.open));
                    fg->AddRectFilled(
                        ImVec2(combo_x + 2, item_y),
                        ImVec2(combo_x + combo_w - 2, item_y + item_height),
                        highlight_col, rounding - 1
                    );
                }

                ImU32 item_text_base = (i == *current) ? pal.accent : IM_COL32(160, 160, 160, 255);
                ImU32 item_text_hover = IM_COL32(255, 255, 255, 255);
                ImU32 item_text_col = LerpColor(item_text_base, item_text_hover, anim.item_hovers[i]);
                item_text_col = (item_text_col & 0x00FFFFFF) | ((int)(255 * anim.open) << 24);

                float text_x = combo_x + S(8.0f);

                if (i == *current)
                {
                    ImU32 check_col = gui::colors::AccentA((int)(255 * anim.open));
                    float cx = combo_x + S(5.0f);
                    float cy = item_y + item_height * 0.5f;
                    fg->AddCircleFilled(ImVec2(cx + 1.5f, cy), 2.0f, check_col);
                    text_x = combo_x + S(12.0f);
                }

                fg->AddText(ImVec2(text_x, item_y + (item_height - ImGui::GetTextLineHeight()) * 0.5f), item_text_col, items[i]);
            }

            fg->PopClipRect();

            if (ImGui::IsMouseClicked(0) && !combo_hovered)
            {
                bool in_dropdown = (mouse.x >= combo_x && mouse.x <= combo_x + combo_w &&
                    mouse.y >= dropdown_y && mouse.y <= dropdown_y + dropdown_h);
                if (!in_dropdown)
                {
                    state_t.open_combo = 0;
                }
            }
        }

        float right_edge = pos.x + width;
        if (hint && hint[0] != '\0')
        {
            float qx = right_edge - S(10.0f);
            float qy = pos.y + (row_height - S(10.0f)) * 0.5f;
            DrawHintTooltip(ImGui::GetID(label), qx, qy, S(10.0f), S(12.0f), hint);
            right_edge -= S(14.0f);
        }
        if (bind)
        {
            float kb_x = right_edge - S(28.0f);
            DrawKeybind(bind, kb_x, pos.y, S(28.0f));
        }

        ImGui::PopID();
        return changed;
    }
}
