#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include <cstdio>
#include <cmath>
#include <cstring>
#include <vector>

namespace widgets
{
    float dpi_scale_t = 1.0f;
    WidgetState state_t;
    std::unordered_map<ImGuiID, ComboAnim> combo_anims_t;
    std::unordered_map<ImGuiID, TooltipAnim> tooltip_anims_t;
    std::unordered_map<ImGuiID, KeybindAnim> keybind_anims_t;

    static Keybind* listening_bind_t = nullptr;

    static ImU32 LerpColorSel(ImU32 a, ImU32 b, float t)
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

    float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    float Clamp(float v, float lo, float hi)
    {
        return v < lo ? lo : (v > hi ? hi : v);
    }

    ImU32 BlendAlpha(ImU32 col, float alpha)
    {
        ImVec4 c = ImGui::ColorConvertU32ToFloat4(col);
        c.w *= alpha;
        return ImGui::ColorConvertFloat4ToU32(c);
    }

    void DrawTextShadow(ImDrawList* draw, ImVec2 pos, ImU32 col, const char* text)
    {
        draw->AddText(ImVec2(pos.x + 1, pos.y + 1), IM_COL32(0, 0, 0, 150), text);
        draw->AddText(pos, col, text);
    }

    const char* GetKeyName(ImGuiKey key)
    {
        if (key == ImGuiKey_None) return "None";
        if (key == ImGuiKey_MouseLeft) return "M1";
        if (key == ImGuiKey_MouseRight) return "M2";
        if (key == ImGuiKey_MouseMiddle) return "M3";
        if (key == ImGuiKey_MouseX1) return "M4";
        if (key == ImGuiKey_MouseX2) return "M5";
        if (key == ImGuiKey_Tab) return "Tab";
        if (key == ImGuiKey_LeftArrow) return "Left";
        if (key == ImGuiKey_RightArrow) return "Right";
        if (key == ImGuiKey_UpArrow) return "Up";
        if (key == ImGuiKey_DownArrow) return "Down";
        if (key == ImGuiKey_Space) return "Space";
        if (key == ImGuiKey_Enter) return "Enter";
        if (key == ImGuiKey_Escape) return "Esc";
        if (key == ImGuiKey_Backspace) return "Back";
        if (key == ImGuiKey_Delete) return "Del";
        if (key == ImGuiKey_Insert) return "Ins";
        if (key == ImGuiKey_Home) return "Home";
        if (key == ImGuiKey_End) return "End";
        if (key == ImGuiKey_PageUp) return "PgUp";
        if (key == ImGuiKey_PageDown) return "PgDn";
        if (key == ImGuiKey_CapsLock) return "Caps";
        if (key == ImGuiKey_LeftShift) return "LShift";
        if (key == ImGuiKey_RightShift) return "RShift";
        if (key == ImGuiKey_LeftCtrl) return "LCtrl";
        if (key == ImGuiKey_RightCtrl) return "RCtrl";
        if (key == ImGuiKey_LeftAlt) return "LAlt";
        if (key == ImGuiKey_RightAlt) return "RAlt";
        if (key == ImGuiKey_F1) return "F1";
        if (key == ImGuiKey_F2) return "F2";
        if (key == ImGuiKey_F3) return "F3";
        if (key == ImGuiKey_F4) return "F4";
        if (key == ImGuiKey_F5) return "F5";
        if (key == ImGuiKey_F6) return "F6";
        if (key == ImGuiKey_F7) return "F7";
        if (key == ImGuiKey_F8) return "F8";
        if (key == ImGuiKey_F9) return "F9";
        if (key == ImGuiKey_F10) return "F10";
        if (key == ImGuiKey_F11) return "F11";
        if (key == ImGuiKey_F12) return "F12";
        if (key == ImGuiKey_A) return "A";
        if (key == ImGuiKey_B) return "B";
        if (key == ImGuiKey_C) return "C";
        if (key == ImGuiKey_D) return "D";
        if (key == ImGuiKey_E) return "E";
        if (key == ImGuiKey_F) return "F";
        if (key == ImGuiKey_G) return "G";
        if (key == ImGuiKey_H) return "H";
        if (key == ImGuiKey_I) return "I";
        if (key == ImGuiKey_J) return "J";
        if (key == ImGuiKey_K) return "K";
        if (key == ImGuiKey_L) return "L";
        if (key == ImGuiKey_M) return "M";
        if (key == ImGuiKey_N) return "N";
        if (key == ImGuiKey_O) return "O";
        if (key == ImGuiKey_P) return "P";
        if (key == ImGuiKey_Q) return "Q";
        if (key == ImGuiKey_R) return "R";
        if (key == ImGuiKey_S) return "S";
        if (key == ImGuiKey_T) return "T";
        if (key == ImGuiKey_U) return "U";
        if (key == ImGuiKey_V) return "V";
        if (key == ImGuiKey_W) return "W";
        if (key == ImGuiKey_X) return "X";
        if (key == ImGuiKey_Y) return "Y";
        if (key == ImGuiKey_Z) return "Z";
        if (key == ImGuiKey_0) return "0";
        if (key == ImGuiKey_1) return "1";
        if (key == ImGuiKey_2) return "2";
        if (key == ImGuiKey_3) return "3";
        if (key == ImGuiKey_4) return "4";
        if (key == ImGuiKey_5) return "5";
        if (key == ImGuiKey_6) return "6";
        if (key == ImGuiKey_7) return "7";
        if (key == ImGuiKey_8) return "8";
        if (key == ImGuiKey_9) return "9";
        if (key == ImGuiKey_GraveAccent) return "`";
        if (key == ImGuiKey_Minus) return "-";
        if (key == ImGuiKey_Equal) return "=";
        if (key == ImGuiKey_LeftBracket) return "[";
        if (key == ImGuiKey_RightBracket) return "]";
        if (key == ImGuiKey_Backslash) return "\\";
        if (key == ImGuiKey_Semicolon) return ";";
        if (key == ImGuiKey_Apostrophe) return "'";
        if (key == ImGuiKey_Comma) return ",";
        if (key == ImGuiKey_Period) return ".";
        if (key == ImGuiKey_Slash) return "/";
        return "?";
    }

    void ProcessKeybinds()
    {

        if (listening_bind_t != nullptr)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                listening_bind_t->waiting = false;
                listening_bind_t = nullptr;
                return;
            }

