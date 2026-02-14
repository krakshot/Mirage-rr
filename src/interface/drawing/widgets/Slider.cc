#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include <cstdio>
#include <cmath>

namespace widgets
{
    std::unordered_map<ImGuiID, SliderAnim> slider_anims_t;

    static float EaseOutCubic(float t)
    {
        return 1.0f - powf(1.0f - t, 3.0f);
    }

    static float EaseOutQuad(float t)
    {
        return 1.0f - (1.0f - t) * (1.0f - t);
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

    bool SliderInt(const char* label, int* value, int v_min, int v_max, const char* hint, Keybind* bind)
    {
        ImGui::PushID(label);
        ImGuiID id = ImGui::GetID("##slider");

        SliderAnim& anim = slider_anims_t[id];

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float width = ImGui::GetContentRegionAvail().x;
        const float row_height = S(18.0f);
        const float label_width = S(65.0f);
        const float bar_height = S(8.0f);
        const float rounding = 2.0f;

        float bar_x = pos.x + label_width;
        float right_reserved = (hint && hint[0] ? S(14.0f) : 0.0f) + (bind ? S(32.0f) : 0.0f);
        float bar_w = width - label_width - right_reserved;
        float bar_y = pos.y + (row_height - bar_height) * 0.5f;

        ImGui::Dummy(ImVec2(width, row_height));

        ImVec2 mouse = ImGui::GetIO().MousePos;

        bool blocked_by_dropdown = false;
        if (state_t.open_combo != 0)
        {
            if (mouse.x >= state_t.dropdown_x1 && mouse.x <= state_t.dropdown_x2 &&
                mouse.y >= state_t.dropdown_y1 && mouse.y <= state_t.dropdown_y2)
            {
                blocked_by_dropdown = true;
            }
        }

        bool bar_hovered = !blocked_by_dropdown && (mouse.x >= bar_x - 4 && mouse.x <= bar_x + bar_w + 4 &&
            mouse.y >= bar_y - 4 && mouse.y <= bar_y + bar_height + 4);
        bool row_hovered = !blocked_by_dropdown && ImGui::IsItemHovered();

        bool is_active = (state_t.active_slider == id);
        bool changed = false;
        bool just_clicked = false;

        if (bar_hovered && ImGui::IsMouseClicked(0))
        {
            state_t.active_slider = id;
            is_active = true;
            just_clicked = true;
        }

        if (is_active)
        {
            if (ImGui::IsMouseDown(0))
            {
                float t = (mouse.x - bar_x) / bar_w;
                t = Clamp(t, 0.0f, 1.0f);
                int new_val = (int)(v_min + (v_max - v_min) * t + 0.5f);
                if (new_val != *value)
                {
                    *value = new_val;
                    changed = true;
                }
            }
            else
            {
                state_t.active_slider = 0;
                is_active = false;
            }
        }

        float target_fill = (v_max != v_min) ? (float)(*value - v_min) / (float)(v_max - v_min) : 0.0f;

        float dt = ImGui::GetIO().DeltaTime;
        float hover_target = (bar_hovered || row_hovered) ? 1.0f : 0.0f;
        float active_target = is_active ? 1.0f : 0.0f;

        anim.hover = Lerp(anim.hover, hover_target, dt * 10.0f);
        anim.active = Lerp(anim.active, active_target, dt * 12.0f);
        anim.fill = Lerp(anim.fill, target_fill, dt * 15.0f);
        anim.glow = Lerp(anim.glow, (is_active ? 1.0f : 0.0f), dt * 8.0f);

        float thumb_target = is_active ? 1.3f : (bar_hovered ? 1.1f : 1.0f);
        anim.thumb_scale = Lerp(anim.thumb_scale, thumb_target, dt * 12.0f);

        if (anim.fill < 0.001f) anim.fill = 0.0f;
        if (anim.fill > 0.999f) anim.fill = 1.0f;

        ImDrawList* draw = ImGui::GetWindowDrawList();

        const auto& pal = gui::colors::Get();
        ImU32 col_bar_bg = IM_COL32(18, 18, 18, 255);
        ImU32 col_bar_border = IM_COL32(40, 40, 40, 255);
        ImU32 col_fill_idle = IM_COL32(45, 45, 45, 255);
        ImU32 col_fill_hover = gui::colors::AccentBg(0.55f, 255);
        ImU32 col_fill_active = pal.accent_dim;
        ImU32 col_thumb_idle = IM_COL32(100, 100, 100, 255);
        ImU32 col_thumb_hover = pal.accent_dim;
        ImU32 col_thumb_active = pal.accent;
        ImU32 col_label_idle = IM_COL32(150, 150, 150, 255);
        ImU32 col_label_hover = IM_COL32(200, 200, 200, 255);
        ImU32 col_label_active = pal.accent;

        ImU32 fill_col = LerpColor(col_fill_idle, col_fill_hover, anim.hover);
        fill_col = LerpColor(fill_col, col_fill_active, anim.active);

        ImU32 thumb_col = LerpColor(col_thumb_idle, col_thumb_hover, anim.hover);
        thumb_col = LerpColor(thumb_col, col_thumb_active, anim.active);

        ImU32 label_col = LerpColor(col_label_idle, col_label_hover, anim.hover);
        label_col = LerpColor(label_col, col_label_active, anim.active);

        if (anim.glow > 0.01f)
        {
            ImU32 glow_col = gui::colors::AccentDimA((int)(30 * anim.glow));
            draw->AddRectFilled(
                ImVec2(bar_x - 2, bar_y - 2),
                ImVec2(bar_x + bar_w + 2, bar_y + bar_height + 2),
                glow_col, rounding + 2
            );
        }

        draw->AddRectFilled(
            ImVec2(bar_x, bar_y),
            ImVec2(bar_x + bar_w, bar_y + bar_height),
            col_bar_bg, rounding
        );

        ImU32 border_col = LerpColor(col_bar_border, pal.accent_dim, anim.active);
        draw->AddRect(
            ImVec2(bar_x, bar_y),
            ImVec2(bar_x + bar_w, bar_y + bar_height),
            border_col, rounding
        );

        float fill_w = bar_w * anim.fill;
        if (fill_w > 2.0f)
        {
            draw->AddRectFilled(
                ImVec2(bar_x + 1, bar_y + 1),
                ImVec2(bar_x + fill_w - 1, bar_y + bar_height - 1),
                fill_col, rounding - 1
            );
        }

        float thumb_x = bar_x + fill_w;
        float thumb_w = 4.0f * anim.thumb_scale;
        float thumb_h = (bar_height + 4.0f) * anim.thumb_scale;
        float thumb_y = bar_y + bar_height * 0.5f - thumb_h * 0.5f;

        if (thumb_x >= bar_x + thumb_w * 0.5f && thumb_x <= bar_x + bar_w - thumb_w * 0.5f)
        {
            if (anim.active > 0.1f)
            {
                ImU32 shadow_col = IM_COL32(0, 0, 0, (int)(60 * anim.active));
                draw->AddRectFilled(
                    ImVec2(thumb_x - thumb_w * 0.5f + 1, thumb_y + 1),
                    ImVec2(thumb_x + thumb_w * 0.5f + 1, thumb_y + thumb_h + 1),
                    shadow_col, 1.0f
                );
            }

            draw->AddRectFilled(
                ImVec2(thumb_x - thumb_w * 0.5f, thumb_y),
                ImVec2(thumb_x + thumb_w * 0.5f, thumb_y + thumb_h),
                thumb_col, 1.0f
            );
        }

        ImVec2 label_pos(pos.x, pos.y + (row_height - ImGui::GetTextLineHeight()) * 0.5f);
        DrawTextShadow(draw, label_pos, label_col, label);

        char value_buf[32];
        std::snprintf(value_buf, sizeof(value_buf), "%d", *value);
        ImVec2 val_size = ImGui::CalcTextSize(value_buf);
        float val_x = bar_x + bar_w - val_size.x - 4.0f;
        float val_y = bar_y + (bar_height - val_size.y) * 0.5f;

        ImU32 value_col = IM_COL32(255, 255, 255, (int)(180 + 75 * anim.active));
        DrawTextShadow(draw, ImVec2(val_x, val_y), value_col, value_buf);

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

    bool SliderFloat(const char* label, float* value, float v_min, float v_max, const char* format, const char* hint, Keybind* bind)
    {
        ImGui::PushID(label);
        ImGuiID id = ImGui::GetID("##sliderf");

        SliderAnim& anim = slider_anims_t[id];

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float width = ImGui::GetContentRegionAvail().x;
        const float row_height = S(18.0f);
        const float label_width = S(65.0f);
        const float bar_height = S(8.0f);
        const float rounding = 2.0f;

        float bar_x = pos.x + label_width;
        float right_reserved_f = (hint && hint[0] ? S(14.0f) : 0.0f) + (bind ? S(32.0f) : 0.0f);
        float bar_w = width - label_width - right_reserved_f;
        float bar_y = pos.y + (row_height - bar_height) * 0.5f;

        ImGui::Dummy(ImVec2(width, row_height));

        ImVec2 mouse = ImGui::GetIO().MousePos;

        bool blocked_by_dropdown = false;
        if (state_t.open_combo != 0)
        {
            if (mouse.x >= state_t.dropdown_x1 && mouse.x <= state_t.dropdown_x2 &&
                mouse.y >= state_t.dropdown_y1 && mouse.y <= state_t.dropdown_y2)
            {
                blocked_by_dropdown = true;
            }
        }

        bool bar_hovered = !blocked_by_dropdown && (mouse.x >= bar_x - 4 && mouse.x <= bar_x + bar_w + 4 &&
            mouse.y >= bar_y - 4 && mouse.y <= bar_y + bar_height + 4);
        bool row_hovered = !blocked_by_dropdown && ImGui::IsItemHovered();

        bool is_active = (state_t.active_slider == id);
        bool changed = false;

        if (bar_hovered && ImGui::IsMouseClicked(0))
        {
            state_t.active_slider = id;
            is_active = true;
        }

        if (is_active)
        {
            if (ImGui::IsMouseDown(0))
            {
                float t = (mouse.x - bar_x) / bar_w;
                t = Clamp(t, 0.0f, 1.0f);
                float new_val = v_min + (v_max - v_min) * t;
                if (new_val != *value)
                {
                    *value = new_val;
                    changed = true;
                }
            }
            else
            {
                state_t.active_slider = 0;
                is_active = false;
            }
        }

        float target_fill = (v_max != v_min) ? (*value - v_min) / (v_max - v_min) : 0.0f;

        float dt = ImGui::GetIO().DeltaTime;
        float hover_target = (bar_hovered || row_hovered) ? 1.0f : 0.0f;
        float active_target = is_active ? 1.0f : 0.0f;

        anim.hover = Lerp(anim.hover, hover_target, dt * 10.0f);
        anim.active = Lerp(anim.active, active_target, dt * 12.0f);
        anim.fill = Lerp(anim.fill, target_fill, dt * 15.0f);
        anim.glow = Lerp(anim.glow, (is_active ? 1.0f : 0.0f), dt * 8.0f);

        float thumb_target = is_active ? 1.3f : (bar_hovered ? 1.1f : 1.0f);
        anim.thumb_scale = Lerp(anim.thumb_scale, thumb_target, dt * 12.0f);

        if (anim.fill < 0.001f) anim.fill = 0.0f;
        if (anim.fill > 0.999f) anim.fill = 1.0f;

        ImDrawList* draw = ImGui::GetWindowDrawList();

        const auto& palf = gui::colors::Get();
        ImU32 col_bar_bg = IM_COL32(18, 18, 18, 255);
        ImU32 col_bar_border = IM_COL32(40, 40, 40, 255);
        ImU32 col_fill_idle = IM_COL32(45, 45, 45, 255);
        ImU32 col_fill_hover = gui::colors::AccentBg(0.55f, 255);
        ImU32 col_fill_active = palf.accent_dim;
        ImU32 col_thumb_idle = IM_COL32(100, 100, 100, 255);
        ImU32 col_thumb_hover = palf.accent_dim;
        ImU32 col_thumb_active = palf.accent;
        ImU32 col_label_idle = IM_COL32(150, 150, 150, 255);
        ImU32 col_label_hover = IM_COL32(200, 200, 200, 255);
        ImU32 col_label_active = palf.accent;

        ImU32 fill_col = LerpColor(col_fill_idle, col_fill_hover, anim.hover);
        fill_col = LerpColor(fill_col, col_fill_active, anim.active);

        ImU32 thumb_col = LerpColor(col_thumb_idle, col_thumb_hover, anim.hover);
        thumb_col = LerpColor(thumb_col, col_thumb_active, anim.active);

        ImU32 label_col = LerpColor(col_label_idle, col_label_hover, anim.hover);
        label_col = LerpColor(label_col, col_label_active, anim.active);

        if (anim.glow > 0.01f)
        {
            ImU32 glow_col = gui::colors::AccentDimA((int)(30 * anim.glow));
            draw->AddRectFilled(
                ImVec2(bar_x - 2, bar_y - 2),
                ImVec2(bar_x + bar_w + 2, bar_y + bar_height + 2),
                glow_col, rounding + 2
            );
        }

        draw->AddRectFilled(
            ImVec2(bar_x, bar_y),
            ImVec2(bar_x + bar_w, bar_y + bar_height),
            col_bar_bg, rounding
        );

        ImU32 border_col = LerpColor(col_bar_border, palf.accent_dim, anim.active);
        draw->AddRect(
            ImVec2(bar_x, bar_y),
            ImVec2(bar_x + bar_w, bar_y + bar_height),
            border_col, rounding
        );

        float fill_w = bar_w * anim.fill;
        if (fill_w > 2.0f)
        {
            draw->AddRectFilled(
                ImVec2(bar_x + 1, bar_y + 1),
                ImVec2(bar_x + fill_w - 1, bar_y + bar_height - 1),
                fill_col, rounding - 1
            );
        }

        float thumb_x = bar_x + fill_w;
        float thumb_w = 4.0f * anim.thumb_scale;
        float thumb_h = (bar_height + 4.0f) * anim.thumb_scale;
        float thumb_y = bar_y + bar_height * 0.5f - thumb_h * 0.5f;

        if (thumb_x >= bar_x + thumb_w * 0.5f && thumb_x <= bar_x + bar_w - thumb_w * 0.5f)
        {
            if (anim.active > 0.1f)
            {
                ImU32 shadow_col = IM_COL32(0, 0, 0, (int)(60 * anim.active));
                draw->AddRectFilled(
                    ImVec2(thumb_x - thumb_w * 0.5f + 1, thumb_y + 1),
                    ImVec2(thumb_x + thumb_w * 0.5f + 1, thumb_y + thumb_h + 1),
                    shadow_col, 1.0f
                );
            }

            draw->AddRectFilled(
                ImVec2(thumb_x - thumb_w * 0.5f, thumb_y),
                ImVec2(thumb_x + thumb_w * 0.5f, thumb_y + thumb_h),
                thumb_col, 1.0f
            );
        }

        ImVec2 label_pos(pos.x, pos.y + (row_height - ImGui::GetTextLineHeight()) * 0.5f);
        DrawTextShadow(draw, label_pos, label_col, label);

        char value_buf[32];
        std::snprintf(value_buf, sizeof(value_buf), format, *value);
        ImVec2 val_size = ImGui::CalcTextSize(value_buf);
        float val_x = bar_x + bar_w - val_size.x - 4.0f;
        float val_y = bar_y + (bar_height - val_size.y) * 0.5f;

        ImU32 value_col = IM_COL32(255, 255, 255, (int)(180 + 75 * anim.active));
        DrawTextShadow(draw, ImVec2(val_x, val_y), value_col, value_buf);

        float right_edge_f = pos.x + width;
        if (hint && hint[0] != '\0')
        {
            float qx = right_edge_f - S(10.0f);
            float qy = pos.y + (row_height - S(10.0f)) * 0.5f;
            DrawHintTooltip(ImGui::GetID(label), qx, qy, S(10.0f), S(12.0f), hint);
            right_edge_f -= S(14.0f);
        }
        if (bind)
        {
            float kb_x = right_edge_f - S(28.0f);
            DrawKeybind(bind, kb_x, pos.y, S(28.0f));
        }

        ImGui::PopID();
        return changed;
    }
}
