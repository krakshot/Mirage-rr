#pragma once
#include "imgui.h"

namespace gui {

    namespace colors {


        enum class Theme
        {
            Ice,
            Steel,
            Amber,
            Forest,
            Crimson,
            Violet,
            Rose,
            Mint,
            COUNT

        };

        struct Palette
        {
            ImU32 window_bg;
            ImU32 window_border_light;
            ImU32 window_border_dark;
            ImU32 header_bg;
            ImU32 panel_bg;
            ImU32 panel_border;
            ImU32 accent;
            ImU32 accent_dim;
            ImU32 text;
            ImU32 text_dim;
            ImU32 text_muted;
            ImU32 control_bg;
            ImU32 control_border;
            ImU32 tooltip_bg;
        };

        inline Palette& Get()
        {
            static Palette palette = {
                IM_COL32(18, 18, 18, 255), // window_bg
                IM_COL32(70, 70, 70, 255), // window_border_light
                IM_COL32(8, 8, 8, 255), // window_border_dark
                IM_COL32(24, 24, 24, 255), // header_bg
                IM_COL32(20, 20, 20, 255), // panel_bg
                IM_COL32(60, 60, 60, 255), // panel_border
                IM_COL32(120, 150, 255, 255), // accent
                IM_COL32(90, 110, 190, 255), // accent_dim
                IM_COL32(230, 230, 230, 255), // text
                IM_COL32(170, 170, 170, 255), // text_dim
                IM_COL32(140, 140, 140, 255), // text_muted
                IM_COL32(28, 28, 28, 255), // control_bg
                IM_COL32(50, 50, 50, 255), // control_border
                IM_COL32(22, 22, 22, 240) // tooltip_bg
            };

            return palette;
        }

        inline void SetTheme(Theme theme)
        {
            auto& p = Get();
            switch (theme)
            {
            case Theme::Ice: // default blue
                p.accent = IM_COL32(120, 150, 255, 255);
                p.accent_dim = IM_COL32(90, 110, 190, 255);
                break;
            case Theme::Steel: // silver/grey
                p.accent = IM_COL32(160, 170, 185, 255);
                p.accent_dim = IM_COL32(120, 128, 140, 255);
                break;
            case Theme::Amber: // warm orange
                p.accent = IM_COL32(240, 180, 50, 255);
                p.accent_dim = IM_COL32(180, 135, 40, 255);
                break;
            case Theme::Forest: // green
                p.accent = IM_COL32(80, 200, 120, 255);
                p.accent_dim = IM_COL32(60, 150, 90, 255);
                break;
            case Theme::Crimson: // red
                p.accent = IM_COL32(220, 70, 70, 255);
                p.accent_dim = IM_COL32(170, 50, 50, 255);
                break;
            case Theme::Violet: // purple
                p.accent = IM_COL32(170, 100, 240, 255);
                p.accent_dim = IM_COL32(130, 75, 185, 255);
                break;
            case Theme::Rose: // pink
                p.accent = IM_COL32(230, 100, 150, 255);
                p.accent_dim = IM_COL32(175, 75, 115, 255);
                break;
            case Theme::Mint: // cyan
                p.accent = IM_COL32(80, 220, 210, 255);
                p.accent_dim = IM_COL32(60, 165, 158, 255);
                break;
            default: break;
            }
        }

        inline void SetDensity(int density)
        {
            float t = (float)density / 100.0f;
            int base = (int)(35.0f - 29.0f * t);
            int header = base + 6;
            int panel = base + 2;
            int control = base + 10;
            int ctrl_brd = base + 32;
            int tooltip = base + 4;
            int brd_l = base + 52;
            int brd_d = (base > 8) ? base - 10 : 0;
            int pnl_brd = base + 42;

            auto& p = Get();
            p.window_bg = IM_COL32(base, base, base, 255);
            p.window_border_light = IM_COL32(brd_l, brd_l, brd_l, 255);
            p.window_border_dark = IM_COL32(brd_d, brd_d, brd_d, 255);
            p.header_bg = IM_COL32(header, header, header, 255);
            p.panel_bg = IM_COL32(panel, panel, panel, 255);
            p.panel_border = IM_COL32(pnl_brd, pnl_brd, pnl_brd, 255);
            p.control_bg = IM_COL32(control, control, control, 255);
            p.control_border = IM_COL32(ctrl_brd, ctrl_brd, ctrl_brd, 255);
            p.tooltip_bg = IM_COL32(tooltip, tooltip, tooltip, 240);
        }

        inline ImU32 AccentA(int alpha)
        {
            return (Get().accent & 0x00FFFFFF) | ((ImU32)(alpha & 0xFF) << 24);
        }
        inline ImU32 AccentDimA(int alpha)
        {
            return (Get().accent_dim & 0x00FFFFFF) | ((ImU32)(alpha & 0xFF) << 24);
        }
        inline ImU32 AccentBg(float factor, int alpha)
        {
            ImU32 a = Get().accent;
            int r = (int)(((a >> 0) & 0xFF) * factor);
            int g = (int)(((a >> 8) & 0xFF) * factor);
            int b = (int)(((a >> 16) & 0xFF) * factor);
            return IM_COL32(r, g, b, alpha);
        }
        inline ImU32 AccentBright(int alpha)
        {
            ImU32 a = Get().accent;
            int r = (int)(((a >> 0) & 0xFF) * 1.3f); if (r > 255) r = 255;
            int g = (int)(((a >> 8) & 0xFF) * 1.3f); if (g > 255) g = 255;
            int b = (int)(((a >> 16) & 0xFF) * 1.3f); if (b > 255) b = 255;
            return IM_COL32(r, g, b, alpha);
        }

    }
}

