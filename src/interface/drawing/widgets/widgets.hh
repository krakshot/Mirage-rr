#pragma once
#include "imgui.h"
#include <unordered_map>
#include <string>

namespace widgets
{
    enum class KeybindMode : int
    {
        Toggle = 0,
        Hold = 1
    };

    struct Keybind
    {
        ImGuiKey key = ImGuiKey_None;
        KeybindMode mode = KeybindMode::Toggle;
        bool active = false;
        bool waiting = false;
    };

    struct KeybindAnim
    {
        float hover = 0.0f;
        float listening = 0.0f;
        float glow = 0.0f;
        float pulse_timer = 0.0f;
    };

    extern std::unordered_map<ImGuiID, KeybindAnim> keybind_anims_t;

    struct WidgetState
    {
        ImGuiID active_slider = 0;
        ImGuiID open_combo = 0;
        int combo_hover_idx = -1;
        float dropdown_x1 = 0, dropdown_y1 = 0;
        float dropdown_x2 = 0, dropdown_y2 = 0;
    };

    struct ComboAnim
    {
        float open = 0.0f;
        float hover = 0.0f;
        float arrow_rot = 0.0f;
        float glow = 0.0f;
        int hover_idx = -1;
        float item_hovers[32] = { 0 };
    };

    struct CheckboxAnim
    {
        float check = 0.0f;
        float hover = 0.0f;
        float click = 0.0f;
    };

    struct SliderAnim
    {
        float hover = 0.0f;
        float active = 0.0f;
        float fill = 0.0f;
        float thumb_scale = 1.0f;
        float glow = 0.0f;
    };

    struct TooltipAnim
    {
        float alpha = 0.0f;
        float offset = 6.0f;
    };

    struct ColorEditAnim
    {
        float hover = 0.0f;
        float open = 0.0f;
        float glow = 0.0f;
        float pulse = 0.0f;
        float scale = 1.0f;
    };

    struct ButtonAnim
    {
        float hover = 0.0f;
        float press = 0.0f;
        float click = 0.0f;
        float glow = 0.0f;
        float scale = 1.0f;
    };

    struct TabAnim
    {
        float hover = 0.0f;
        float active = 0.0f;
        float click = 0.0f;
        float accent_width = 0.0f;
    };

    struct TabTransition
    {
        float alpha = 1.0f;
        int from_tab = -1;
        int to_tab = 0;
        bool transitioning = false;
    };

    extern TabTransition tab_transition_t;

    extern WidgetState state_t;
    extern std::unordered_map<ImGuiID, ComboAnim> combo_anims_t;
    extern std::unordered_map<ImGuiID, CheckboxAnim> checkbox_anims_t;
    extern std::unordered_map<ImGuiID, SliderAnim> slider_anims_t;
    extern std::unordered_map<ImGuiID, TooltipAnim> tooltip_anims_t;
    extern std::unordered_map<ImGuiID, ColorEditAnim> coloredit_anims_t;
    extern std::unordered_map<ImGuiID, ButtonAnim> button_anims_t;
    extern std::unordered_map<ImGuiID, TabAnim> tab_anims_t;

    struct InputTextAnim
    {
        float hover = 0.0f;
        float focus = 0.0f;
        float glow = 0.0f;
        float cursor_blink = 0.0f;
    };

    struct ListboxAnim
    {
        float hover = 0.0f;
        float glow = 0.0f;
        float scroll_anim = 0.0f;
        float scroll_target = 0.0f;
        int hover_idx = -1;
        float item_hovers[64] = { 0 };
        float select_flash = 0.0f;
    };

    struct WatermarkAnim
    {
        float alpha = 0.0f;
        float glow = 0.0f;
        float separator_w = 0.0f;
        float slide = 0.0f;
        float fps_pulse = 0.0f;
        float timer = 0.0f;
        float pos_x = -1.0f;
        float pos_y = -1.0f;
        int last_corner = -1;
    };

    extern std::unordered_map<ImGuiID, InputTextAnim> inputtext_anims_t;
    extern std::unordered_map<ImGuiID, ListboxAnim> listbox_anims_t;

