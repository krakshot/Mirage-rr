#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include <cmath>
#include <cstring>

namespace widgets
{
    std::unordered_map<ImGuiID, ComboMultiAnim> combomulti_anims_t;

    static ImU32 LerpColorMC(ImU32 a, ImU32 b, float t)
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

    static float EaseOutQuadMC(float t)
    {
        return 1.0f - (1.0f - t) * (1.0f - t);
    }

    static void BuildPreview(char* buf, int buf_size, bool* selected, const char* const* items, int count)
    {
        buf[0] = '\0';
        int written = 0;
        for (int i = 0; i < count; i++)
        {
            if (!selected[i]) continue;
            if (written > 0)
            {
                int len = (int)strlen(buf);
                if (len + 2 < buf_size)
                {
                    strcat(buf, ", ");
                }
            }
            int len = (int)strlen(buf);
            int item_len = (int)strlen(items[i]);
            if (len + item_len < buf_size)
            {
                strcat(buf, items[i]);
            }
            else
            {
                if (len + 3 < buf_size)
                {
                    strcat(buf, "...");
                }
                break;
            }
            written++;
        }
        if (written == 0)
            strncpy(buf, "None", buf_size);
    }

    bool ComboMulti(const char* label, bool* selected, const char* const* items, int items_count, const char* hint)
    {
        ImGui::PushID(label);
        ImGuiID id = ImGui::GetID("##combomulti");

        ComboMultiAnim& anim = combomulti_anims_t[id];

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float width = ImGui::GetContentRegionAvail().x;
        const float row_height = S(18.0f);
        const float label_width = S(65.0f);
        const float combo_height = S(16.0f);
        const float rounding = 2.0f;

        float combo_x = pos.x + label_width;
        float right_reserved = (hint && hint[0] ? S(14.0f) : 0.0f);
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
            }
        }

        float dt = ImGui::GetIO().DeltaTime;
        float open_speed = is_open ? 12.0f : 8.0f;
        anim.open = Lerp(anim.open, is_open ? 1.0f : 0.0f, dt * open_speed);
        anim.hover = Lerp(anim.hover, (combo_hovered || row_hovered) ? 1.0f : 0.0f, dt * 10.0f);
        anim.arrow_rot = Lerp(anim.arrow_rot, is_open ? 1.0f : 0.0f, dt * 12.0f);
        anim.glow = Lerp(anim.glow, is_open ? 1.0f : 0.0f, dt * 8.0f);

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

        ImU32 bg_col = LerpColorMC(col_bg_idle, col_bg_hover, anim.hover);
        bg_col = LerpColorMC(bg_col, col_bg_open, anim.glow);
        ImU32 border_col = LerpColorMC(col_border_idle, col_border_hover, anim.hover);
        border_col = LerpColorMC(border_col, col_border_open, anim.glow);
        ImU32 label_col = LerpColorMC(col_label_idle, col_label_hover, anim.hover);
        label_col = LerpColorMC(label_col, col_label_open, anim.glow);
        ImU32 arrow_col = LerpColorMC(col_arrow_idle, col_arrow_hover, anim.hover);
        arrow_col = LerpColorMC(arrow_col, col_arrow_open, anim.glow);

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

        draw->AddRectFilled(ImVec2(combo_x, combo_y), ImVec2(combo_x + combo_w, combo_y + combo_height), bg_col, rounding);
        draw->AddRect(ImVec2(combo_x, combo_y), ImVec2(combo_x + combo_w, combo_y + combo_height), border_col, rounding);

        float arrow_size = S(4.0f);
        float arrow_x = combo_x + combo_w - S(10.0f);
        float arrow_y_center = combo_y + combo_height * 0.5f;
        float rot = anim.arrow_rot * 3.14159f;
        float cos_r = cosf(rot), sin_r = sinf(rot);

        ImVec2 p1(-arrow_size, -arrow_size * 0.4f);
        ImVec2 p2(arrow_size, -arrow_size * 0.4f);
        ImVec2 p3(0, arrow_size * 0.6f);
        auto rotate_point = [&](ImVec2& p) {
            float x = p.x * cos_r - p.y * sin_r;
            float y = p.x * sin_r + p.y * cos_r;
            p.x = arrow_x + x;
            p.y = arrow_y_center + y;
            };
        rotate_point(p1); rotate_point(p2); rotate_point(p3);
        draw->AddTriangleFilled(p1, p2, p3, arrow_col);

        char preview[128];
        BuildPreview(preview, sizeof(preview), selected, items, items_count);

        draw->PushClipRect(ImVec2(combo_x + S(4.0f), combo_y), ImVec2(combo_x + combo_w - S(16.0f), combo_y + combo_height), true);
        ImU32 value_col = LerpColorMC(IM_COL32(180, 180, 180, 255), IM_COL32(255, 255, 255, 255), anim.glow);
        ImVec2 val_pos(combo_x + S(6.0f), combo_y + (combo_height - ImGui::GetTextLineHeight()) * 0.5f);
        DrawTextShadow(draw, val_pos, value_col, preview);
        draw->PopClipRect();

        if (anim.open > 0.01f && items_count > 0)
        {
            const float item_height = S(18.0f);
            int vis_count = items_count > 8 ? 8 : items_count;
            float dropdown_h = vis_count * item_height + S(6.0f);
            float content_h = items_count * item_height;
            float dropdown_y = combo_y + combo_height + S(2.0f);

            if (is_open)
            {
                state_t.dropdown_x1 = combo_x;
                state_t.dropdown_y1 = dropdown_y;
                state_t.dropdown_x2 = combo_x + combo_w;
                state_t.dropdown_y2 = dropdown_y + dropdown_h;
            }

            float eased_open = EaseOutQuadMC(anim.open);
            float current_h = dropdown_h * eased_open;

            ImDrawList* fg = ImGui::GetForegroundDrawList();

            if (anim.open > 0.1f)
            {
                fg->AddRectFilled(
                    ImVec2(combo_x + 3, dropdown_y + 3),
                    ImVec2(combo_x + combo_w + 3, dropdown_y + current_h + 3),
                    IM_COL32(0, 0, 0, (int)(60 * anim.open)), rounding
                );
            }

            fg->AddRectFilled(ImVec2(combo_x, dropdown_y), ImVec2(combo_x + combo_w, dropdown_y + current_h),
                IM_COL32(18, 18, 18, (int)(250 * anim.open)), rounding);
            fg->AddRect(ImVec2(combo_x, dropdown_y), ImVec2(combo_x + combo_w, dropdown_y + current_h),
                IM_COL32(50, 50, 50, (int)(255 * anim.open)), rounding);

            float max_scroll = content_h - (dropdown_h - S(6.0f));
            if (max_scroll < 0.0f) max_scroll = 0.0f;

            bool in_dropdown = is_open && (mouse.x >= combo_x && mouse.x <= combo_x + combo_w &&
                mouse.y >= dropdown_y && mouse.y <= dropdown_y + dropdown_h);

            if (in_dropdown && max_scroll > 0.0f)
            {
                float wheel = ImGui::GetIO().MouseWheel;
                if (wheel != 0.0f)
                {
                    anim.scroll_target -= wheel * item_height * 2.0f;
                    anim.scroll_target = Clamp(anim.scroll_target, 0.0f, max_scroll);
                }
            }

            anim.scroll_anim = Lerp(anim.scroll_anim, anim.scroll_target, dt * 14.0f);
            if (fabsf(anim.scroll_anim - anim.scroll_target) < 0.5f)
                anim.scroll_anim = anim.scroll_target;

            fg->PushClipRect(ImVec2(combo_x, dropdown_y), ImVec2(combo_x + combo_w, dropdown_y + current_h), true);

            for (int i = 0; i < items_count && i < 32; i++)
            {
                float item_y = dropdown_y + S(3.0f) + i * item_height - anim.scroll_anim;

                if (item_y + item_height < dropdown_y || item_y > dropdown_y + dropdown_h)
                {
                    anim.item_hovers[i] = Lerp(anim.item_hovers[i], 0.0f, dt * 12.0f);
                    continue;
                }

                bool item_hovered = is_open && (mouse.x >= combo_x && mouse.x <= combo_x + combo_w &&
                    mouse.y >= item_y && mouse.y <= item_y + item_height &&
                    mouse.y >= dropdown_y && mouse.y <= dropdown_y + dropdown_h);

                anim.item_hovers[i] = Lerp(anim.item_hovers[i], item_hovered ? 1.0f : 0.0f, dt * 12.0f);

                if (item_hovered && ImGui::IsMouseClicked(0))
                {
                    selected[i] = !selected[i];
                    changed = true;
                }

                float highlight = anim.item_hovers[i];
                bool is_on = selected[i];

                if (is_on && highlight < 0.3f) highlight = 0.3f;

                if (highlight > 0.01f)
                {
                    ImU32 hl_col = is_on
                        ? gui::colors::AccentBg(0.55f, (int)(180 * highlight * anim.open))
                        : gui::colors::AccentBg(0.45f, (int)(200 * highlight * anim.open));
                    fg->AddRectFilled(
                        ImVec2(combo_x + 2, item_y),
                        ImVec2(combo_x + combo_w - 2, item_y + item_height),
                        hl_col, rounding - 1
                    );
                }

                float cb_x = combo_x + S(6.0f);
                float cb_y = item_y + (item_height - S(10.0f)) * 0.5f;
                float cb_sz = S(10.0f);

                fg->AddRectFilled(ImVec2(cb_x, cb_y), ImVec2(cb_x + cb_sz, cb_y + cb_sz),
                    is_on ? gui::colors::AccentDimA((int)(255 * anim.open)) : IM_COL32(25, 25, 25, (int)(255 * anim.open)), 2.0f);
                fg->AddRect(ImVec2(cb_x, cb_y), ImVec2(cb_x + cb_sz, cb_y + cb_sz),
                    is_on ? gui::colors::AccentA((int)(255 * anim.open)) : IM_COL32(50, 50, 50, (int)(255 * anim.open)), 2.0f);

                if (is_on)
                {
                    float cx = cb_x + cb_sz * 0.5f;
                    float cy = cb_y + cb_sz * 0.5f;
                    float s = cb_sz * 0.25f;
                    fg->AddLine(ImVec2(cx - s * 1.2f, cy), ImVec2(cx - s * 0.2f, cy + s), IM_COL32(255, 255, 255, (int)(255 * anim.open)), S(1.5f));
                    fg->AddLine(ImVec2(cx - s * 0.2f, cy + s), ImVec2(cx + s * 1.4f, cy - s * 0.8f), IM_COL32(255, 255, 255, (int)(255 * anim.open)), S(1.5f));
                }

                float text_x = cb_x + cb_sz + S(6.0f);
                ImU32 text_base = is_on ? pal.accent : IM_COL32(160, 160, 160, 255);
                ImU32 text_hover = IM_COL32(255, 255, 255, 255);
                ImU32 text_col = LerpColorMC(text_base, text_hover, anim.item_hovers[i]);
                text_col = (text_col & 0x00FFFFFF) | ((int)(255 * anim.open) << 24);

                fg->AddText(ImVec2(text_x, item_y + (item_height - ImGui::GetTextLineHeight()) * 0.5f), text_col, items[i]);
            }

            fg->PopClipRect();

            if (max_scroll > 0.0f)
            {
                float sb_w = S(3.0f);
                float sb_x = combo_x + combo_w - sb_w - 2.0f;
                float sb_area = dropdown_h - S(6.0f);
                float vis_ratio = sb_area / content_h;
                float sb_h = sb_area * vis_ratio;
                if (sb_h < S(14.0f)) sb_h = S(14.0f);
                float sb_ratio = anim.scroll_anim / max_scroll;
                float sb_y = dropdown_y + S(3.0f) + (sb_area - sb_h) * sb_ratio;

                fg->AddRectFilled(ImVec2(sb_x, sb_y), ImVec2(sb_x + sb_w, sb_y + sb_h),
                    IM_COL32(60, 60, 60, (int)(200 * anim.open)), sb_w * 0.5f);
            }

            if (ImGui::IsMouseClicked(0) && !combo_hovered && !in_dropdown)
            {
                state_t.open_combo = 0;
            }
        }

        if (hint && hint[0] != '\0')
        {
            float qx = pos.x + width - S(10.0f);
            float qy = pos.y + (row_height - S(10.0f)) * 0.5f;
            DrawHintTooltip(ImGui::GetID(label), qx, qy, S(10.0f), S(12.0f), hint);
        }

        ImGui::PopID();
        return changed;
    }
}