            for (int k = (int)ImGuiKey_NamedKey_BEGIN; k < (int)ImGuiKey_NamedKey_END; k++)
            {
                ImGuiKey key = (ImGuiKey)k;
                if (key == ImGuiKey_Escape) continue;
                if (key == ImGuiKey_MouseLeft) continue;

                if (ImGui::IsKeyPressed(key))
                {
                    listening_bind_t->key = key;
                    listening_bind_t->waiting = false;
                    listening_bind_t = nullptr;
                    return;
                }
            }
        }
    }

    bool DrawKeybind(Keybind* bind, float x, float y, float max_w)
    {
        if (!bind) return false;

        ImGui::PushID(bind);
        ImGuiID id = ImGui::GetID("##keybind");
        KeybindAnim& anim = keybind_anims_t[id];

        float dt = ImGui::GetIO().DeltaTime;
        ImVec2 mouse = ImGui::GetIO().MousePos;
        ImDrawList* draw = ImGui::GetWindowDrawList();

        const char* display_text;
        if (bind->waiting)
            display_text = "...";
        else if (bind->key == ImGuiKey_None)
            display_text = "-";
        else
            display_text = GetKeyName(bind->key);

        ImVec2 text_sz = ImGui::CalcTextSize(display_text);
        float btn_w = text_sz.x + S(8.0f);
        if (btn_w < S(18.0f)) btn_w = S(18.0f);
        if (btn_w > max_w) btn_w = max_w;
        float btn_h = S(12.0f);

        float btn_y = y + (S(18.0f) - btn_h) * 0.5f;

        bool hovered = (mouse.x >= x && mouse.x <= x + btn_w &&
            mouse.y >= btn_y && mouse.y <= btn_y + btn_h);

        bool blocked = false;
        if (state_t.open_combo != 0)
        {
            if (mouse.x >= state_t.dropdown_x1 && mouse.x <= state_t.dropdown_x2 &&
                mouse.y >= state_t.dropdown_y1 && mouse.y <= state_t.dropdown_y2)
                blocked = true;
        }
        if (blocked) hovered = false;

        if (hovered && ImGui::IsMouseClicked(0) && !bind->waiting)
        {
            bind->waiting = true;
            listening_bind_t = bind;
        }

        if (hovered && ImGui::IsMouseClicked(1))
        {
            if (bind->waiting)
            {
                bind->key = ImGuiKey_None;
                bind->waiting = false;
                bind->active = false;
                listening_bind_t = nullptr;
            }
            else
            {
                bind->mode = (bind->mode == KeybindMode::Toggle) ? KeybindMode::Hold : KeybindMode::Toggle;
            }
        }

        if (!bind->waiting && bind->key != ImGuiKey_None)
        {
            if (bind->mode == KeybindMode::Hold)
            {
                bind->active = ImGui::IsKeyDown(bind->key);
            }
            else
            {
                if (ImGui::IsKeyPressed(bind->key, false))
                {
                    bind->active = !bind->active;
                }
            }
        }

        float hover_target = hovered ? 1.0f : 0.0f;
        float listening_target = bind->waiting ? 1.0f : 0.0f;
        float glow_target = bind->active ? 1.0f : 0.0f;

        anim.hover = Lerp(anim.hover, hover_target, dt * 12.0f);
        anim.listening = Lerp(anim.listening, listening_target, dt * 10.0f);
        anim.glow = Lerp(anim.glow, glow_target, dt * 8.0f);

        if (bind->waiting)
        {
            anim.pulse_timer += dt * 3.0f;
            if (anim.pulse_timer > 6.28318f) anim.pulse_timer -= 6.28318f;
        }
        else
        {
            anim.pulse_timer = 0.0f;
        }

        const auto& pal = gui::colors::Get();
        ImU32 col_bg_idle = IM_COL32(20, 20, 20, 255);
        ImU32 col_bg_hover = IM_COL32(28, 28, 28, 255);
        ImU32 col_bg_listening = gui::colors::AccentBg(0.22f, 255);
        ImU32 col_bg_active = gui::colors::AccentBg(0.30f, 255);
        ImU32 col_border_idle = IM_COL32(42, 42, 42, 255);
        ImU32 col_border_hover = IM_COL32(60, 60, 60, 255);
        ImU32 col_border_listening = pal.accent;
        ImU32 col_border_active = pal.accent_dim;
        ImU32 col_text_idle = IM_COL32(100, 100, 100, 255);
        ImU32 col_text_hover = IM_COL32(160, 160, 160, 255);
        ImU32 col_text_listening = pal.accent;
        ImU32 col_text_active = gui::colors::AccentBright(255);

        ImU32 bg_col = LerpColorSel(col_bg_idle, col_bg_hover, anim.hover);
        bg_col = LerpColorSel(bg_col, col_bg_listening, anim.listening);
        bg_col = LerpColorSel(bg_col, col_bg_active, anim.glow * (1.0f - anim.listening));

        ImU32 border_col = LerpColorSel(col_border_idle, col_border_hover, anim.hover);
        border_col = LerpColorSel(border_col, col_border_listening, anim.listening);
        border_col = LerpColorSel(border_col, col_border_active, anim.glow * (1.0f - anim.listening));

        ImU32 text_col = LerpColorSel(col_text_idle, col_text_hover, anim.hover);
        text_col = LerpColorSel(text_col, col_text_listening, anim.listening);
        text_col = LerpColorSel(text_col, col_text_active, anim.glow * (1.0f - anim.listening));

        if (bind->waiting && anim.listening > 0.5f)
        {
            float pulse = (sinf(anim.pulse_timer) + 1.0f) * 0.5f;
            ImU32 pulse_col = gui::colors::AccentA((int)(80 + 80 * pulse));
            border_col = LerpColorSel(border_col, pulse_col, pulse * 0.5f);
        }

        if (anim.glow > 0.01f && !bind->waiting)
        {
            ImU32 glow_col = gui::colors::AccentDimA((int)(20 * anim.glow));
            draw->AddRectFilled(
                ImVec2(x - 1, btn_y - 1),
                ImVec2(x + btn_w + 1, btn_y + btn_h + 1),
                glow_col, 3.0f
            );
        }

        draw->AddRectFilled(
            ImVec2(x, btn_y),
            ImVec2(x + btn_w, btn_y + btn_h),
            bg_col, 2.0f
        );

        draw->AddRect(
            ImVec2(x, btn_y),
            ImVec2(x + btn_w, btn_y + btn_h),
            border_col, 2.0f
        );

        float tx = x + (btn_w - text_sz.x) * 0.5f;
        float ty = btn_y + (btn_h - text_sz.y) * 0.5f;
        draw->AddText(ImVec2(tx, ty), text_col, display_text);

        if (hovered && !bind->waiting && bind->key != ImGuiKey_None)
        {
            const char* mode_text = (bind->mode == KeybindMode::Hold) ? "[Hold]" : "[Toggle]";
            ImVec2 mode_sz = ImGui::CalcTextSize(mode_text);
            float tip_x = x + btn_w * 0.5f - mode_sz.x * 0.5f;
            float tip_y = btn_y - mode_sz.y - S(3.0f);

            ImDrawList* fg = ImGui::GetForegroundDrawList();
            float tip_pad = S(3.0f);
            fg->AddRectFilled(
                ImVec2(tip_x - tip_pad, tip_y - tip_pad + 1),
                ImVec2(tip_x + mode_sz.x + tip_pad, tip_y + mode_sz.y + tip_pad - 1),
                IM_COL32(15, 15, 15, (int)(240 * anim.hover)), 2.0f
            );
            fg->AddRect(
                ImVec2(tip_x - tip_pad, tip_y - tip_pad + 1),
                ImVec2(tip_x + mode_sz.x + tip_pad, tip_y + mode_sz.y + tip_pad - 1),
                IM_COL32(50, 50, 50, (int)(255 * anim.hover)), 2.0f
            );
            fg->AddText(ImVec2(tip_x, tip_y), IM_COL32(140, 140, 140, (int)(255 * anim.hover)), mode_text);
        }

        ImGui::PopID();
        return bind->active;
    }

    void DrawHintTooltip(ImGuiID id, float qx, float qy, float qw, float qh, const char* hint)
    {
        if (hint == nullptr || hint[0] == '\0')
            return;

        ImGuiIO& io = ImGui::GetIO();
        ImVec2 mouse = io.MousePos;
        bool hovered = (mouse.x >= qx && mouse.x <= qx + qw && mouse.y >= qy && mouse.y <= qy + qh);

        TooltipAnim& anim = tooltip_anims_t[id];
        float target = hovered ? 1.0f : 0.0f;
        anim.alpha = Lerp(anim.alpha, target, io.DeltaTime * 12.0f);
        anim.offset = Lerp(anim.offset, hovered ? 0.0f : 6.0f, io.DeltaTime * 12.0f);

        ImDrawList* draw = ImGui::GetWindowDrawList();
        ImU32 q_col = hovered ? IM_COL32(180, 180, 180, 255) : IM_COL32(80, 80, 80, 255);
        draw->AddText(ImVec2(qx + 1, qy + 1), IM_COL32(0, 0, 0, 150), "?");
        draw->AddText(ImVec2(qx, qy), q_col, "?");

        if (anim.alpha <= 0.01f)
            return;

        ImVec2 pos(qx + qw + S(8.0f), qy - 2.0f + anim.offset);

        char tooltip_id[64];
        std::snprintf(tooltip_id, sizeof(tooltip_id), "##hint_%u", id);

        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowBgAlpha(0.95f * anim.alpha);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, anim.alpha);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(S(8.0f), S(6.0f)));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(20, 20, 20, 240));
        ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(60, 60, 60, 255));
        ImGui::Begin(tooltip_id, nullptr, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);
        ImGui::TextUnformatted(hint);
        ImGui::End();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
    }

    void DrawBackgroundNoise(ImDrawList* draw, ImVec2 min, ImVec2 max, float intensity, float cell_size)
    {
        if (intensity <= 0.0f || cell_size < 1.0f)
            return;

        unsigned int frame_seed = (unsigned int)(ImGui::GetFrameCount() * 7919u);

        float scaled_cell = cell_size * dpi_scale_t;
        if (scaled_cell < 1.0f) scaled_cell = 1.0f;

        int max_alpha = (int)(intensity * 255.0f);
        if (max_alpha > 80) max_alpha = 80;

        float half_cell = scaled_cell * 0.5f;

        for (float y = min.y; y < max.y; y += scaled_cell)
        {
            for (float x = min.x; x < max.x; x += scaled_cell)
            {
                unsigned int ix = (unsigned int)(x * 73856093u);
                unsigned int iy = (unsigned int)(y * 19349663u);
                unsigned int h = (ix ^ iy ^ frame_seed);
                h = h * 2654435761u;
                h = (h >> 16) ^ h;

                if ((h & 3u) != 0u)
                    continue;

                int alpha = (int)((h & 0xFFu) * max_alpha / 255);
                if (alpha < 2) continue;

                int lum = 20 + (int)((h >> 8) & 0x1Fu);

                ImU32 col = IM_COL32(lum, lum, lum, alpha);
                draw->AddRectFilled(ImVec2(x, y), ImVec2(x + half_cell, y + half_cell), col);
            }
        }
    }

    static std::vector<RegisteredKeybind> registered_keybinds_t;

    void RegisterKeybind(const char* label, Keybind* bind)
    {
        if (!bind) return;
        for (auto& rb : registered_keybinds_t)
        {
            if (rb.bind == bind)
            {
                rb.label = label;
                return;
            }
        }
        registered_keybinds_t.push_back({ label, bind });
    }

    void ClearKeybindRegistry()
    {
        registered_keybinds_t.clear();
    }

    static std::vector<PlayerInfo> players_t;

    void AddPlayer(const char* name, int health, int armor,
        const char* weapon, const char* rank, bool alive)
    {
        for (auto& s : players_t)
        {
            if (strcmp(s.name, name) == 0)
            {
                s.health = health;
                s.armor = armor;
                s.weapon = weapon;
                s.rank = rank;
                s.alive = alive;
                return;
            }
        }
        PlayerInfo info = {};
        strncpy(info.name, name, sizeof(info.name) - 1);
        info.name[sizeof(info.name) - 1] = '\0';
        info.health = health;
        info.armor = armor;
        info.weapon = weapon;
        info.rank = rank;
        info.time = 0.0f;
        info.alive = alive;
        players_t.push_back(info);
    }

    void RemovePlayer(const char* name)
    {
        for (auto it = players_t.begin(); it != players_t.end(); ++it)
        {
            if (strcmp(it->name, name) == 0)
            {
                players_t.erase(it);
                return;
            }
        }
    }

    void ClearPlayers() { players_t.clear(); }
    int  GetPlayerCount() { return (int)players_t.size(); }

    void DrawPlayerListOverlay(int corner)
    {
        float dt = ImGui::GetIO().DeltaTime;
        for (auto& s : players_t)
            s.time += dt;

        int count = (int)players_t.size();

        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = gui::colors::Get();

        static float s_alpha = 0.0f;
        static float s_accent_w = 0.0f;
        static float s_row_alpha[32] = {};

        s_alpha = Lerp(s_alpha, 1.0f, dt * 6.0f);
        s_accent_w = Lerp(s_accent_w, 1.0f, dt * 4.0f);

        for (int i = 0; i < count && i < 32; i++)
        {
            float delay = (float)i * 0.05f;
            float raw = Clamp(s_alpha - delay, 0.0f, 1.0f);
            s_row_alpha[i] = Lerp(s_row_alpha[i], raw > 0.5f ? 1.0f : 0.0f, dt * 8.0f);
        }

        if (s_alpha < 0.01f) return;
        float alpha = Clamp(s_alpha, 0.0f, 1.0f);

        ImGui::PushFont(ImGui::GetFont());

        float row_h = S(24.0f);
        float title_h = S(24.0f);
        float pad_x = S(12.0f);
        float pad_y = S(6.0f);
        float margin = S(14.0f);
        float rounding = S(3.0f);
        float accent_t = S(2.0f);
        float hp_bar_h = S(3.0f);

        float max_name_w = 0.0f;
        float max_weapon_w = 0.0f;
        float max_time_w = 0.0f;

        char time_bufs[32][16];
        for (int i = 0; i < count && i < 32; i++)
        {
            auto& s = players_t[i];
            float nw = ImGui::CalcTextSize(s.name).x;
            float ww = ImGui::CalcTextSize(s.weapon).x + S(12.0f);

            int mins = (int)(s.time / 60.0f);
            int secs = (int)s.time % 60;
            snprintf(time_bufs[i], sizeof(time_bufs[i]), "%d:%02d", mins, secs);
            float tw = ImGui::CalcTextSize(time_bufs[i]).x;

            if (nw > max_name_w)   max_name_w = nw;
            if (ww > max_weapon_w) max_weapon_w = ww;
            if (tw > max_time_w)   max_time_w = tw;
        }

        char title_with_count[48];
        snprintf(title_with_count, sizeof(title_with_count), "Player List (%d)", count);
        float title_text_w = ImGui::CalcTextSize(title_with_count).x;

        float col_gap = S(14.0f);
        float content_w = max_name_w + col_gap + max_weapon_w + col_gap + max_time_w;
        if (count == 0) content_w = title_text_w;
        float min_w = title_text_w + pad_x * 2.0f;
        float panel_w = (content_w + pad_x * 2.0f > min_w) ? (content_w + pad_x * 2.0f) : min_w;
        if (panel_w < S(160.0f)) panel_w = S(160.0f);

        int display_rows = count > 0 ? count : 1;
        float panel_h = accent_t + title_h + (display_rows * row_h) + pad_y;

        static ImVec2 s_pos = ImVec2(-1.0f, -1.0f);
        static bool   s_dragging = false;
        static ImVec2 s_drag_off;

        if (s_pos.x < 0.0f && s_pos.y < 0.0f)
        {
            switch (corner)
            {
            case 0: s_pos = ImVec2(margin, margin); break;
            case 1: s_pos = ImVec2(io.DisplaySize.x - panel_w - margin, margin); break;
            case 2: s_pos = ImVec2(margin, io.DisplaySize.y - panel_h - margin); break;
            default: s_pos = ImVec2(io.DisplaySize.x - panel_w - margin, io.DisplaySize.y - panel_h - margin); break;
            }
        }

        ImVec2 mouse = io.MousePos;
        bool in_title = (mouse.x >= s_pos.x && mouse.x <= s_pos.x + panel_w &&
            mouse.y >= s_pos.y && mouse.y <= s_pos.y + accent_t + title_h);

        if (in_title && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            s_dragging = true;
            s_drag_off = ImVec2(mouse.x - s_pos.x, mouse.y - s_pos.y);
        }
        if (s_dragging)
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                s_pos.x = mouse.x - s_drag_off.x;
                s_pos.y = mouse.y - s_drag_off.y;
            }
            else
                s_dragging = false;
        }

        s_pos.x = Clamp(s_pos.x, 0.0f, io.DisplaySize.x - panel_w);
        s_pos.y = Clamp(s_pos.y, 0.0f, io.DisplaySize.y - panel_h);

        float px = s_pos.x;
        float py = s_pos.y;

        ImU32 bg_col = BlendAlpha(IM_COL32(12, 12, 12, 235), alpha);
        ImU32 border_col = BlendAlpha(IM_COL32(40, 40, 40, 200), alpha);

        draw->AddRectFilled(ImVec2(px, py), ImVec2(px + panel_w, py + panel_h), bg_col, rounding);
        draw->AddRect(ImVec2(px, py), ImVec2(px + panel_w, py + panel_h), border_col, rounding);

        float acc_full = panel_w - rounding * 2.0f;
        float acc_w = acc_full * Clamp(s_accent_w, 0.0f, 1.0f);
        float acc_x = px + rounding + (acc_full - acc_w) * 0.5f;
        ImU32 acc_col = BlendAlpha(pal.accent, alpha);

        draw->AddRectFilled(ImVec2(acc_x, py), ImVec2(acc_x + acc_w, py + accent_t), acc_col);

        for (int g = 1; g <= 3; g++)
        {
            float e = (float)g * S(1.5f);
            ImU32 gc = BlendAlpha(pal.accent, alpha * 0.12f / (float)g);
            draw->AddRectFilled(ImVec2(acc_x - e, py), ImVec2(acc_x + acc_w + e, py + accent_t + e), gc);
        }

        float title_y = py + accent_t;
        float title_tx = px + (panel_w - title_text_w) * 0.5f;
        float title_ty = title_y + (title_h - ImGui::CalcTextSize(title_with_count).y) * 0.5f;

        draw->AddText(ImVec2(title_tx + 1, title_ty + 1), BlendAlpha(IM_COL32(0, 0, 0, 200), alpha), title_with_count);
        draw->AddText(ImVec2(title_tx, title_ty), BlendAlpha(pal.accent, alpha * 0.9f), title_with_count);

        float sep_y = title_y + title_h - 1.0f;
        draw->AddLine(ImVec2(px + pad_x, sep_y), ImVec2(px + panel_w - pad_x, sep_y),
            BlendAlpha(IM_COL32(45, 45, 45, 180), alpha));

        float row_start_y = title_y + title_h;

        if (count == 0)
        {
            const char* empty_msg = "No players";
            float ew = ImGui::CalcTextSize(empty_msg).x;
            float ey = row_start_y + (row_h - ImGui::CalcTextSize(empty_msg).y) * 0.5f;
            draw->AddText(ImVec2(px + (panel_w - ew) * 0.5f, ey),
                BlendAlpha(IM_COL32(70, 70, 70, 200), alpha), empty_msg);
        }

        float col_name_x = px + pad_x;
        float col_weapon_x = px + pad_x + max_name_w + col_gap;
        float col_time_x = col_weapon_x + max_weapon_w + col_gap;

        for (int i = 0; i < count && i < 32; i++)
        {
            auto& s = players_t[i];
            float ry = row_start_y + (float)i * row_h;
            float row_a = Clamp(s_row_alpha[i], 0.0f, 1.0f) * alpha;

            if (row_a < 0.01f) continue;

            if (i % 2 == 0)
            {
                draw->AddRectFilled(ImVec2(px + 1, ry), ImVec2(px + panel_w - 1, ry + row_h),
                    BlendAlpha(IM_COL32(255, 255, 255, 6), row_a));
            }

            float text_cy = ry + (row_h - ImGui::CalcTextSize(s.name).y) * 0.5f - hp_bar_h * 0.5f;

            ImU32 name_col = s.alive
                ? BlendAlpha(IM_COL32(235, 235, 235, 255), row_a)
                : BlendAlpha(IM_COL32(120, 120, 120, 200), row_a);
            draw->AddText(ImVec2(col_name_x + 1, text_cy + 1), BlendAlpha(IM_COL32(0, 0, 0, 120), row_a), s.name);
            draw->AddText(ImVec2(col_name_x, text_cy), name_col, s.name);

            float bar_y = text_cy + ImGui::CalcTextSize(s.name).y + S(2.0f);
            float bar_max_w = ImGui::CalcTextSize(s.name).x;
            if (bar_max_w < S(30.0f)) bar_max_w = S(30.0f);

            draw->AddRectFilled(
                ImVec2(col_name_x, bar_y),
                ImVec2(col_name_x + bar_max_w, bar_y + hp_bar_h),
                BlendAlpha(IM_COL32(30, 30, 30, 200), row_a), S(1.0f)
            );

            float hp_frac = Clamp((float)s.health / 100.0f, 0.0f, 1.0f);
            ImU32 hp_col;
            if (hp_frac > 0.6f)
                hp_col = BlendAlpha(IM_COL32(80, 200, 100, 255), row_a);
            else if (hp_frac > 0.3f)
                hp_col = BlendAlpha(IM_COL32(230, 180, 50, 255), row_a);
            else
                hp_col = BlendAlpha(IM_COL32(220, 60, 60, 255), row_a);

            if (!s.alive) hp_col = BlendAlpha(IM_COL32(80, 80, 80, 160), row_a);

            if (hp_frac > 0.0f)
            {
                draw->AddRectFilled(
                    ImVec2(col_name_x, bar_y),
                    ImVec2(col_name_x + bar_max_w * hp_frac, bar_y + hp_bar_h),
                    hp_col, S(1.0f)
                );
            }

            if (s.armor > 0)
            {
                float armor_frac = Clamp((float)s.armor / 100.0f, 0.0f, 1.0f);
                draw->AddRectFilled(
                    ImVec2(col_name_x, bar_y - S(1.5f)),
                    ImVec2(col_name_x + bar_max_w * armor_frac, bar_y),
                    BlendAlpha(gui::colors::AccentA(180), row_a), S(0.5f)
                );
            }

            float wep_text_w = ImGui::CalcTextSize(s.weapon).x;
            float wep_text_h = ImGui::CalcTextSize(s.weapon).y;
            float badge_pad = S(5.0f);
            float badge_h = wep_text_h + S(4.0f);
            float badge_w = wep_text_w + badge_pad * 2.0f;
            float badge_y = ry + (row_h - badge_h) * 0.5f;

            draw->AddRectFilled(
                ImVec2(col_weapon_x, badge_y),
                ImVec2(col_weapon_x + badge_w, badge_y + badge_h),
                BlendAlpha(IM_COL32(35, 35, 35, 255), row_a), S(3.0f)
            );
            draw->AddRect(
                ImVec2(col_weapon_x, badge_y),
                ImVec2(col_weapon_x + badge_w, badge_y + badge_h),
                BlendAlpha(IM_COL32(55, 55, 55, 200), row_a), S(3.0f)
            );
            draw->AddText(
                ImVec2(col_weapon_x + badge_pad, badge_y + S(2.0f)),
                BlendAlpha(IM_COL32(170, 170, 170, 255), row_a), s.weapon
            );

            float time_y = ry + (row_h - ImGui::CalcTextSize(time_bufs[i]).y) * 0.5f;
            ImU32 time_col = BlendAlpha(IM_COL32(100, 100, 100, 200), row_a);
            float tw = ImGui::CalcTextSize(time_bufs[i]).x;
            draw->AddText(ImVec2(col_time_x + max_time_w - tw, time_y), time_col, time_bufs[i]);

            if (s.rank && s.rank[0] != '\0')
            {
                float rw = ImGui::CalcTextSize(s.rank).x;
                draw->AddText(
                    ImVec2(col_time_x + max_time_w - rw, time_y + ImGui::CalcTextSize(time_bufs[i]).y + S(1.0f)),
                    BlendAlpha(pal.accent_dim, row_a * 0.6f), s.rank
                );
            }
        }

        draw->AddLine(
            ImVec2(px + rounding, py + panel_h - 1),
            ImVec2(px + panel_w - rounding, py + panel_h - 1),
            BlendAlpha(IM_COL32(0, 0, 0, 40), alpha)
        );

        ImGui::PopFont();
    }

    struct SpectatorEntry { char name[32]; float time; };
    static std::vector<SpectatorEntry> spectator_names_t;

    void AddSpectatorName(const char* name)
    {
        for (auto& s : spectator_names_t)
        {
            if (strcmp(s.name, name) == 0) return;
        }
        SpectatorEntry e = {};
        strncpy(e.name, name, sizeof(e.name) - 1);
        e.name[sizeof(e.name) - 1] = '\0';
        e.time = 0.0f;
        spectator_names_t.push_back(e);
    }

    void RemoveSpectatorName(const char* name)
    {
        for (auto it = spectator_names_t.begin(); it != spectator_names_t.end(); ++it)
        {
            if (strcmp(it->name, name) == 0)
            {
                spectator_names_t.erase(it);
                return;
            }
        }
    }

    void ClearSpectatorNames() { spectator_names_t.clear(); }
    int  GetSpectatorNameCount() { return (int)spectator_names_t.size(); }

    void DrawSpectatorOverlay(int corner)
    {
        float dt = ImGui::GetIO().DeltaTime;
        for (auto& s : spectator_names_t)
            s.time += dt;

        int count = (int)spectator_names_t.size();

        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = gui::colors::Get();

        static float s_alpha = 0.0f;
        static float s_accent_w = 0.0f;
        static float s_row_alpha[32] = {};

        s_alpha = Lerp(s_alpha, 1.0f, dt * 6.0f);
        s_accent_w = Lerp(s_accent_w, 1.0f, dt * 4.0f);

        for (int i = 0; i < count && i < 32; i++)
        {
            float delay = (float)i * 0.05f;
            float raw = Clamp(s_alpha - delay, 0.0f, 1.0f);
            s_row_alpha[i] = Lerp(s_row_alpha[i], raw > 0.5f ? 1.0f : 0.0f, dt * 8.0f);
        }

        if (s_alpha < 0.01f) return;
        float alpha = Clamp(s_alpha, 0.0f, 1.0f);

        ImGui::PushFont(ImGui::GetFont());

        float row_h = S(22.0f);
        float title_h = S(24.0f);
        float pad_x = S(12.0f);
        float pad_y = S(6.0f);
        float margin = S(14.0f);
        float rounding = S(3.0f);
        float accent_t = S(2.0f);
        float dot_r = S(3.0f);
        float dot_gap = S(8.0f);

        char title_buf[48];
        snprintf(title_buf, sizeof(title_buf), "Spectators (%d)", count);
        float title_text_w = ImGui::CalcTextSize(title_buf).x;

        float max_name_w = 0.0f;
        for (int i = 0; i < count && i < 32; i++)
        {
            float nw = ImGui::CalcTextSize(spectator_names_t[i].name).x;
            if (nw > max_name_w) max_name_w = nw;
        }

        char time_bufs[32][16];
        float max_time_w = 0.0f;
        for (int i = 0; i < count && i < 32; i++)
        {
            int mins = (int)(spectator_names_t[i].time / 60.0f);
            int secs = (int)spectator_names_t[i].time % 60;
            snprintf(time_bufs[i], sizeof(time_bufs[i]), "%d:%02d", mins, secs);
            float tw = ImGui::CalcTextSize(time_bufs[i]).x;
            if (tw > max_time_w) max_time_w = tw;
        }

        float col_gap = S(16.0f);
        float content_w = dot_r * 2.0f + dot_gap + max_name_w + col_gap + max_time_w;
        if (count == 0) content_w = title_text_w;
        float min_w = title_text_w + pad_x * 2.0f;
        float panel_w = (content_w + pad_x * 2.0f > min_w) ? (content_w + pad_x * 2.0f) : min_w;
        if (panel_w < S(140.0f)) panel_w = S(140.0f);

        int display_rows = count > 0 ? count : 1;
        float panel_h = accent_t + title_h + (display_rows * row_h) + pad_y;

        static ImVec2 s_pos = ImVec2(-1.0f, -1.0f);
        static bool   s_dragging = false;
        static ImVec2 s_drag_off;

        if (s_pos.x < 0.0f && s_pos.y < 0.0f)
        {
            switch (corner)
            {
            case 0: s_pos = ImVec2(margin, margin); break;
            case 1: s_pos = ImVec2(io.DisplaySize.x - panel_w - margin, margin); break;
            case 2: s_pos = ImVec2(margin, io.DisplaySize.y - panel_h - margin); break;
            default: s_pos = ImVec2(io.DisplaySize.x - panel_w - margin, io.DisplaySize.y - panel_h - margin); break;
            }
        }

        ImVec2 mouse = io.MousePos;
        bool in_title = (mouse.x >= s_pos.x && mouse.x <= s_pos.x + panel_w &&
            mouse.y >= s_pos.y && mouse.y <= s_pos.y + accent_t + title_h);

        if (in_title && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            s_dragging = true;
            s_drag_off = ImVec2(mouse.x - s_pos.x, mouse.y - s_pos.y);
        }
        if (s_dragging)
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                s_pos.x = mouse.x - s_drag_off.x;
                s_pos.y = mouse.y - s_drag_off.y;
            }
            else
                s_dragging = false;
        }

        s_pos.x = Clamp(s_pos.x, 0.0f, io.DisplaySize.x - panel_w);
        s_pos.y = Clamp(s_pos.y, 0.0f, io.DisplaySize.y - panel_h);

        float px = s_pos.x;
        float py = s_pos.y;

        ImU32 bg_col = BlendAlpha(IM_COL32(12, 12, 12, 235), alpha);
        ImU32 border_col = BlendAlpha(IM_COL32(40, 40, 40, 200), alpha);

        draw->AddRectFilled(ImVec2(px, py), ImVec2(px + panel_w, py + panel_h), bg_col, rounding);
        draw->AddRect(ImVec2(px, py), ImVec2(px + panel_w, py + panel_h), border_col, rounding);

        float acc_full = panel_w - rounding * 2.0f;
        float acc_w = acc_full * Clamp(s_accent_w, 0.0f, 1.0f);
        float acc_x = px + rounding + (acc_full - acc_w) * 0.5f;
        ImU32 acc_col = BlendAlpha(pal.accent, alpha);

        draw->AddRectFilled(ImVec2(acc_x, py), ImVec2(acc_x + acc_w, py + accent_t), acc_col);

        for (int g = 1; g <= 3; g++)
        {
            float e = (float)g * S(1.5f);
            ImU32 gc = BlendAlpha(pal.accent, alpha * 0.12f / (float)g);
            draw->AddRectFilled(ImVec2(acc_x - e, py), ImVec2(acc_x + acc_w + e, py + accent_t + e), gc);
        }

        float title_y = py + accent_t;
        float title_tx = px + (panel_w - title_text_w) * 0.5f;
        float title_ty = title_y + (title_h - ImGui::CalcTextSize(title_buf).y) * 0.5f;

        draw->AddText(ImVec2(title_tx + 1, title_ty + 1), BlendAlpha(IM_COL32(0, 0, 0, 200), alpha), title_buf);
        draw->AddText(ImVec2(title_tx, title_ty), BlendAlpha(pal.accent, alpha * 0.9f), title_buf);

        float sep_y = title_y + title_h - 1.0f;
        draw->AddLine(ImVec2(px + pad_x, sep_y), ImVec2(px + panel_w - pad_x, sep_y), BlendAlpha(IM_COL32(45, 45, 45, 180), alpha));

        float row_start_y = title_y + title_h;

        if (count == 0)
        {
            const char* empty_msg = "No spectators";
            float ew = ImGui::CalcTextSize(empty_msg).x;
            float ey = row_start_y + (row_h - ImGui::CalcTextSize(empty_msg).y) * 0.5f;
            draw->AddText(ImVec2(px + (panel_w - ew) * 0.5f, ey), BlendAlpha(IM_COL32(70, 70, 70, 200), alpha), empty_msg);
        }

        for (int i = 0; i < count && i < 32; i++)
        {
            auto& s = spectator_names_t[i];
            float ry = row_start_y + (float)i * row_h;
            float row_a = Clamp(s_row_alpha[i], 0.0f, 1.0f) * alpha;

            if (row_a < 0.01f) continue;

            if (i % 2 == 0)
            {
                draw->AddRectFilled(ImVec2(px + 1, ry), ImVec2(px + panel_w - 1, ry + row_h), BlendAlpha(IM_COL32(255, 255, 255, 6), row_a));
            }

            float text_cy = ry + (row_h - ImGui::CalcTextSize(s.name).y) * 0.5f;

            float dot_cx = px + pad_x + dot_r;
            float dot_cy_pos = text_cy + ImGui::CalcTextSize(s.name).y * 0.5f;
            draw->AddCircleFilled(ImVec2(dot_cx, dot_cy_pos), dot_r, BlendAlpha(pal.accent, row_a * 0.8f));
            draw->AddCircle(ImVec2(dot_cx, dot_cy_pos), dot_r + S(1.5f), BlendAlpha(pal.accent, row_a * 0.15f));

            float name_x = px + pad_x + dot_r * 2.0f + dot_gap;
            draw->AddText(ImVec2(name_x + 1, text_cy + 1), BlendAlpha(IM_COL32(0, 0, 0, 120), row_a), s.name);
            draw->AddText(ImVec2(name_x, text_cy), BlendAlpha(IM_COL32(220, 220, 220, 255), row_a), s.name);

            float tw = ImGui::CalcTextSize(time_bufs[i]).x;
            float time_x = px + panel_w - pad_x - tw;
            float time_y = ry + (row_h - ImGui::CalcTextSize(time_bufs[i]).y) * 0.5f;
            draw->AddText(ImVec2(time_x, time_y), BlendAlpha(IM_COL32(90, 90, 90, 180), row_a), time_bufs[i]);
        }

        draw->AddLine(
            ImVec2(px + rounding, py + panel_h - 1),
            ImVec2(px + panel_w - rounding, py + panel_h - 1),
            BlendAlpha(IM_COL32(0, 0, 0, 40), alpha)
        );

        ImGui::PopFont();
    }

    void DrawHotkeyOverlay(int corner)
    {
        struct BindEntry { const char* label; const char* key; const char* mode; bool active; };
        BindEntry entries[64];
        int count = 0;

        for (auto& rb : registered_keybinds_t)
        {
            if (rb.bind->key == ImGuiKey_None) continue;
            if (count >= 64) break;
            entries[count].label = rb.label;
            entries[count].key = GetKeyName(rb.bind->key);
            entries[count].mode = (rb.bind->mode == KeybindMode::Hold) ? "Hold" : "Toggle";
            entries[count].active = rb.bind->active;
            count++;
        }
        if (count == 0)
            return;

        ImGuiIO& io = ImGui::GetIO();
        ImDrawList* draw = ImGui::GetForegroundDrawList();
        const auto& pal = gui::colors::Get();
        float dt = io.DeltaTime;

        static float s_alpha = 0.0f;
        static float s_accent_w = 0.0f;
        static float s_row_alpha[64] = {};
        static float s_active_glow[64] = {};

        s_alpha = Lerp(s_alpha, 1.0f, dt * 6.0f);
        s_accent_w = Lerp(s_accent_w, 1.0f, dt * 4.0f);

        for (int i = 0; i < count; i++)
        {
            float stagger_target = 1.0f;
            float delay = (float)i * 0.06f;
            float raw = Clamp(s_alpha - delay, 0.0f, 1.0f);
            s_row_alpha[i] = Lerp(s_row_alpha[i], raw > 0.5f ? stagger_target : 0.0f, dt * 8.0f);

            float glow_target = entries[i].active ? 1.0f : 0.0f;
            s_active_glow[i] = Lerp(s_active_glow[i], glow_target, dt * 10.0f);
        }

        if (s_alpha < 0.01f) return;
        float alpha = Clamp(s_alpha, 0.0f, 1.0f);

        ImGui::PushFont(ImGui::GetFont());

        float row_h = S(22.0f);
        float title_h = S(24.0f);
        float pad_x = S(12.0f);
        float pad_y = S(6.0f);
        float margin = S(14.0f);
        float rounding = S(3.0f);
        float accent_t = S(2.0f);
        float key_pad_x = S(6.0f);
        float key_pad_y = S(2.0f);
        float key_rnd = S(3.0f);

        float max_label_w = 0.0f;
        float max_key_w = 0.0f;
        float max_mode_w = 0.0f;

        for (int i = 0; i < count; i++)
        {
            float lw = ImGui::CalcTextSize(entries[i].label).x;
            float kw = ImGui::CalcTextSize(entries[i].key).x + key_pad_x * 2.0f;
            float mw = ImGui::CalcTextSize(entries[i].mode).x;
            if (lw > max_label_w) max_label_w = lw;
            if (kw > max_key_w)   max_key_w = kw;
            if (mw > max_mode_w)  max_mode_w = mw;
        }

        const char* title = "Binds";
        float title_text_w = ImGui::CalcTextSize(title).x;

        float col_gap = S(14.0f);
        float bar_w = S(2.0f);
        float bar_gap = S(6.0f);
        float content_w = bar_w + bar_gap + max_label_w + col_gap + max_key_w + col_gap + max_mode_w;
        float min_title_w = title_text_w + pad_x * 2.0f;
        float panel_w = (content_w + pad_x * 2.0f > min_title_w) ? (content_w + pad_x * 2.0f) : min_title_w;
        float panel_h = accent_t + title_h + (count * row_h) + pad_y;

        static ImVec2 s_pos = ImVec2(-1.0f, -1.0f);
        static bool   s_dragging = false;
        static ImVec2 s_drag_off;

        if (s_pos.x < 0.0f && s_pos.y < 0.0f)
        {
            switch (corner)
            {
            case 0: s_pos = ImVec2(margin, margin); break;
            case 1: s_pos = ImVec2(io.DisplaySize.x - panel_w - margin, margin); break;
            case 2: s_pos = ImVec2(margin, io.DisplaySize.y - panel_h - margin); break;
            default: s_pos = ImVec2(io.DisplaySize.x - panel_w - margin, io.DisplaySize.y - panel_h - margin); break;
            }
        }

        ImVec2 mouse = io.MousePos;
        bool in_title = (mouse.x >= s_pos.x && mouse.x <= s_pos.x + panel_w &&
            mouse.y >= s_pos.y && mouse.y <= s_pos.y + accent_t + title_h);

        if (in_title && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            s_dragging = true;
            s_drag_off = ImVec2(mouse.x - s_pos.x, mouse.y - s_pos.y);
        }
        if (s_dragging)
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                s_pos.x = mouse.x - s_drag_off.x;
                s_pos.y = mouse.y - s_drag_off.y;
            }
            else
                s_dragging = false;
        }

        s_pos.x = Clamp(s_pos.x, 0.0f, io.DisplaySize.x - panel_w);
        s_pos.y = Clamp(s_pos.y, 0.0f, io.DisplaySize.y - panel_h);

        float px = s_pos.x;
        float py = s_pos.y;

        ImU32 bg_col = BlendAlpha(IM_COL32(12, 12, 12, 235), alpha);
        ImU32 border_col = BlendAlpha(IM_COL32(40, 40, 40, 200), alpha);

        draw->AddRectFilled(ImVec2(px, py), ImVec2(px + panel_w, py + panel_h), bg_col, rounding);
        draw->AddRect(ImVec2(px, py), ImVec2(px + panel_w, py + panel_h), border_col, rounding);

        float acc_full = panel_w - rounding * 2.0f;
        float acc_w = acc_full * Clamp(s_accent_w, 0.0f, 1.0f);
        float acc_x = px + rounding + (acc_full - acc_w) * 0.5f;
        ImU32 acc_col = BlendAlpha(pal.accent, alpha);

        draw->AddRectFilled(ImVec2(acc_x, py), ImVec2(acc_x + acc_w, py + accent_t), acc_col);

        for (int g = 1; g <= 3; g++)
        {
            float e = (float)g * S(1.5f);
            ImU32 gc = BlendAlpha(pal.accent, alpha * 0.12f / (float)g);
            draw->AddRectFilled(ImVec2(acc_x - e, py), ImVec2(acc_x + acc_w + e, py + accent_t + e), gc);
        }

        float title_y = py + accent_t;
        float title_tx = px + (panel_w - title_text_w) * 0.5f;
        float title_ty = title_y + (title_h - ImGui::CalcTextSize(title).y) * 0.5f;

        draw->AddText(ImVec2(title_tx + 1, title_ty + 1), BlendAlpha(IM_COL32(0, 0, 0, 200), alpha), title);
        draw->AddText(ImVec2(title_tx, title_ty), BlendAlpha(pal.accent, alpha * 0.9f), title);

        float sep_y = title_y + title_h - 1.0f;
        ImU32 sep_col = BlendAlpha(IM_COL32(45, 45, 45, 180), alpha);
        draw->AddLine(ImVec2(px + pad_x, sep_y), ImVec2(px + panel_w - pad_x, sep_y), sep_col);

        float row_start_y = title_y + title_h;
        float col_label_x = px + pad_x + bar_w + bar_gap;
        float col_key_x = px + pad_x + bar_w + bar_gap + max_label_w + col_gap;
        float col_mode_x = col_key_x + max_key_w + col_gap;

        for (int i = 0; i < count; i++)
        {
            float ry = row_start_y + (float)i * row_h;
            float row_a = Clamp(s_row_alpha[i], 0.0f, 1.0f) * alpha;
            float glow = Clamp(s_active_glow[i], 0.0f, 1.0f);
            bool  active = entries[i].active;

            if (row_a < 0.01f) continue;

            if (i % 2 == 0)
            {
                ImU32 row_bg = BlendAlpha(IM_COL32(255, 255, 255, 6), row_a);
                draw->AddRectFilled(ImVec2(px + 1, ry), ImVec2(px + panel_w - 1, ry + row_h), row_bg);
            }

            if (glow > 0.01f)
            {
                ImU32 bar_col = BlendAlpha(pal.accent, row_a * glow);
                float bar_margin = S(4.0f);
                draw->AddRectFilled(
                    ImVec2(px + pad_x, ry + bar_margin),
                    ImVec2(px + pad_x + bar_w, ry + row_h - bar_margin),
                    bar_col, S(1.0f)
                );

                ImU32 row_glow = BlendAlpha(pal.accent, row_a * glow * 0.04f);
                draw->AddRectFilled(ImVec2(px + 1, ry), ImVec2(px + panel_w - 1, ry + row_h), row_glow);
            }

            float text_cy = ry + (row_h - ImGui::CalcTextSize(entries[i].label).y) * 0.5f;
            ImU32 label_col = active
                ? BlendAlpha(IM_COL32(235, 235, 235, 255), row_a)
                : BlendAlpha(IM_COL32(140, 140, 140, 255), row_a);
            draw->AddText(ImVec2(col_label_x + 1, text_cy + 1), BlendAlpha(IM_COL32(0, 0, 0, 120), row_a), entries[i].label);
            draw->AddText(ImVec2(col_label_x, text_cy), label_col, entries[i].label);

            float kw = ImGui::CalcTextSize(entries[i].key).x;
            float kh = ImGui::CalcTextSize(entries[i].key).y;
            float badge_w = kw + key_pad_x * 2.0f;
            float badge_h = kh + key_pad_y * 2.0f;
            float badge_x = col_key_x;
            float badge_y = ry + (row_h - badge_h) * 0.5f;

            ImU32 badge_bg = active
                ? BlendAlpha(pal.accent_dim, row_a * 0.25f)
                : BlendAlpha(IM_COL32(35, 35, 35, 255), row_a);
            ImU32 badge_border = active
                ? BlendAlpha(pal.accent_dim, row_a * 0.5f)
                : BlendAlpha(IM_COL32(55, 55, 55, 200), row_a);
            ImU32 badge_text = active
                ? BlendAlpha(pal.accent, row_a)
                : BlendAlpha(IM_COL32(160, 160, 160, 255), row_a);

            draw->AddRectFilled(ImVec2(badge_x, badge_y), ImVec2(badge_x + badge_w, badge_y + badge_h), badge_bg, key_rnd);
            draw->AddRect(ImVec2(badge_x, badge_y), ImVec2(badge_x + badge_w, badge_y + badge_h), badge_border, key_rnd);
            draw->AddText(ImVec2(badge_x + key_pad_x, badge_y + key_pad_y), badge_text, entries[i].key);

            float mode_w = ImGui::CalcTextSize(entries[i].mode).x;
            float mode_x = col_mode_x + (max_mode_w - mode_w) * 0.5f;
            float mode_y = ry + (row_h - ImGui::CalcTextSize(entries[i].mode).y) * 0.5f;
            ImU32 mode_col = active
                ? BlendAlpha(IM_COL32(110, 110, 110, 220), row_a)
                : BlendAlpha(IM_COL32(70, 70, 70, 180), row_a);
            draw->AddText(ImVec2(mode_x, mode_y), mode_col, entries[i].mode);
        }

        draw->AddLine(
            ImVec2(px + rounding, py + panel_h - 1),
            ImVec2(px + panel_w - rounding, py + panel_h - 1),
            BlendAlpha(IM_COL32(0, 0, 0, 40), alpha)
        );

        ImGui::PopFont();
    }
}
