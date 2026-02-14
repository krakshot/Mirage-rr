#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include "../../ressources/font/font.hh"
#include <cstdio>
#include <cmath>
#include <ctime>

namespace widgets
{
    static WatermarkAnim g_watermark_anim;
    static float watermark_time_t = 0.0f;

    static float EaseOutExpo(float t)
    {
        return t >= 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * t);
    }

    static float EaseOutCubicWm(float t)
    {
        t = t - 1.0f;
        return t * t * t + 1.0f;
    }

    void DrawWatermark(bool visible, int position, const char* name)
    {
        ImGuiIO& io = ImGui::GetIO();
        float dt = io.DeltaTime;
        auto& anim = g_watermark_anim;

        anim.timer += dt;
        watermark_time_t += dt;

        float target_alpha = visible ? 1.0f : 0.0f;
        anim.alpha = Lerp(anim.alpha, target_alpha, dt * 8.0f);

        float target_slide = visible ? 1.0f : 0.0f;
        anim.slide = Lerp(anim.slide, target_slide, dt * 6.0f);

        float target_sep = visible ? 1.0f : 0.0f;
        anim.separator_w = Lerp(anim.separator_w, target_sep, dt * 5.0f);

        float glow_target = visible ? (0.3f + 0.15f * sinf(anim.timer * 2.0f)) : 0.0f;
        anim.glow = Lerp(anim.glow, glow_target, dt * 4.0f);

        anim.fps_pulse = Lerp(anim.fps_pulse, 0.0f, dt * 6.0f);

        if (anim.alpha < 0.01f)
            return;

        char fps_buf[32];
        snprintf(fps_buf, sizeof(fps_buf), "%d fps", (int)(io.Framerate + 0.5f));

        time_t now = time(nullptr);
        struct tm t_local;
        localtime_s(&t_local, &now);
        char time_buf[16];
        snprintf(time_buf, sizeof(time_buf), "%02d:%02d:%02d", t_local.tm_hour, t_local.tm_min, t_local.tm_sec);

        static int last_fps = 0;
        int cur_fps = (int)(io.Framerate + 0.5f);
        if (cur_fps != last_fps)
        {
            anim.fps_pulse = 0.25f;
            last_fps = cur_fps;
        }

        const auto& pal = gui::colors::Get();
        ImFont* font_base = gui::font::Get().base;

        if (font_base) ImGui::PushFont(font_base);
        float font_sz = ImGui::GetFontSize();
        ImFont* active_font = ImGui::GetFont();

        ImVec2 name_size = active_font->CalcTextSizeA(font_sz, FLT_MAX, 0.0f, name);
        ImVec2 fps_size = active_font->CalcTextSizeA(font_sz, FLT_MAX, 0.0f, fps_buf);
        ImVec2 time_size = active_font->CalcTextSizeA(font_sz, FLT_MAX, 0.0f, time_buf);
        if (font_base) ImGui::PopFont();

        const float h_pad = S(10.0f);
        const float v_pad = S(6.0f);
        const float sep_gap = S(8.0f);
        const float sep_w = 1.0f;

        float total_w = h_pad + name_size.x + sep_gap + sep_w + sep_gap + fps_size.x + sep_gap + sep_w + sep_gap + time_size.x + h_pad;
        float total_h = v_pad * 2.0f + name_size.y;

        float margin = S(12.0f);
        float target_x, target_y;

        int corner = Clamp((float)position, 0.0f, 3.0f);
        bool is_right = (corner == 1 || corner == 3);
        bool is_bottom = (corner == 2 || corner == 3);

        target_x = is_right ? (io.DisplaySize.x - total_w - margin) : margin;
        target_y = is_bottom ? (io.DisplaySize.y - total_h - margin) : margin;

        if (anim.pos_x < 0.0f || anim.last_corner < 0)
        {
            float off_x = is_right ? (io.DisplaySize.x + 20.0f) : (-total_w - 20.0f);
            float off_y = target_y;
            anim.pos_x = off_x;
            anim.pos_y = off_y;
            anim.last_corner = corner;
        }

        if (anim.last_corner != corner)
        {
            anim.last_corner = corner;
        }

        float slide_speed = 8.0f;
        anim.pos_x = Lerp(anim.pos_x, target_x, dt * slide_speed);
        anim.pos_y = Lerp(anim.pos_y, target_y, dt * slide_speed);

        if (fabsf(anim.pos_x - target_x) < 0.5f) anim.pos_x = target_x;
        if (fabsf(anim.pos_y - target_y) < 0.5f) anim.pos_y = target_y;

        float x = anim.pos_x;
        float y = anim.pos_y;

        if (anim.slide < 0.99f)
        {
            float slide_off = (1.0f - EaseOutExpo(anim.slide)) * (total_w + margin + 20.0f);
            if (is_right)
                x += slide_off;
            else
                x -= slide_off;
        }

        float alpha = anim.alpha;
        ImDrawList* draw = ImGui::GetForegroundDrawList();

        ImU32 bg_col = BlendAlpha(IM_COL32(12, 12, 12, 230), alpha);
        draw->AddRectFilled(
            ImVec2(x, y),
            ImVec2(x + total_w, y + total_h),
            bg_col, 2.0f
        );

        float accent_w = total_w * Clamp(anim.separator_w, 0.0f, 1.0f);
        float accent_x_start = x + (total_w - accent_w) * 0.5f;
        ImU32 accent_col = BlendAlpha(pal.accent, alpha);

        float accent_y1, accent_y2;
        if (is_bottom)
        {
            accent_y1 = y + total_h - 2.0f;
            accent_y2 = y + total_h;
        }
        else
        {
            accent_y1 = y;
            accent_y2 = y + 2.0f;
        }
        draw->AddRectFilled(
            ImVec2(accent_x_start, accent_y1),
            ImVec2(accent_x_start + accent_w, accent_y2),
            accent_col
        );

        if (anim.glow > 0.01f)
        {
            ImU32 glow_col = BlendAlpha(pal.accent, alpha * anim.glow * 0.6f);
            for (int i = 1; i <= 4; i++)
            {
                float expand = (float)i * 1.5f;
                draw->AddRectFilled(
                    ImVec2(accent_x_start - expand, accent_y1 - expand),
                    ImVec2(accent_x_start + accent_w + expand, accent_y2 + expand * 0.5f),
                    BlendAlpha(glow_col, 1.0f / (float)(i + 1))
                );
            }
        }

        ImU32 border_col = BlendAlpha(IM_COL32(40, 40, 40, 200), alpha);
        draw->AddRect(
            ImVec2(x, y),
            ImVec2(x + total_w, y + total_h),
            border_col, 2.0f
        );

        float shadow_y = is_bottom ? (y + 1) : (y + total_h - 1);
        draw->AddLine(
            ImVec2(x + 1, shadow_y),
            ImVec2(x + total_w - 1, shadow_y),
            BlendAlpha(IM_COL32(0, 0, 0, 60), alpha)
        );

        float cx = x + h_pad;
        float cy = y + v_pad;

        if (font_base) ImGui::PushFont(font_base);

        ImU32 name_col = BlendAlpha(pal.accent, alpha);
        ImU32 name_shadow = BlendAlpha(IM_COL32(0, 0, 0, 180), alpha);
        draw->AddText(ImVec2(cx + 1, cy + 1), name_shadow, name);
        draw->AddText(ImVec2(cx, cy), name_col, name);

        cx += name_size.x + sep_gap;

        float sep_h = (total_h - v_pad * 2.0f) * Clamp(anim.separator_w, 0.0f, 1.0f);
        float sep_y_offset = ((total_h - v_pad * 2.0f) - sep_h) * 0.5f;
        ImU32 sep_col = BlendAlpha(IM_COL32(55, 55, 55, 200), alpha);
        draw->AddLine(
            ImVec2(cx, cy + sep_y_offset),
            ImVec2(cx, cy + sep_y_offset + sep_h),
            sep_col
        );

        cx += sep_w + sep_gap;

        float fps_alpha_mult = 1.0f + anim.fps_pulse * 0.5f;
        ImU32 fps_col = BlendAlpha(IM_COL32(230, 230, 230, 255), alpha * Clamp(fps_alpha_mult, 0.0f, 1.0f));
        ImU32 fps_shadow = BlendAlpha(IM_COL32(0, 0, 0, 180), alpha);
        draw->AddText(ImVec2(cx + 1, cy + 1), fps_shadow, fps_buf);
        draw->AddText(ImVec2(cx, cy), fps_col, fps_buf);

        cx += fps_size.x + sep_gap;

        draw->AddLine(
            ImVec2(cx, cy + sep_y_offset),
            ImVec2(cx, cy + sep_y_offset + sep_h),
            sep_col
        );

        cx += sep_w + sep_gap;

        ImU32 time_col = BlendAlpha(IM_COL32(170, 170, 170, 255), alpha);
        ImU32 time_shadow = BlendAlpha(IM_COL32(0, 0, 0, 180), alpha);
        draw->AddText(ImVec2(cx + 1, cy + 1), time_shadow, time_buf);
        draw->AddText(ImVec2(cx, cy), time_col, time_buf);

        if (font_base) ImGui::PopFont();
    }
}
