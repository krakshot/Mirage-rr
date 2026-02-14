#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>

namespace widgets
{
    static float S_T(float v) { return v * dpi_scale_t; }

    static float LerpF(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    static float ClampF(float v, float lo, float hi)
    {
        return v < lo ? lo : (v > hi ? hi : v);
    }

    static ImU32 BlendAlphaT(ImU32 col, float a)
    {
        int r = (col >> 0) & 0xFF;
        int g = (col >> 8) & 0xFF;
        int b = (col >> 16) & 0xFF;
        int base_a = (col >> 24) & 0xFF;
        int na = (int)((float)base_a * ClampF(a, 0.0f, 1.0f));
        return IM_COL32(r, g, b, na);
    }

    static float EaseOutCubicT(float t)
    {
        t = ClampF(t, 0.0f, 1.0f);
        t = 1.0f - t;
        return 1.0f - t * t * t;
    }

    static float EaseInCubicT(float t)
    {
        t = ClampF(t, 0.0f, 1.0f);
        return t * t * t;
    }

    struct ToastInternal
    {
        char title[64];
        char message[256];
        ToastType type;
        float duration;
        float elapsed;
        float enter_anim;
        float exit_anim;
        float target_y;
        float current_y;
        bool  hovered;
        bool  dismissed;
        bool  removing;
        int   id;
    };

    static std::vector<ToastInternal> toasts_t;
    static int toast_next_id_t = 0;
    static const int MAX_VISIBLE = 6;

    struct ToastStyle
    {
        ImU32 accent;
        ImU32 icon_bg;
        const char* icon_char;
    };

    static ToastStyle GetToastStyle(ToastType type)
    {
        switch (type)
        {
        case ToastType::Success:
            return { IM_COL32(80, 200, 100, 255), IM_COL32(40, 100, 50, 200), "\xEE\x80\x87" };
        case ToastType::Warning:
            return { IM_COL32(240, 190, 50, 255), IM_COL32(120, 95, 25, 200), "!" };
        case ToastType::Error:
            return { IM_COL32(220, 60, 60, 255), IM_COL32(110, 30, 30, 200), "X" };
        case ToastType::Info:
        default:
            return { gui::colors::Get().accent, gui::colors::AccentDimA(200), "i" };
        }
    }

    void PushNotification(const char* title, const char* message, ToastType type, float duration)
    {
        ToastInternal t = {};
        strncpy(t.title, title, sizeof(t.title) - 1);
        t.title[sizeof(t.title) - 1] = '\0';
        strncpy(t.message, message, sizeof(t.message) - 1);
        t.message[sizeof(t.message) - 1] = '\0';
        t.type = type;
        t.duration = duration;
        t.elapsed = 0.0f;
        t.enter_anim = 0.0f;
        t.exit_anim = 0.0f;
        t.target_y = 0.0f;
        t.current_y = 0.0f;
        t.hovered = false;
        t.dismissed = false;
        t.removing = false;
        t.id = toast_next_id_t++;

        toasts_t.insert(toasts_t.begin(), t);

        while ((int)toasts_t.size() > MAX_VISIBLE + 2)
            toasts_t.pop_back();
    }

    void ClearNotifications() { toasts_t.clear(); }

    void DrawNotifications()
    {
        if (toasts_t.empty()) return;

        float dt = ImGui::GetIO().DeltaTime;
        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = gui::colors::Get();

        ImGui::PushFont(ImGui::GetFont());

        float toast_w = S_T(300.0f);
        float pad_x = S_T(14.0f);
        float pad_y = S_T(10.0f);
        float margin = S_T(14.0f);
        float toast_gap = S_T(6.0f);
        float rounding = S_T(4.0f);
        float accent_w = S_T(3.0f);
        float icon_size = S_T(28.0f);
        float icon_gap = S_T(10.0f);
        float close_size = S_T(12.0f);
        float close_pad = S_T(8.0f);
        float progress_h = S_T(2.5f);

        float title_font_scale = 1.0f;

        for (int i = 0; i < (int)toasts_t.size(); i++)
        {
            auto& t = toasts_t[i];

            t.enter_anim = LerpF(t.enter_anim, 1.0f, dt * 8.0f);

            if (!t.hovered && !t.removing)
                t.elapsed += dt;

            if ((t.elapsed >= t.duration || t.dismissed) && !t.removing)
                t.removing = true;

            if (t.removing)
                t.exit_anim = LerpF(t.exit_anim, 1.0f, dt * 6.0f);
        }

        for (int i = (int)toasts_t.size() - 1; i >= 0; i--)
        {
            if (toasts_t[i].removing && toasts_t[i].exit_anim > 0.97f)
                toasts_t.erase(toasts_t.begin() + i);
        }

        struct ToastMeasure { float h; };
        std::vector<ToastMeasure> measures(toasts_t.size());

        for (int i = 0; i < (int)toasts_t.size(); i++)
        {
            auto& t = toasts_t[i];

            float text_area_w = toast_w - pad_x * 2.0f - icon_size - icon_gap - close_size - close_pad;
            ImVec2 title_sz = ImGui::CalcTextSize(t.title, nullptr, false, text_area_w);
            ImVec2 msg_sz = ImGui::CalcTextSize(t.message, nullptr, false, text_area_w);

            float content_h = title_sz.y + S_T(3.0f) + msg_sz.y;
            float total_h = pad_y * 2.0f + (content_h > icon_size ? content_h : icon_size) + progress_h + S_T(4.0f);
            measures[i].h = total_h;
        }

        float stack_y = io.DisplaySize.y - margin;
        for (int i = 0; i < (int)toasts_t.size(); i++)
        {
            stack_y -= measures[i].h;
            toasts_t[i].target_y = stack_y;
            stack_y -= toast_gap;
        }

        for (auto& t : toasts_t)
        {
            if (t.current_y == 0.0f)
                t.current_y = t.target_y + S_T(40.0f);
            t.current_y = LerpF(t.current_y, t.target_y, dt * 12.0f);
        }

        for (int i = 0; i < (int)toasts_t.size(); i++)
        {
            auto& t = toasts_t[i];
            float enter = EaseOutCubicT(ClampF(t.enter_anim, 0.0f, 1.0f));
            float exit = EaseInCubicT(ClampF(t.exit_anim, 0.0f, 1.0f));

            float slide_in = (1.0f - enter) * (toast_w + margin);
            float slide_out = exit * (toast_w + margin * 2.0f);
            float x_offset = slide_in + slide_out;

            float alpha = ClampF(enter - exit, 0.0f, 1.0f);
            if (alpha < 0.01f) continue;

            float px = io.DisplaySize.x - toast_w - margin + x_offset;
            float py = t.current_y;
            float ph = measures[i].h;

            ToastStyle style = GetToastStyle(t.type);

            ImVec2 mouse = io.MousePos;
            bool in_toast = (mouse.x >= px && mouse.x <= px + toast_w &&
                mouse.y >= py && mouse.y <= py + ph);
            t.hovered = in_toast && !t.removing;

            float cx = px + toast_w - close_pad - close_size;
            float cy_btn = py + pad_y;
            bool in_close = (mouse.x >= cx - S_T(2.0f) && mouse.x <= cx + close_size + S_T(2.0f) &&
                mouse.y >= cy_btn - S_T(2.0f) && mouse.y <= cy_btn + close_size + S_T(2.0f));

            if (in_close && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !t.removing)
                t.dismissed = true;

            ImU32 bg_base = t.hovered ? IM_COL32(22, 22, 22, 245) : IM_COL32(14, 14, 14, 240);
            ImU32 bg_col = BlendAlphaT(bg_base, alpha);
            ImU32 brd_col = BlendAlphaT(IM_COL32(45, 45, 45, 200), alpha);

            draw->AddRectFilled(ImVec2(px, py), ImVec2(px + toast_w, py + ph), bg_col, rounding);
            draw->AddRect(ImVec2(px, py), ImVec2(px + toast_w, py + ph), brd_col, rounding);

            draw->AddRectFilled(
                ImVec2(px, py + rounding),
                ImVec2(px + accent_w, py + ph - rounding),
                BlendAlphaT(style.accent, alpha)
            );
            for (int g = 1; g <= 2; g++)
            {
                float e = (float)g * S_T(2.0f);
                draw->AddRectFilled(
                    ImVec2(px - e, py + rounding),
                    ImVec2(px + accent_w + e * 0.3f, py + ph - rounding),
                    BlendAlphaT(style.accent, alpha * 0.08f / (float)g)
                );
            }

            float icon_x = px + accent_w + pad_x;
            float icon_cy = py + pad_y + icon_size * 0.5f;
            float icon_cx = icon_x + icon_size * 0.5f;

            draw->AddCircleFilled(ImVec2(icon_cx, icon_cy), icon_size * 0.5f,
                BlendAlphaT(style.icon_bg, alpha));
            draw->AddCircle(ImVec2(icon_cx, icon_cy), icon_size * 0.5f,
                BlendAlphaT(style.accent, alpha * 0.5f), 0, S_T(1.5f));

            ImVec2 ic_sz = ImGui::CalcTextSize(style.icon_char);
            draw->AddText(
                ImVec2(icon_cx - ic_sz.x * 0.5f, icon_cy - ic_sz.y * 0.5f),
                BlendAlphaT(style.accent, alpha), style.icon_char
            );

            float text_x = icon_x + icon_size + icon_gap;
            float text_area_w = toast_w - (text_x - px) - close_size - close_pad - pad_x * 0.5f;

            float title_y = py + pad_y;
            ImVec2 title_sz = ImGui::CalcTextSize(t.title, nullptr, false, text_area_w);

            draw->AddText(ImGui::GetFont(), ImGui::GetFontSize() * title_font_scale,
                ImVec2(text_x + 1, title_y + 1), BlendAlphaT(IM_COL32(0, 0, 0, 160), alpha),
                t.title, nullptr, text_area_w);
            draw->AddText(ImGui::GetFont(), ImGui::GetFontSize() * title_font_scale,
                ImVec2(text_x, title_y), BlendAlphaT(IM_COL32(235, 235, 235, 255), alpha),
                t.title, nullptr, text_area_w);

            float msg_y = title_y + title_sz.y + S_T(3.0f);
            draw->AddText(ImGui::GetFont(), ImGui::GetFontSize(),
                ImVec2(text_x, msg_y), BlendAlphaT(IM_COL32(160, 160, 160, 220), alpha),
                t.message, nullptr, text_area_w);

            if (t.hovered && !t.removing)
            {
                ImU32 close_col = in_close
                    ? BlendAlphaT(IM_COL32(220, 80, 80, 255), alpha)
                    : BlendAlphaT(IM_COL32(140, 140, 140, 200), alpha);

                float x1 = cx + S_T(2.0f);
                float y1 = cy_btn + S_T(2.0f);
                float x2 = cx + close_size - S_T(2.0f);
                float y2 = cy_btn + close_size - S_T(2.0f);

                draw->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), close_col, S_T(1.5f));
                draw->AddLine(ImVec2(x2, y1), ImVec2(x1, y2), close_col, S_T(1.5f));
            }

            float progress = 1.0f - ClampF(t.elapsed / t.duration, 0.0f, 1.0f);
            float bar_y = py + ph - progress_h;
            float bar_w = (toast_w - rounding * 2.0f) * progress;

            draw->AddRectFilled(
                ImVec2(px + rounding, bar_y),
                ImVec2(px + toast_w - rounding, bar_y + progress_h),
                BlendAlphaT(IM_COL32(30, 30, 30, 150), alpha)
            );

            if (bar_w > 0.5f)
            {
                draw->AddRectFilled(
                    ImVec2(px + rounding, bar_y),
                    ImVec2(px + rounding + bar_w, bar_y + progress_h),
                    BlendAlphaT(style.accent, alpha * 0.7f),
                    S_T(1.0f)
                );
            }

            draw->AddLine(
                ImVec2(px + rounding, py + ph - 1),
                ImVec2(px + toast_w - rounding, py + ph - 1),
                BlendAlphaT(IM_COL32(0, 0, 0, 30), alpha)
            );
        }

        ImGui::PopFont();
    }

}
