#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include <cmath>

namespace widgets
{
    std::unordered_map<ImGuiID, ListboxAnim> listbox_anims_t;

    static float EaseOutQuad(float t)
    {
        return 1.0f - (1.0f - t) * (1.0f - t);
    }

    static float EaseOutCubic(float t)
    {
        return 1.0f - powf(1.0f - t, 3.0f);
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

    bool Listbox(const char* label, int* current, const char* const* items, int items_count, int visible_count, const char* hint)
    {
        ImGui::PushID(label);
        ImGuiID id = ImGui::GetID("##listbox");

        ListboxAnim& anim = listbox_anims_t[id];

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float width = ImGui::GetContentRegionAvail().x;
        const float label_height = S(16.0f);
        const float item_height = S(18.0f);
        const float rounding = 2.0f;
        const float padding = S(3.0f);

        int vis = visible_count;
        if (vis > items_count) vis = items_count;
        if (vis < 1) vis = 1;

        float list_height = vis * item_height + padding * 2.0f;
        float total_height = label_height + S(2.0f) + list_height;
        float list_w = width - (hint && hint[0] ? S(14.0f) : 0.0f);
        float list_x = pos.x;
        float list_y = pos.y + label_height + S(2.0f);

        ImGui::Dummy(ImVec2(width, total_height));

        ImVec2 mouse = ImGui::GetIO().MousePos;
        float dt = ImGui::GetIO().DeltaTime;

        bool blocked_by_dropdown = false;
        if (state_t.open_combo != 0)
        {
            if (mouse.x >= state_t.dropdown_x1 && mouse.x <= state_t.dropdown_x2 &&
                mouse.y >= state_t.dropdown_y1 && mouse.y <= state_t.dropdown_y2)
            {
                blocked_by_dropdown = true;
            }
        }

        bool list_hovered = !blocked_by_dropdown &&
            (mouse.x >= list_x && mouse.x <= list_x + list_w &&
                mouse.y >= list_y && mouse.y <= list_y + list_height);

        float hover_target = list_hovered ? 1.0f : 0.0f;
        float glow_target = list_hovered ? 0.6f : 0.0f;

        anim.hover = Lerp(anim.hover, hover_target, dt * 10.0f);
        anim.glow = Lerp(anim.glow, glow_target, dt * 8.0f);

        anim.select_flash = Lerp(anim.select_flash, 0.0f, dt * 6.0f);
        if (anim.select_flash < 0.01f) anim.select_flash = 0.0f;

        float content_height = items_count * item_height;
        float max_scroll = content_height - (list_height - padding * 2.0f);
        if (max_scroll < 0.0f) max_scroll = 0.0f;

        if (list_hovered && !blocked_by_dropdown)
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

        ImDrawList* draw = ImGui::GetWindowDrawList();

        const auto& pal = gui::colors::Get();
        ImU32 col_bg_idle = IM_COL32(16, 16, 16, 255);
        ImU32 col_bg_hover = IM_COL32(20, 20, 20, 255);
        ImU32 col_border_idle = IM_COL32(38, 38, 38, 255);
        ImU32 col_border_hover = IM_COL32(55, 55, 55, 255);
        ImU32 col_border_accent = pal.accent_dim;
        ImU32 col_label_idle = IM_COL32(150, 150, 150, 255);
        ImU32 col_label_hover = IM_COL32(200, 200, 200, 255);

        ImU32 bg_col = LerpColor(col_bg_idle, col_bg_hover, anim.hover);
        ImU32 border_col = LerpColor(col_border_idle, col_border_hover, anim.hover);

        if (anim.hover > 0.5f)
        {
            float accent_t = (anim.hover - 0.5f) * 2.0f;
            border_col = LerpColor(border_col, col_border_accent, accent_t * 0.4f);
        }

        ImU32 label_col = LerpColor(col_label_idle, col_label_hover, anim.hover);

        ImVec2 label_pos(pos.x, pos.y + (label_height - ImGui::GetTextLineHeight()) * 0.5f);
        DrawTextShadow(draw, label_pos, label_col, label);

        if (anim.glow > 0.01f)
        {
            ImU32 glow_col = gui::colors::AccentDimA((int)(20 * anim.glow));
            draw->AddRectFilled(
                ImVec2(list_x - 2, list_y - 2),
                ImVec2(list_x + list_w + 2, list_y + list_height + 2),
                glow_col, rounding + 2
            );
        }

        draw->AddRectFilled(
            ImVec2(list_x, list_y),
            ImVec2(list_x + list_w, list_y + list_height),
            bg_col, rounding
        );

        draw->AddRect(
            ImVec2(list_x, list_y),
            ImVec2(list_x + list_w, list_y + list_height),
            border_col, rounding
        );

        draw->PushClipRect(
            ImVec2(list_x, list_y),
            ImVec2(list_x + list_w, list_y + list_height), true);

        bool changed = false;
        float scroll_offset = anim.scroll_anim;

        for (int i = 0; i < items_count && i < 64; i++)
        {
            float item_y = list_y + padding + i * item_height - scroll_offset;

            if (item_y + item_height < list_y || item_y > list_y + list_height)
            {
                anim.item_hovers[i] = Lerp(anim.item_hovers[i], 0.0f, dt * 12.0f);
                continue;
            }

            bool item_hovered = !blocked_by_dropdown && list_hovered &&
                (mouse.x >= list_x + 2 && mouse.x <= list_x + list_w - 2 &&
                    mouse.y >= item_y && mouse.y <= item_y + item_height &&
                    mouse.y >= list_y && mouse.y <= list_y + list_height);

            float item_hover_target = item_hovered ? 1.0f : 0.0f;
            anim.item_hovers[i] = Lerp(anim.item_hovers[i], item_hover_target, dt * 14.0f);

            if (item_hovered && ImGui::IsMouseClicked(0))
            {
                if (*current != i)
                {
                    *current = i;
                    changed = true;
                    anim.select_flash = 1.0f;
                }
            }

            bool is_selected = (i == *current);

            float highlight = anim.item_hovers[i];

            if (is_selected)
            {
                float sel_alpha = 0.6f + 0.2f * anim.item_hovers[i];
                sel_alpha += anim.select_flash * 0.3f;
                if (sel_alpha > 1.0f) sel_alpha = 1.0f;

                ImU32 sel_bg = gui::colors::AccentBg(0.55f, (int)(180 * sel_alpha));
                draw->AddRectFilled(
                    ImVec2(list_x + 2, item_y),
                    ImVec2(list_x + list_w - 2, item_y + item_height),
                    sel_bg, rounding - 1
                );

                float bar_alpha = 0.7f + 0.3f * anim.item_hovers[i] + anim.select_flash * 0.3f;
                if (bar_alpha > 1.0f) bar_alpha = 1.0f;
                ImU32 bar_col = gui::colors::AccentA((int)(255 * bar_alpha));
                draw->AddRectFilled(
                    ImVec2(list_x + 2, item_y + 2),
                    ImVec2(list_x + 4, item_y + item_height - 2),
                    bar_col, 1.0f
                );
            }
            else if (highlight > 0.01f)
            {
                ImU32 hover_bg = gui::colors::AccentBg(0.40f, (int)(140 * highlight));
                draw->AddRectFilled(
                    ImVec2(list_x + 2, item_y),
                    ImVec2(list_x + list_w - 2, item_y + item_height),
                    hover_bg, rounding - 1
                );
            }

            ImU32 text_base = is_selected ? pal.accent : IM_COL32(155, 155, 155, 255);
            ImU32 text_hover = IM_COL32(255, 255, 255, 255);
            ImU32 text_col = LerpColor(text_base, text_hover, anim.item_hovers[i]);

            if (is_selected)
            {
                text_col = LerpColor(gui::colors::AccentBright(255), IM_COL32(255, 255, 255, 255), anim.item_hovers[i]);
            }

            float text_x = list_x + (is_selected ? S(10.0f) : S(8.0f));
            float text_y = item_y + (item_height - ImGui::GetTextLineHeight()) * 0.5f;

            draw->AddText(ImVec2(text_x + 1, text_y + 1), IM_COL32(0, 0, 0, 120), items[i]);
            draw->AddText(ImVec2(text_x, text_y), text_col, items[i]);

            if (i < items_count - 1)
            {
                float sep_alpha = 0.3f - anim.item_hovers[i] * 0.2f;
                if (sep_alpha > 0.01f)
                {
                    ImU32 sep_col = IM_COL32(40, 40, 40, (int)(255 * sep_alpha));
                    draw->AddLine(
                        ImVec2(list_x + 6, item_y + item_height - 0.5f),
                        ImVec2(list_x + list_w - 6, item_y + item_height - 0.5f),
                        sep_col
                    );
                }
            }
        }

        draw->PopClipRect();

        if (max_scroll > 0.0f)
        {
            float scrollbar_w = S(3.0f);
            float scrollbar_x = list_x + list_w - scrollbar_w - 2.0f;
            float scrollbar_area_h = list_height - padding * 2.0f;
            float visible_ratio = (list_height - padding * 2.0f) / content_height;
            float scrollbar_h = scrollbar_area_h * visible_ratio;
            if (scrollbar_h < S(14.0f)) scrollbar_h = S(14.0f);

            float scroll_ratio = anim.scroll_anim / max_scroll;
            float scrollbar_y = list_y + padding + (scrollbar_area_h - scrollbar_h) * scroll_ratio;

            ImU32 track_col = IM_COL32(25, 25, 25, (int)(120 * anim.hover));
            draw->AddRectFilled(
                ImVec2(scrollbar_x, list_y + padding),
                ImVec2(scrollbar_x + scrollbar_w, list_y + list_height - padding),
                track_col, scrollbar_w * 0.5f
            );

            bool sb_hovered = !blocked_by_dropdown &&
                (mouse.x >= scrollbar_x - 2 && mouse.x <= scrollbar_x + scrollbar_w + 2 &&
                    mouse.y >= scrollbar_y && mouse.y <= scrollbar_y + scrollbar_h);

            float sb_hover_alpha = sb_hovered ? 1.0f : 0.5f + 0.2f * anim.hover;
            ImU32 sb_idle = IM_COL32(50, 50, 50, (int)(255 * sb_hover_alpha));
            ImU32 sb_accent = gui::colors::AccentDimA((int)(255 * sb_hover_alpha));
            ImU32 sb_col = LerpColor(sb_idle, sb_accent, anim.hover * 0.5f);

            draw->AddRectFilled(
                ImVec2(scrollbar_x, scrollbar_y),
                ImVec2(scrollbar_x + scrollbar_w, scrollbar_y + scrollbar_h),
                sb_col, scrollbar_w * 0.5f
            );

            static ImGuiID dragging_scrollbar = 0;
            static float drag_start_y = 0.0f;
            static float drag_start_scroll = 0.0f;

            if (sb_hovered && ImGui::IsMouseClicked(0))
            {
                dragging_scrollbar = id;
                drag_start_y = mouse.y;
                drag_start_scroll = anim.scroll_target;
            }

            if (dragging_scrollbar == id)
            {
                if (ImGui::IsMouseDown(0))
                {
                    float delta = mouse.y - drag_start_y;
                    float scroll_per_pixel = max_scroll / (scrollbar_area_h - scrollbar_h);
                    anim.scroll_target = Clamp(drag_start_scroll + delta * scroll_per_pixel, 0.0f, max_scroll);
                }
                else
                {
                    dragging_scrollbar = 0;
                }
            }
        }

        if (max_scroll > 0.0f)
        {
            if (anim.scroll_anim > 1.0f)
            {
                float fade_alpha = Clamp(anim.scroll_anim / (item_height * 2.0f), 0.0f, 1.0f);
                for (int i = 0; i < 8; i++)
                {
                    float a = (1.0f - i / 8.0f) * fade_alpha * 0.8f;
                    ImU32 fade_col = IM_COL32(16, 16, 16, (int)(255 * a));
                    draw->AddRectFilled(
                        ImVec2(list_x + 1, list_y + i * 1.5f),
                        ImVec2(list_x + list_w - 1, list_y + (i + 1) * 1.5f),
                        fade_col
                    );
                }
            }

            if (anim.scroll_anim < max_scroll - 1.0f)
            {
                float fade_alpha = Clamp((max_scroll - anim.scroll_anim) / (item_height * 2.0f), 0.0f, 1.0f);
                for (int i = 0; i < 8; i++)
                {
                    float a = (1.0f - i / 8.0f) * fade_alpha * 0.8f;
                    ImU32 fade_col = IM_COL32(16, 16, 16, (int)(255 * a));
                    draw->AddRectFilled(
                        ImVec2(list_x + 1, list_y + list_height - (i + 1) * 1.5f),
                        ImVec2(list_x + list_w - 1, list_y + list_height - i * 1.5f),
                        fade_col
                    );
                }
            }
        }

        if (hint && hint[0] != '\0')
        {
            float qx = pos.x + width - S(10.0f);
            float qy = pos.y + (label_height - S(10.0f)) * 0.5f;
            DrawHintTooltip(ImGui::GetID(label), qx, qy, S(10.0f), S(12.0f), hint);
        }

        ImGui::PopID();
        return changed;
    }
}