    struct ComboMultiAnim
    {
        float open = 0.0f;
        float hover = 0.0f;
        float arrow_rot = 0.0f;
        float glow = 0.0f;
        float scroll_anim = 0.0f;
        float scroll_target = 0.0f;
        float item_hovers[32] = { 0 };
    };
    extern std::unordered_map<ImGuiID, ComboMultiAnim> combomulti_anims_t;

    struct GroupBoxAnim
    {
        float collapse = 0.0f;
        float hover = 0.0f;
        float arrow_rot = 0.0f;
        bool collapsed = false;
    };
    extern std::unordered_map<ImGuiID, GroupBoxAnim> groupbox_anims_t;

    extern float dpi_scale_t;
    inline float S(float v) { return v * dpi_scale_t; }

    float Lerp(float a, float b, float t);
    float Clamp(float v, float lo, float hi);
    ImU32 BlendAlpha(ImU32 col, float alpha);
    void DrawTextShadow(ImDrawList* draw, ImVec2 pos, ImU32 col, const char* text);
    void DrawHintTooltip(ImGuiID id, float qx, float qy, float qw, float qh, const char* hint);
    const char* GetKeyName(ImGuiKey key);

    void ProcessKeybinds();

    bool DrawKeybind(Keybind* bind, float x, float y, float max_w);

    void DrawWatermark(bool visible, int position = 1, const char* name = "ngui");

    void DrawBackgroundNoise(ImDrawList* draw, ImVec2 min, ImVec2 max, float intensity = 0.06f, float cell_size = 4.0f);

    void SpawnConfetti(float cx, float cy, int count = 40);
    void UpdateAndDrawParticles(ImDrawList* draw);

    struct RegisteredKeybind
    {
        const char* label;
        Keybind* bind;
    };
    void RegisterKeybind(const char* label, Keybind* bind);
    void ClearKeybindRegistry();
    void DrawHotkeyOverlay(int corner = 3);

    struct PlayerInfo
    {
        char name[32];
        int health;
        int armor;
        const char* weapon; //faje player inf struct cs how tf am i supposed to showcase it 
        const char* rank;
        float time;
        bool alive;
    };

    void AddPlayer(const char* name, int health = 100, int armor = 0, const char* weapon = "Unknown", const char* rank = "", bool alive = true);
    void RemovePlayer(const char* name);
    void ClearPlayers();
    int  GetPlayerCount();
    void DrawPlayerListOverlay(int corner = 2);

    void AddSpectatorName(const char* name);
    void RemoveSpectatorName(const char* name);
    void ClearSpectatorNames();
    int  GetSpectatorNameCount();
    void DrawSpectatorOverlay(int corner = 2);

    enum class ToastType { Info, Success, Warning, Error };
    void PushNotification(const char* title, const char* message, ToastType type = ToastType::Info, float duration = 3.5f);
    void ClearNotifications();
    void DrawNotifications();

    float CalcGroupBoxHeight(int widget_count, const char* label = nullptr);
    bool BeginGroupBox(const char* label, float width, int widget_count, bool collapsible = true);
    void EndGroupBox();

    bool TabButton(const char* label, bool active, float width, float height);
    bool Checkbox(const char* label, bool* value, const char* hint = nullptr, Keybind* bind = nullptr);
    bool SliderInt(const char* label, int* value, int v_min, int v_max, const char* hint = nullptr, Keybind* bind = nullptr);
    bool SliderFloat(const char* label, float* value, float v_min, float v_max, const char* format = "%.1f", const char* hint = nullptr, Keybind* bind = nullptr);
    bool Combo(const char* label, int* current, const char* const* items, int items_count, const char* hint = nullptr, Keybind* bind = nullptr);
    bool Button(const char* label, float width = 0.0f, float height = 0.0f);
    bool InputText(const char* label, char* buf, size_t buf_size, const char* hint = nullptr);
    bool ColorEdit(const char* label, float col[4], const char* hint = nullptr, Keybind* bind = nullptr);
    bool Listbox(const char* label, int* current, const char* const* items, int items_count, int visible_count = 5, const char* hint = nullptr);
    bool ComboMulti(const char* label, bool* selected, const char* const* items, int items_count, const char* hint = nullptr);
    bool ReorderList(const char* label, int* order, const char* const* items, int items_count);

    extern int group_depth_t;
}
