#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include <cmath>

namespace widgets
{
    std::unordered_map<ImGuiID, ButtonAnim> button_anims_t;

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

    bool Button(const char* label, float width, float height)
    {
        ImGui::PushID(label);
        ImGuiID id = ImGui::GetID("##button");

        ButtonAnim& anim = button_anims_t[id];

        if (width <= 0.0f)
            width = ImGui::GetContentRegionAvail().x;
        if (height <= 0.0f)
            height = S(20.0f);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float center_x = pos.x + width * 0.5f;
        float center_y = pos.y + height * 0.5f;
        const float rounding = 2.0f;

        ImGui::InvisibleButton("##btn", ImVec2(width, height));

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
        bool held = !blocked_by_dropdown && ImGui::IsItemActive();
        bool pressed = !blocked_by_dropdown && ImGui::IsItemClicked();

        if (pressed)
        {
            anim.click = 1.0f;
        }

        float dt = ImGui::GetIO().DeltaTime;
        float hover_target = hovered ? 1.0f : 0.0f;
        float press_target = held ? 1.0f : 0.0f;
        float glow_target = held ? 1.0f : (hovered ? 0.5f : 0.0f);
        float scale_target = held ? 0.97f : 1.0f;

        anim.hover = Lerp(anim.hover, hover_target, dt * 12.0f);
        anim.press = Lerp(anim.press, press_target, dt * 18.0f);
        anim.glow = Lerp(anim.glow, glow_target, dt * 10.0f);
        anim.scale = Lerp(anim.scale, scale_target, dt * 16.0f);

        if (anim.click > 0.0f)
        {
            anim.click -= dt * 4.0f;
            if (anim.click < 0.0f) anim.click = 0.0f;
        }

        ImDrawList* draw = ImGui::GetWindowDrawList();

        float scaled_w = width * anim.scale;
        float scaled_h = height * anim.scale;
        float box_x1 = center_x - scaled_w * 0.5f;
        float box_y1 = center_y - scaled_h * 0.5f;
        float box_x2 = center_x + scaled_w * 0.5f;
        float box_y2 = center_y + scaled_h * 0.5f;

        const auto& pal = gui::colors::Get();
        ImU32 col_bg_idle = IM_COL32(18, 18, 18, 255);
        ImU32 col_bg_hover = IM_COL32(25, 25, 25, 255);
        ImU32 col_bg_press = gui::colors::AccentBg(0.55f, 255);
        ImU32 col_border_idle = IM_COL32(40, 40, 40, 255);
        ImU32 col_border_hover = IM_COL32(60, 60, 60, 255);
        ImU32 col_border_press = pal.accent_dim;
        ImU32 col_text_idle = IM_COL32(150, 150, 150, 255);
        ImU32 col_text_hover = IM_COL32(220, 220, 220, 255);
        ImU32 col_text_press = IM_COL32(255, 255, 255, 255);

        ImU32 bg_col = LerpColor(col_bg_idle, col_bg_hover, anim.hover);
        bg_col = LerpColor(bg_col, col_bg_press, anim.press);

        ImU32 border_col = LerpColor(col_border_idle, col_border_hover, anim.hover);
        border_col = LerpColor(border_col, col_border_press, anim.press);

        ImU32 text_col = LerpColor(col_text_idle, col_text_hover, anim.hover);
        text_col = LerpColor(text_col, col_text_press, anim.press);

        if (anim.glow > 0.01f)
        {
            ImU32 glow_col = gui::colors::AccentDimA((int)(30 * anim.glow));
            draw->AddRectFilled(
                ImVec2(box_x1 - 2, box_y1 - 2),
                ImVec2(box_x2 + 2, box_y2 + 2),
                glow_col, rounding + 2
            );
        }

        draw->AddRectFilled(ImVec2(box_x1, box_y1), ImVec2(box_x2, box_y2), bg_col, rounding);

        if (anim.click > 0.01f)
        {
            float ripple_progress = 1.0f - anim.click;
            float ripple_radius = (scaled_w > scaled_h ? scaled_w : scaled_h) * 0.8f * EaseOutQuad(ripple_progress);
            int ripple_alpha = (int)(100 * anim.click);
            ImU32 ripple_col = gui::colors::AccentA(ripple_alpha);

            draw->PushClipRect(ImVec2(box_x1, box_y1), ImVec2(box_x2, box_y2), true);
            draw->AddCircleFilled(ImVec2(center_x, center_y), ripple_radius, ripple_col, 32);
            draw->PopClipRect();
        }

        draw->AddRect(ImVec2(box_x1, box_y1), ImVec2(box_x2, box_y2), border_col, rounding);

        if (anim.hover > 0.01f && anim.press < 0.5f)
        {
            ImU32 inner_highlight = IM_COL32(255, 255, 255, (int)(15 * anim.hover * (1.0f - anim.press)));
            draw->AddRect(
                ImVec2(box_x1 + 1, box_y1 + 1),
                ImVec2(box_x2 - 1, box_y2 - 1),
                inner_highlight, rounding - 1);
        }

        ImVec2 text_size = ImGui::CalcTextSize(label);
        float text_scale = 1.0f + (anim.press * 0.02f);
        float text_x = center_x - (text_size.x * text_scale) * 0.5f;
        float text_y = center_y - (text_size.y * text_scale) * 0.5f;

        draw->AddText(ImVec2(text_x + 1, text_y + 1), IM_COL32(0, 0, 0, 120), label);
        draw->AddText(ImVec2(text_x, text_y), text_col, label);

        ImGui::PopID();
        return pressed;
    }
}
