#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include <cmath>

namespace widgets
{
    std::unordered_map<ImGuiID, CheckboxAnim> checkbox_anims_t;

    static float EaseOutBack(float t)
    {
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;
        return 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
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

    bool Checkbox(const char* label, bool* value, const char* hint, Keybind* bind)
    {
        ImGui::PushID(label);
        ImGuiID id = ImGui::GetID("##checkbox");

        CheckboxAnim& anim = checkbox_anims_t[id];

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float width = ImGui::GetContentRegionAvail().x;
        const float row_height = S(18.0f);
        const float box_size = S(12.0f);
        const float rounding = 2.0f;

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

        bool hovered = !blocked_by_dropdown && ImGui::IsItemHovered();
        bool clicked = !blocked_by_dropdown && ImGui::IsMouseClicked(0) && hovered;

        if (clicked)
        {
            *value = !*value;
            anim.click = 1.0f;
        }

        if (bind && bind->key != ImGuiKey_None && !bind->waiting)
        {
            if (bind->mode == KeybindMode::Toggle)
            {
                if (ImGui::IsKeyPressed(bind->key, false))
                {
                    *value = !*value;
                    anim.click = 1.0f;
                    clicked = true;
                }
            }
            else
            {
                bool held = ImGui::IsKeyDown(bind->key);
                if (held != *value)
                {
                    *value = held;
                    if (held) anim.click = 1.0f;
                    clicked = true;
                }
            }
        }

        float dt = ImGui::GetIO().DeltaTime;
        float check_target = *value ? 1.0f : 0.0f;
        float hover_target = hovered ? 1.0f : 0.0f;

        float check_speed = *value ? 12.0f : 8.0f;
        anim.check = Lerp(anim.check, check_target, dt * check_speed);
        anim.hover = Lerp(anim.hover, hover_target, dt * 10.0f);
        anim.click = Lerp(anim.click, 0.0f, dt * 6.0f);

        if (anim.check < 0.01f) anim.check = 0.0f;
        if (anim.check > 0.99f) anim.check = 1.0f;

        ImDrawList* draw = ImGui::GetWindowDrawList();

        float box_x = pos.x;
        float box_y = pos.y + (row_height - box_size) * 0.5f;

        const auto& pal = gui::colors::Get();
        ImU32 col_bg_off = IM_COL32(18, 18, 18, 255);
        ImU32 col_bg_on = pal.accent;
        ImU32 col_bg_hover = gui::colors::AccentA(255);
        ImU32 col_border_off = IM_COL32(50, 50, 50, 255);
        ImU32 col_border_on = pal.accent_dim;

        ImU32 bg_base = LerpColor(col_bg_off, col_bg_on, anim.check);
        ImU32 bg_final = LerpColor(bg_base, col_bg_hover, anim.hover * anim.check);
        ImU32 border_col = LerpColor(col_border_off, col_border_on, anim.check);

        if (anim.hover > 0.01f && anim.check < 0.5f)
        {
            bg_final = LerpColor(bg_final, IM_COL32(30, 30, 30, 255), anim.hover);
            border_col = LerpColor(border_col, IM_COL32(70, 70, 70, 255), anim.hover);
        }

        float scale = 1.0f + anim.click * 0.1f;
        float scaled_size = box_size * scale;
        float offset = (scaled_size - box_size) * 0.5f;

        if (anim.check > 0.01f)
        {
            ImU32 shadow_col = gui::colors::AccentDimA((int)(40 * anim.check));
            draw->AddRectFilled(
                ImVec2(box_x - offset + 1, box_y - offset + 2),
                ImVec2(box_x + scaled_size - offset + 1, box_y + scaled_size - offset + 2),
                shadow_col, rounding
            );
        }

        draw->AddRectFilled(
            ImVec2(box_x - offset, box_y - offset),
            ImVec2(box_x + scaled_size - offset, box_y + scaled_size - offset),
            bg_final, rounding
        );

        draw->AddRect(
            ImVec2(box_x - offset, box_y - offset),
            ImVec2(box_x + scaled_size - offset, box_y + scaled_size - offset),
            border_col, rounding, 0, 1.0f
        );

        if (anim.check > 0.01f)
        {
            float check_alpha = Clamp(anim.check * 1.5f, 0.0f, 1.0f);
            float check_scale = EaseOutBack(Clamp(anim.check, 0.0f, 1.0f));

            float cx = box_x + box_size * 0.5f;
            float cy = box_y + box_size * 0.5f;

            float p1x = cx - S(2.5f) * check_scale;
            float p1y = cy + 0.0f * check_scale;
            float p2x = cx - S(0.5f) * check_scale;
            float p2y = cy + S(2.5f) * check_scale;
            float p3x = cx + S(3.0f) * check_scale;
            float p3y = cy - S(2.0f) * check_scale;

            ImU32 check_col = IM_COL32(255, 255, 255, (int)(255 * check_alpha));

            ImU32 check_shadow = IM_COL32(0, 0, 0, (int)(80 * check_alpha));
            draw->AddLine(ImVec2(p1x + 0.5f, p1y + 0.5f), ImVec2(p2x + 0.5f, p2y + 0.5f), check_shadow, S(2.0f));
            draw->AddLine(ImVec2(p2x + 0.5f, p2y + 0.5f), ImVec2(p3x + 0.5f, p3y + 0.5f), check_shadow, S(2.0f));

            draw->AddLine(ImVec2(p1x, p1y), ImVec2(p2x, p2y), check_col, S(1.8f));
            draw->AddLine(ImVec2(p2x, p2y), ImVec2(p3x, p3y), check_col, S(1.8f));
        }

        float label_alpha = Lerp(0.5f, 1.0f, Clamp(anim.check + anim.hover * 0.5f, 0.0f, 1.0f));
        ImU32 text_col = IM_COL32(200, 200, 200, (int)(255 * label_alpha));
        if (*value)
            text_col = LerpColor(IM_COL32(200, 200, 200, 255), IM_COL32(255, 255, 255, 255), anim.hover);

        ImVec2 text_pos(box_x + box_size + S(8.0f), pos.y + (row_height - ImGui::GetTextLineHeight()) * 0.5f);
        DrawTextShadow(draw, text_pos, text_col, label);

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
            RegisterKeybind(label, bind);
            float kb_x = right_edge - S(28.0f);
            DrawKeybind(bind, kb_x, pos.y, S(28.0f));
        }

        ImGui::PopID();
        return clicked;
    }
}
