#include <imgui_internal.h>
#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include <cmath>
#include <cstdio>
#include <cstring>

namespace widgets
{
    std::unordered_map<ImGuiID, ColorEditAnim> coloredit_anims_t;

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

    bool ColorEdit(const char* label, float col[4], const char* hint, Keybind* bind)
    {
        ImGui::PushID(label);
        ImGuiID id = ImGui::GetID("##coloredit");

        ColorEditAnim& anim = coloredit_anims_t[id];

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float width = ImGui::GetContentRegionAvail().x;
        const float row_height = S(18.0f);
        const float label_width = S(65.0f);
        const float color_height = S(14.0f);
        const float color_width = S(45.0f);
        const float rounding = 2.0f;

        float color_x = pos.x + label_width;
        float color_y = pos.y + (row_height - color_height) * 0.5f;
        float center_x = color_x + color_width * 0.5f;
        float center_y = color_y + color_height * 0.5f;

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

        bool color_hovered = !blocked_by_dropdown && (mouse.x >= color_x && mouse.x <= color_x + color_width &&
            mouse.y >= color_y && mouse.y <= color_y + color_height);
        bool row_hovered = !blocked_by_dropdown && ImGui::IsItemHovered();
        bool is_open = ImGui::IsPopupOpen("##colorpicker");

        float dt = ImGui::GetIO().DeltaTime;
        float hover_target = (color_hovered || row_hovered) ? 1.0f : 0.0f;
        float open_target = is_open ? 1.0f : 0.0f;
        float glow_target = is_open ? 1.0f : (color_hovered ? 0.5f : 0.0f);
        float scale_target = is_open ? 1.08f : (color_hovered ? 1.04f : 1.0f);

        anim.hover = Lerp(anim.hover, hover_target, dt * 10.0f);
        anim.open = Lerp(anim.open, open_target, dt * 12.0f);
        anim.glow = Lerp(anim.glow, glow_target, dt * 8.0f);
        anim.scale = Lerp(anim.scale, scale_target, dt * 14.0f);

        if (is_open)
        {
            anim.pulse += dt * 3.0f;
            if (anim.pulse > 6.28318f) anim.pulse -= 6.28318f;
        }
        else
        {
            anim.pulse = Lerp(anim.pulse, 0.0f, dt * 5.0f);
        }

        ImDrawList* draw = ImGui::GetWindowDrawList();

        const auto& pal = gui::colors::Get();
        ImU32 col_label_idle = IM_COL32(150, 150, 150, 255);
        ImU32 col_label_hover = IM_COL32(200, 200, 200, 255);
        ImU32 col_label_open = pal.accent;

        ImU32 label_col = LerpColor(col_label_idle, col_label_hover, anim.hover);
        label_col = LerpColor(label_col, col_label_open, anim.open);

        ImVec2 label_pos(pos.x, pos.y + (row_height - ImGui::GetTextLineHeight()) * 0.5f);
        DrawTextShadow(draw, label_pos, label_col, label);

        float scaled_w = color_width * anim.scale;
        float scaled_h = color_height * anim.scale;
        float box_x1 = center_x - scaled_w * 0.5f;
        float box_y1 = center_y - scaled_h * 0.5f;
        float box_x2 = center_x + scaled_w * 0.5f;
        float box_y2 = center_y + scaled_h * 0.5f;

        if (anim.glow > 0.01f)
        {
            float pulse_offset = sinf(anim.pulse) * 0.15f;
            float glow_intensity = anim.glow + pulse_offset;
            glow_intensity = Clamp(glow_intensity, 0.0f, 1.0f);

            ImU32 glow_col = IM_COL32(
                (int)(col[0] * 255 * 0.6f),
                (int)(col[1] * 255 * 0.6f),
                (int)(col[2] * 255 * 0.6f),
                (int)(80 * glow_intensity)
            );
            draw->AddRectFilled(
                ImVec2(box_x1 - 3, box_y1 - 3),
                ImVec2(box_x2 + 3, box_y2 + 3),
                glow_col, rounding + 3
            );
        }

        float checker_size = S(4.0f);
        draw->PushClipRect(ImVec2(box_x1, box_y1), ImVec2(box_x2, box_y2), true);
        for (float cy = box_y1; cy < box_y2; cy += checker_size)
        {
            for (float cx = box_x1; cx < box_x2; cx += checker_size)
            {
                int xi = (int)((cx - box_x1) / checker_size);
                int yi = (int)((cy - box_y1) / checker_size);
                ImU32 checker_col = ((xi + yi) % 2 == 0) ? IM_COL32(35, 35, 35, 255) : IM_COL32(55, 55, 55, 255);
                draw->AddRectFilled(
                    ImVec2(cx, cy),
                    ImVec2(cx + checker_size, cy + checker_size),
                    checker_col);
            }
        }
        draw->PopClipRect();

        ImU32 col_with_alpha = ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], col[3]));
        draw->AddRectFilled(ImVec2(box_x1, box_y1), ImVec2(box_x2, box_y2), col_with_alpha, rounding);

        ImU32 border_idle = IM_COL32(45, 45, 45, 255);
        ImU32 border_hover = IM_COL32(70, 70, 70, 255);
        ImU32 border_open = pal.accent_dim;
        ImU32 border_col = LerpColor(border_idle, border_hover, anim.hover);
        border_col = LerpColor(border_col, border_open, anim.open);
        draw->AddRect(ImVec2(box_x1, box_y1), ImVec2(box_x2, box_y2), border_col, rounding);

        if (anim.hover > 0.01f)
        {
            ImU32 inner_highlight = IM_COL32(255, 255, 255, (int)(20 * anim.hover));
            draw->AddRect(
                ImVec2(box_x1 + 1, box_y1 + 1),
                ImVec2(box_x2 - 1, box_y2 - 1),
                inner_highlight, rounding - 1);
        }

        if (anim.hover > 0.1f)
        {
            char hex_buf[16];
            snprintf(hex_buf, sizeof(hex_buf), "#%02X%02X%02X",
                (int)(col[0] * 255),
                (int)(col[1] * 255),
                (int)(col[2] * 255));

            float text_alpha = EaseOutQuad(anim.hover);
            ImU32 hex_col = IM_COL32(140, 140, 140, (int)(200 * text_alpha));

            ImVec2 text_size = ImGui::CalcTextSize(hex_buf);
            float hex_x = center_x - text_size.x * 0.5f;
            float hex_y = box_y2 + 2.0f;

            draw->AddText(ImVec2(hex_x, hex_y), hex_col, hex_buf);
        }

        if (color_hovered && ImGui::IsMouseClicked(0))
        {
            ImGui::OpenPopup("##colorpicker");
        }

        bool changed = false;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 6));
        ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(12, 12, 12, 250));
        ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(50, 50, 50, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(20, 20, 20, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(30, 30, 30, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(35, 35, 35, 255));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, pal.accent_dim);
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, pal.accent);
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(25, 25, 25, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(40, 40, 40, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, gui::colors::AccentBg(0.55f, 255));

        if (ImGui::BeginPopup("##colorpicker"))
        {
            changed = ImGui::ColorPicker4("##picker", col,
                ImGuiColorEditFlags_AlphaBar |
                ImGuiColorEditFlags_NoSidePreview |
                ImGuiColorEditFlags_NoSmallPreview |
                ImGuiColorEditFlags_DisplayRGB |
                ImGuiColorEditFlags_PickerHueWheel);

            ImGui::Separator();

            static char hex_input[16] = "";
            char current_hex[16];
            snprintf(current_hex, sizeof(current_hex), "#%02X%02X%02X%02X",
                (int)(col[0] * 255), (int)(col[1] * 255),
                (int)(col[2] * 255), (int)(col[3] * 255));

            if (!ImGui::IsItemActive())
            {
                strncpy(hex_input, current_hex, sizeof(hex_input));
            }

            ImGui::Text("Hex:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(90);
            if (ImGui::InputText("##hex", hex_input, sizeof(hex_input), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                unsigned int r = 255, g = 255, b = 255, a = 255;
                const char* hex = hex_input;
                if (hex[0] == '#') hex++;

                int len = (int)strlen(hex);
                if (len == 6)
                {
                    sscanf(hex, "%02x%02x%02x", &r, &g, &b);
                    col[0] = r / 255.0f;
                    col[1] = g / 255.0f;
                    col[2] = b / 255.0f;
                    changed = true;
                }
                else if (len == 8)
                {
                    sscanf(hex, "%02x%02x%02x%02x", &r, &g, &b, &a);
                    col[0] = r / 255.0f;
                    col[1] = g / 255.0f;
                    col[2] = b / 255.0f;
                    col[3] = a / 255.0f;
                    changed = true;
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Copy", ImVec2(40, 0)))
            {
                char copy_buf[32];
                snprintf(copy_buf, sizeof(copy_buf), "#%02X%02X%02X%02X",
                    (int)(col[0] * 255), (int)(col[1] * 255),
                    (int)(col[2] * 255), (int)(col[3] * 255));
                ImGui::SetClipboardText(copy_buf);
            }

            ImGui::SameLine();
            if (ImGui::Button("Paste", ImVec2(40, 0)))
            {
                const char* clip = ImGui::GetClipboardText();
                if (clip && clip[0])
                {
                    unsigned int r = 255, g = 255, b = 255, a = 255;
                    const char* hex = clip;
                    if (hex[0] == '#') hex++;

                    int len = (int)strlen(hex);
                    if (len >= 6)
                    {
                        if (len == 6)
                        {
                            sscanf(hex, "%02x%02x%02x", &r, &g, &b);
                        }
                        else if (len >= 8)
                        {
                            sscanf(hex, "%02x%02x%02x%02x", &r, &g, &b, &a);
                        }
                        col[0] = r / 255.0f;
                        col[1] = g / 255.0f;
                        col[2] = b / 255.0f;
                        col[3] = a / 255.0f;
                        changed = true;
                    }
                }
            }

            ImGui::EndPopup();
        }

        ImGui::PopStyleColor(10);
        ImGui::PopStyleVar(3);

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
