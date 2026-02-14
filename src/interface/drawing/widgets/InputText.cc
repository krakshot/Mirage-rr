#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include <imgui_internal.h>
#include <cmath>

namespace widgets
{
    std::unordered_map<ImGuiID, InputTextAnim> inputtext_anims_t;

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

    bool InputText(const char* label, char* buf, size_t buf_size, const char* hint)
    {
        ImGui::PushID(label);
        ImGuiID id = ImGui::GetID("##inputtext");

        InputTextAnim& anim = inputtext_anims_t[id];

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float width = ImGui::GetContentRegionAvail().x;
        const float row_height = S(18.0f);
        const float label_width = S(65.0f);
        const float input_height = S(16.0f);
        const float rounding = 2.0f;

        float input_x = pos.x + label_width;
        float input_w = width - label_width - (hint && hint[0] ? S(14.0f) : 0.0f);
        float input_y = pos.y + (row_height - input_height) * 0.5f;

        ImDrawList* draw = ImGui::GetWindowDrawList();

        ImGui::SetCursorScreenPos(ImVec2(input_x, input_y));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(S(5), (input_height - ImGui::GetTextLineHeight()) * 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

        float dt = ImGui::GetIO().DeltaTime;

        bool was_focused = (ImGui::GetActiveID() == ImGui::GetID("##input"));

        ImVec2 mouse = ImGui::GetIO().MousePos;
        bool input_hovered = (mouse.x >= input_x && mouse.x <= input_x + input_w &&
            mouse.y >= input_y && mouse.y <= input_y + input_height);

        bool blocked_by_dropdown = false;
        if (state_t.open_combo != 0)
        {
            if (mouse.x >= state_t.dropdown_x1 && mouse.x <= state_t.dropdown_x2 &&
                mouse.y >= state_t.dropdown_y1 && mouse.y <= state_t.dropdown_y2)
            {
                blocked_by_dropdown = true;
            }
        }
        if (blocked_by_dropdown) input_hovered = false;

        float hover_target = input_hovered ? 1.0f : 0.0f;
        float focus_target = was_focused ? 1.0f : 0.0f;
        float glow_target = was_focused ? 1.0f : (input_hovered ? 0.4f : 0.0f);

        anim.hover = Lerp(anim.hover, hover_target, dt * 12.0f);
        anim.focus = Lerp(anim.focus, focus_target, dt * 10.0f);
        anim.glow = Lerp(anim.glow, glow_target, dt * 8.0f);

        if (was_focused)
        {
            anim.cursor_blink += dt * 4.0f;
            if (anim.cursor_blink > 6.28318f) anim.cursor_blink -= 6.28318f;
        }
        else
        {
            anim.cursor_blink = 0.0f;
        }

        const auto& pal = gui::colors::Get();
        ImU32 col_bg_idle = IM_COL32(16, 16, 16, 255);
        ImU32 col_bg_hover = IM_COL32(20, 20, 20, 255);
        ImU32 col_bg_focus = IM_COL32(22, 22, 24, 255);
        ImU32 col_border_idle = IM_COL32(38, 38, 38, 255);
        ImU32 col_border_hover = IM_COL32(55, 55, 55, 255);
        ImU32 col_border_focus = pal.accent_dim;
        ImU32 col_text_idle = IM_COL32(175, 175, 175, 255);
        ImU32 col_text_focus = IM_COL32(240, 240, 240, 255);

        ImU32 bg_col = LerpColor(col_bg_idle, col_bg_hover, anim.hover);
        bg_col = LerpColor(bg_col, col_bg_focus, anim.focus);

        ImU32 border_col = LerpColor(col_border_idle, col_border_hover, anim.hover);
        border_col = LerpColor(border_col, col_border_focus, anim.focus);

        ImU32 text_col = LerpColor(col_text_idle, col_text_focus, anim.focus);

        ImU32 col_label_idle = IM_COL32(150, 150, 150, 255);
        ImU32 col_label_hover = IM_COL32(200, 200, 200, 255);
        ImU32 col_label_focus = pal.accent;
        ImU32 label_col = LerpColor(col_label_idle, col_label_hover, anim.hover);
        label_col = LerpColor(label_col, col_label_focus, anim.focus);

        ImVec4 bg_vec = ImGui::ColorConvertU32ToFloat4(bg_col);
        ImVec4 border_vec = ImGui::ColorConvertU32ToFloat4(border_col);
        ImVec4 text_vec = ImGui::ColorConvertU32ToFloat4(text_col);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, bg_vec);
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, bg_vec);
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImGui::ColorConvertU32ToFloat4(col_bg_focus));
        ImGui::PushStyleColor(ImGuiCol_Border, border_vec);
        ImGui::PushStyleColor(ImGuiCol_Text, text_vec);
        ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ImGui::ColorConvertU32ToFloat4(gui::colors::AccentBg(0.55f, 77)));
        ImGui::PushStyleColor(ImGuiCol_NavHighlight, ImVec4(0, 0, 0, 0));

        ImGui::SetNextItemWidth(input_w);
        bool changed = ImGui::InputText("##input", buf, buf_size, ImGuiInputTextFlags_AutoSelectAll);

        bool is_focused = ImGui::IsItemActive();

        ImGui::PopStyleColor(7);
        ImGui::PopStyleVar(3);

        if (is_focused)
        {
            ImGuiContext& g = *GImGui;
            ImGuiInputTextState* state = ImGui::GetInputTextState(ImGui::GetActiveID());
            if (state)
            {
                ImVec2 cursor_pos = ImGui::GetCursorScreenPos();

                const char* text_begin = buf;
                int cursor_idx = state->GetCursorPos();

                char saved = buf[cursor_idx];
                buf[cursor_idx] = '\0';
                ImVec2 text_sz = ImGui::CalcTextSize(text_begin);
                buf[cursor_idx] = saved;

                float cursor_screen_x = input_x + S(5.0f) + text_sz.x;
                float cursor_screen_y = input_y + S(2.0f);
                float cursor_h = input_height - S(4.0f);

                float blink_alpha = (sinf(anim.cursor_blink) + 1.0f) * 0.5f;
                blink_alpha = blink_alpha * blink_alpha;

                ImU32 cursor_col = gui::colors::AccentBright((int)(255 * blink_alpha));
                draw->AddRectFilled(
                    ImVec2(cursor_screen_x, cursor_screen_y),
                    ImVec2(cursor_screen_x + 1.0f, cursor_screen_y + cursor_h),
                    cursor_col
                );
            }
        }

        if (!is_focused && buf[0] == '\0')
        {
            ImU32 placeholder_col = IM_COL32(60, 60, 60, (int)(200 * (1.0f - anim.focus)));
            draw->AddText(
                ImVec2(input_x + S(5.0f), input_y + (input_height - ImGui::GetTextLineHeight()) * 0.5f),
                placeholder_col, "Type here..."
            );
        }

        ImVec2 label_pos(pos.x, pos.y + (row_height - ImGui::GetTextLineHeight()) * 0.5f);
        DrawTextShadow(draw, label_pos, label_col, label);

        if (anim.glow > 0.01f)
        {
            ImU32 glow_col = gui::colors::AccentDimA((int)(25 * anim.glow));
            draw->AddRectFilled(
                ImVec2(input_x - 2, input_y - 2),
                ImVec2(input_x + input_w + 2, input_y + input_height + 2),
                glow_col, rounding + 2
            );
        }

        if (anim.focus > 0.01f)
        {
            float line_w = input_w * anim.focus;
            float line_x = input_x + (input_w - line_w) * 0.5f;
            ImU32 line_col = gui::colors::AccentDimA((int)(200 * anim.focus));
            draw->AddRectFilled(
                ImVec2(line_x, input_y + input_height),
                ImVec2(line_x + line_w, input_y + input_height + 1),
                line_col, 0.5f
            );
        }

        ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + row_height + S(2.0f)));

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
