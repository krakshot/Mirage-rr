#include "gui.hh"

#include "../drawing/widgets/widgets.hh"
#include "../ressources/font/font.hh"
#include "../interface/sets/Colors.hh"
#include "../../Utils/Logger/Logger.hpp"

#include "imgui.h"
#include <cstring>
#include <cctype>
#include "../../Render/Overlay.hpp"
#include <ranges>
#include <filesystem>

GUI* ngui = new GUI();

static bool watermark_enabled_t = true;
static int watermark_pos_t = 1;
static float dpi_preview_t = 1.0f;
static bool hotkey_overlay_enabled_t = true;
static bool playerlist_overlay_enabled_t = false;
static bool spectator_overlay_enabled_t = false;
static bool confetti_on_tab_t = false;

static char search_buf_t[64] = "";
static bool search_focused_t = false;
static float search_open_anim_t = 0.0f;

struct SearchEntry
{
	const char* label;
	const char* group;
	int tab;
};

static const SearchEntry search_entries_t[] = {
	{ "Enable", "General", 0 },
	{ "Silent", "General", 0 },
	{ "Team check", "General", 0 },
	{ "FOV", "General", 0 },
	{ "Hitbox", "Targeting", 0 },
	{ "Smooth", "Targeting", 0 },
	{ "Delay", "Targeting", 0 },
	{ "Preset", "Weapons", 0 },
	{ "Min dmg", "Weapons", 0 },
	{ "Auto wall", "Weapons", 0 },
	{ "Auto stop", "Weapons", 0 },
	{ "Remove recoil", "Behavior", 0 },
	{ "Scope only", "Behavior", 0 },
	{ "Box", "Players", 1 },
	{ "Name", "Players", 1 },
	{ "Health", "Players", 1 },
	{ "Flags", "Players", 1 },
	{ "Max dist", "Players", 1 },
	{ "Dropped items", "World", 1 },
	{ "Projectiles", "World", 1 },
	{ "Objective", "World", 1 },
	{ "Glow", "Style", 1 },
	{ "Opacity", "Style", 1 },
	{ "Outlines", "Style", 1 },
	{ "Elements", "ESP Elements", 1 },
	{ "Auto jump", "Movement", 2 },
	{ "Auto strafe", "Movement", 2 },
	{ "Speed", "Movement", 2 },
	{ "Edge dist", "Movement", 2 },
	{ "Watermark", "Utility", 2 },
	{ "WM pos", "Utility", 2 },
	{ "Player List", "Utility", 2 },
	{ "Spectators", "Utility", 2 },
	{ "Hotkey overlay","Utility", 2 },
	{ "Confetti on tab","Utility", 2 },
	{ "Clan tag", "Utility", 2 },
	{ "View FOV", "Visuals Misc", 2 },
	{ "Aspect", "Visuals Misc", 2 },
	{ "VM FOV", "Visuals Misc", 2 },
	{ "Third person", "Visuals Misc", 2 },
	{ "TP dist", "Visuals Misc", 2 },
	{ "Enemy", "Colors", 2 },
	{ "Team", "Colors", 2 },
	{ "Weapon", "Colors", 2 },
	{ "Accent", "Colors", 2 },
	{ "Profile", "Config", 3 },
	{ "Auto save", "Config", 3 },
	{ "Menu key", "Binds", 3 },
	{ "Aim key", "Binds", 3 },
	{ "Panic key", "Binds", 3 },
	{ "Hold to aim", "Binds", 3 },
	{ "UI scale", "Theme", 3 },
	{ "Density", "Theme", 3 },
	{ "Show FPS", "Debug", 3 },
	{ "Show ping", "Debug", 3 },
	{ "Log level", "Debug", 3 },
};
static const int search_entry_count_t = sizeof(search_entries_t) / sizeof(search_entries_t[0]);
static const char* tab_names_t[] = { "Aimbot", "Visuals", "Misc", "Config" };

static bool StrContainsInsensitive(const char* haystack, const char* needle)
{
	if (!haystack || !needle || needle[0] == '\0') return false;
	for (const char* h = haystack; *h; h++)
	{
		const char* hh = h;
		const char* nn = needle;
		while (*hh && *nn && (tolower((unsigned char)*hh) == tolower((unsigned char)*nn)))
		{
			hh++;
			nn++;
		}
		if (*nn == '\0') return true;
	}
	return false;
}

void GUI::init()
{
	if (m_inited) { return; }

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 0.0f;
	style.ChildRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.WindowBorderSize = 0.0f;
	style.ChildBorderSize = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 6.0f);
	style.WindowPadding = ImVec2(10.0f, 10.0f);

	gui::font::Init();
	m_inited = true;
}

void GUI::render()
{
	if (!m_inited)
	{
		return;
	}

	ImGuiIO& io = ImGui::GetIO();


	io.FontGlobalScale = widgets::dpi_scale_t;

	widgets::DrawWatermark(watermark_enabled_t, watermark_pos_t, "mirage");

	static bool s_demo_specs_init = false;
	if (!s_demo_specs_init)
	{
		s_demo_specs_init = true;
		widgets::AddPlayer("xViper", 82, 45, "AK-47", "GN3", true);
		widgets::AddPlayer("sk_phantom", 100, 0, "AWP", "DMG", true);
		widgets::AddPlayer("n0thing2lose", 14, 0, "Deagle", "SEM", true);
		widgets::AddPlayer("deadmeat42", 0, 0, "Knife", "S1", false);

		widgets::AddSpectatorName("sk_phantom");
		widgets::AddSpectatorName("deadmeat42");
		widgets::AddSpectatorName("cl0ut_chaser");
	}

	if (hotkey_overlay_enabled_t)
		widgets::DrawHotkeyOverlay(3);

	if (playerlist_overlay_enabled_t)
		widgets::DrawPlayerListOverlay(2);

	if (spectator_overlay_enabled_t)
		widgets::DrawSpectatorOverlay(2);

	widgets::DrawNotifications();

	widgets::ProcessKeybinds();

	if (Overlay::menuOpen)
	{
		const float dpi = widgets::dpi_scale_t;
		const ImVec2 window_size = ImVec2(520.0f * dpi, 420.0f * dpi);
		ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
		ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(center.x - window_size.x * 0.5f, center.y - window_size.y * 0.5f), ImGuiCond_Once);

		ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoMove;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(10, 10, 10, 255));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("##mirage", &Overlay::menuOpen, flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		ImVec2 pos = ImGui::GetWindowPos();
		ImVec2 size = ImGui::GetWindowSize();
		ImDrawList* draw = ImGui::GetWindowDrawList();

		const float header_height = 28.0f * dpi;
		const float sidebar_width = 90.0f * dpi;
		const float pad = 8.0f * dpi;

		draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(10, 10, 10, 255));

		widgets::DrawBackgroundNoise(draw, pos, ImVec2(pos.x + size.x, pos.y + size.y), 0.06f, 4.0f);

		draw->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(5, 5, 5, 255));
		draw->AddRect(ImVec2(pos.x + 1, pos.y + 1), ImVec2(pos.x + size.x - 1, pos.y + size.y - 1), IM_COL32(35, 35, 35, 255));

		draw->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + header_height), IM_COL32(15, 15, 15, 255));
		draw->AddLine(ImVec2(pos.x, pos.y + header_height), ImVec2(pos.x + size.x, pos.y + header_height), IM_COL32(35, 35, 35, 255));

		{
			ImVec2 mouse = io.MousePos;
			bool in_header = (mouse.x >= pos.x && mouse.x <= pos.x + size.x &&
				mouse.y >= pos.y && mouse.y <= pos.y + header_height);
			static bool dragging = false;
			static ImVec2 drag_offset;

			bool widget_active = (widgets::state_t.active_slider != 0 || widgets::state_t.open_combo != 0);

			float search_zone_x = pos.x + size.x - 160.0f * dpi;
			bool in_search_zone = (mouse.x >= search_zone_x && mouse.y >= pos.y && mouse.y <= pos.y + header_height);

			if (in_header && !in_search_zone && ImGui::IsMouseClicked(0) && !widget_active && !search_focused_t)
			{
				dragging = true;
				drag_offset = ImVec2(mouse.x - pos.x, mouse.y - pos.y);
			}

			if (dragging)
			{
				if (ImGui::IsMouseDown(0))
				{
					ImGui::SetWindowPos(ImVec2(mouse.x - drag_offset.x, mouse.y - drag_offset.y));
				}
				else
				{
					dragging = false;
				}
			}
		}

		draw->AddRectFilled(ImVec2(pos.x + 2, pos.y + 2), ImVec2(pos.x + size.x - 2, pos.y + 2 + 2.0f * dpi), gui::colors::Get().accent);

		draw->AddRectFilled(ImVec2(pos.x, pos.y + header_height), ImVec2(pos.x + sidebar_width, pos.y + size.y), IM_COL32(12, 12, 12, 255));
		draw->AddLine(ImVec2(pos.x + sidebar_width, pos.y + header_height), ImVec2(pos.x + sidebar_width, pos.y + size.y), IM_COL32(35, 35, 35, 255));

		if (gui::font::Get().title)
			ImGui::PushFont(gui::font::Get().title);
		draw->AddText(ImVec2(pos.x + 12.0f * dpi + 1, pos.y + 8.0f * dpi + 1), IM_COL32(0, 0, 0, 180), "mirage");
		draw->AddText(ImVec2(pos.x + 12.0f * dpi, pos.y + 8.0f * dpi), IM_COL32(255, 255, 255, 255), "mirage");
		if (gui::font::Get().title)
			ImGui::PopFont();

		{
			float search_w = 140.0f * dpi;
			float search_h = 16.0f * dpi;
			float sx = pos.x + size.x - search_w - 10.0f * dpi;
			float sy = pos.y + (header_height - search_h) * 0.5f;

			float dt_s = io.DeltaTime;
			bool has_text = search_buf_t[0] != '\0';
			float open_target = (search_focused_t || has_text) ? 1.0f : 0.0f;
			search_open_anim_t += (open_target - search_open_anim_t) * widgets::Clamp(dt_s * 10.0f, 0.0f, 1.0f);

			draw->AddRectFilled(ImVec2(sx, sy), ImVec2(sx + search_w, sy + search_h), IM_COL32(20, 20, 20, 220), 3.0f);
			draw->AddRect(ImVec2(sx, sy), ImVec2(sx + search_w, sy + search_h), IM_COL32(45, 45, 45, 255), 3.0f);

			float icon_cx = sx + 8.0f * dpi;
			float icon_cy = sy + search_h * 0.5f;
			float icon_r = 3.0f * dpi;
			draw->AddCircle(ImVec2(icon_cx, icon_cy), icon_r, IM_COL32(100, 100, 100, 200), 8, 1.2f);
			draw->AddLine(ImVec2(icon_cx + icon_r * 0.7f, icon_cy + icon_r * 0.7f), ImVec2(icon_cx + icon_r * 1.6f, icon_cy + icon_r * 1.6f), IM_COL32(100, 100, 100, 200), 1.2f);

			float input_x = sx + 16.0f * dpi;
			float input_w = search_w - 20.0f * dpi;
			ImGui::SetCursorScreenPos(ImVec2(input_x, sy));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, (search_h - ImGui::GetTextLineHeight()) * 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(190, 190, 190, 255));
			ImGui::PushItemWidth(input_w);
			ImGui::InputText("##search", search_buf_t, sizeof(search_buf_t));
			search_focused_t = ImGui::IsItemActive();
			ImGui::PopItemWidth();
			ImGui::PopStyleColor(2);
			ImGui::PopStyleVar(3);

			if (!has_text && !search_focused_t)
			{
				draw->AddText(ImVec2(input_x + 2, sy + (search_h - ImGui::GetTextLineHeight()) * 0.5f), IM_COL32(70, 70, 70, 200), "Search...");
			}

			if (has_text && search_open_anim_t > 0.01f)
			{
				struct Match { int idx; };
				Match matches[20];
				int match_count = 0;

				for (int i = 0; i < search_entry_count_t && match_count < 20; i++)
				{
					if (StrContainsInsensitive(search_entries_t[i].label, search_buf_t) || StrContainsInsensitive(search_entries_t[i].group, search_buf_t))
					{
						matches[match_count++] = { i };
					}
				}

				if (match_count > 0)
				{
					float row_h = 18.0f * dpi;

					float max_text_w = search_w;
					for (int m = 0; m < match_count; m++)
					{
						const SearchEntry& e = search_entries_t[matches[m].idx];
						char badge[16];
						snprintf(badge, sizeof(badge), "%s", tab_names_t[e.tab]);
						float badge_w = ImGui::CalcTextSize(badge).x + 6.0f * dpi;
						float label_w = ImGui::CalcTextSize(e.label).x;
						float grp_w = ImGui::CalcTextSize(e.group).x;
						float line_w = badge_w + 10.0f * dpi + label_w + 16.0f * dpi + grp_w + 6.0f * dpi;
						if (line_w > max_text_w) max_text_w = line_w;
					}

					float dd_w = max_text_w + 4.0f * dpi;
					float dd_h = match_count * row_h + 4.0f * dpi;
					float dd_x = sx + search_w - dd_w;
					if (dd_x < pos.x) dd_x = pos.x;
					float dd_y = sy + search_h + 2.0f * dpi;

					float visible_h = dd_h * search_open_anim_t;

					ImDrawList* fg = ImGui::GetForegroundDrawList();
					fg->PushClipRect(ImVec2(dd_x, dd_y), ImVec2(dd_x + dd_w, dd_y + visible_h), true);

					fg->AddRectFilled(ImVec2(dd_x, dd_y), ImVec2(dd_x + dd_w, dd_y + dd_h), IM_COL32(15, 15, 15, 240), 3.0f);
					fg->AddRect(ImVec2(dd_x, dd_y), ImVec2(dd_x + dd_w, dd_y + dd_h), IM_COL32(45, 45, 45, 255), 3.0f);

					ImVec2 mouse_s = io.MousePos;
					for (int m = 0; m < match_count; m++)
					{
						const SearchEntry& e = search_entries_t[matches[m].idx];
						float ry = dd_y + 2.0f * dpi + m * row_h;

						bool row_hovered = (mouse_s.x >= dd_x && mouse_s.x <= dd_x + dd_w &&
							mouse_s.y >= ry && mouse_s.y <= ry + row_h);

						if (row_hovered)
						{
							fg->AddRectFilled(ImVec2(dd_x + 1, ry), ImVec2(dd_x + dd_w - 1, ry + row_h), gui::colors::AccentBg(0.22f, 255));

							if (ImGui::IsMouseClicked(0))
							{
								m_tab = e.tab;
								search_buf_t[0] = '\0';
							}
						}

						char badge[16];
						snprintf(badge, sizeof(badge), "%s", tab_names_t[e.tab]);
						float badge_w = ImGui::CalcTextSize(badge).x + 6.0f * dpi;

						fg->AddRectFilled(ImVec2(dd_x + 4.0f * dpi, ry + 2.0f * dpi), ImVec2(dd_x + 4.0f * dpi + badge_w, ry + row_h - 2.0f * dpi), gui::colors::AccentBg(0.30f, 255), 2.0f);

						fg->AddText(ImVec2(dd_x + 7.0f * dpi, ry + (row_h - ImGui::GetTextLineHeight()) * 0.5f), gui::colors::Get().accent, badge);

						fg->AddText(ImVec2(dd_x + badge_w + 10.0f * dpi, ry + (row_h - ImGui::GetTextLineHeight()) * 0.5f), IM_COL32(200, 200, 200, 255), e.label);

						float grp_w = ImGui::CalcTextSize(e.group).x;
						fg->AddText(ImVec2(dd_x + dd_w - grp_w - 6.0f * dpi, ry + (row_h - ImGui::GetTextLineHeight()) * 0.5f), IM_COL32(80, 80, 80, 200), e.group);
					}

					fg->PopClipRect();
				}
				else
				{
					ImDrawList* fg = ImGui::GetForegroundDrawList();
					float dd_x = sx;
					float dd_y = sy + search_h + 2.0f * dpi;
					float dd_w = search_w;
					float dd_h = 20.0f * dpi;
					fg->AddRectFilled(ImVec2(dd_x, dd_y), ImVec2(dd_x + dd_w, dd_y + dd_h),
						IM_COL32(15, 15, 15, 240), 3.0f);
					fg->AddRect(ImVec2(dd_x, dd_y), ImVec2(dd_x + dd_w, dd_y + dd_h),
						IM_COL32(45, 45, 45, 255), 3.0f);
					fg->AddText(ImVec2(dd_x + 8.0f * dpi, dd_y + (dd_h - ImGui::GetTextLineHeight()) * 0.5f),
						IM_COL32(80, 80, 80, 200), "No results");
				}
			}
		}

		ImGui::SetCursorPos(ImVec2(6.0f * dpi, header_height + 8.0f * dpi));
		ImGui::BeginGroup();
		{
			const float tab_w = sidebar_width - 12.0f * dpi;
			const float tab_h = 24.0f * dpi;
			int new_tab = m_tab;
			if (widgets::TabButton("Aimbot", m_tab == 0, tab_w, tab_h)) { new_tab = 0; }
			if (widgets::TabButton("Visuals", m_tab == 1, tab_w, tab_h)) { new_tab = 1; }
			if (widgets::TabButton("Misc", m_tab == 2, tab_w, tab_h)) { new_tab = 2; }
			if (widgets::TabButton("Config", m_tab == 3, tab_w, tab_h)) { new_tab = 3; }

			if (new_tab != m_tab)
			{
				widgets::tab_transition_t.from_tab = m_tab;
				widgets::tab_transition_t.to_tab = new_tab;
				widgets::tab_transition_t.transitioning = true;
				widgets::tab_transition_t.alpha = 0.0f;

				if (confetti_on_tab_t)
				{
					ImVec2 wpos = ImGui::GetWindowPos();
					ImVec2 wsz = ImGui::GetWindowSize();
					widgets::SpawnConfetti(wpos.x + wsz.x * 0.5f, wpos.y + wsz.y * 0.4f, 35);
				}

				m_tab = new_tab;
			}
		}
		ImGui::EndGroup();

		float dt = io.DeltaTime;
		if (widgets::tab_transition_t.transitioning)
		{
			widgets::tab_transition_t.alpha += dt * 8.0f;
			if (widgets::tab_transition_t.alpha >= 1.0f)
			{
				widgets::tab_transition_t.alpha = 1.0f;
				widgets::tab_transition_t.transitioning = false;
			}
		}
		else
		{
			widgets::tab_transition_t.alpha = 1.0f;
		}

		float content_alpha = widgets::tab_transition_t.alpha;

		ImVec2 content_pos = ImVec2(sidebar_width + pad, header_height + pad);
		float content_w = size.x - sidebar_width - pad * 2.0f;
		float content_h = size.y - header_height - pad * 2.0f;
		const float col_gap = 8.0f * dpi;
		const float col_w = (content_w - col_gap) * 0.5f;

		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, content_alpha);

		if (m_tab == 0)
		{
			const float box_gap = 8.0f * dpi;
			float general_h = widgets::CalcGroupBoxHeight(4, "General");
			float targeting_h = widgets::CalcGroupBoxHeight(3, "Targeting");
			float weapons_h = widgets::CalcGroupBoxHeight(4, "Weapons");

			ImGui::SetCursorPos(content_pos);
			if (widgets::BeginGroupBox("General", col_w, 4))
			{
				static bool enable = true;
				static bool silent = false;
				static bool team = true;
				static widgets::Keybind enable_bind = { ImGuiKey_None, widgets::KeybindMode::Toggle };
				static widgets::Keybind silent_bind = { ImGuiKey_None, widgets::KeybindMode::Hold };
				widgets::Checkbox("Enable", &enable, "Master switch for all aim features", &enable_bind);
				widgets::Checkbox("Silent", &silent, "Do not move view while adjusting aim", &silent_bind);
				widgets::Checkbox("Team check", &team, "Ignore teammates");
				static int fov = 90;
				widgets::SliderInt("FOV", &fov, 1, 180, "Limit for target selection");
			}
			widgets::EndGroupBox();

			ImGui::SetCursorPos(ImVec2(content_pos.x, content_pos.y + general_h + box_gap));
			if (widgets::BeginGroupBox("Targeting", col_w, 3))
			{
				static int hitbox = 0;
				const char* hitboxes[] = { "Head", "Neck", "Chest", "Stomach" };
				widgets::Combo("Hitbox", &hitbox, hitboxes, 4, "Preferred hitbox");
				static int smoothing = 35;
				widgets::SliderInt("Smooth", &smoothing, 1, 100, "Lower is snappier");
				static int delay = 60;
				widgets::SliderInt("Delay", &delay, 0, 200, "Add a small reaction delay");
			}
			widgets::EndGroupBox();

			ImGui::SetCursorPos(ImVec2(content_pos.x + col_w + col_gap, content_pos.y));
			if (widgets::BeginGroupBox("Weapons", col_w, 4))
			{
				static int weapon = 1;
				const char* weapons[] = { "Pistol", "Rifle", "Sniper", "SMG" };
				widgets::Combo("Preset", &weapon, weapons, 4, "Per-weapon presets");
				static int min_dmg = 28;
				widgets::SliderInt("Min dmg", &min_dmg, 1, 100, "Only fire above this");
				static bool autowall = false;
				static widgets::Keybind autowall_bind = { ImGuiKey_None, widgets::KeybindMode::Toggle };
				widgets::Checkbox("Auto wall", &autowall, "Allow wall penetration", &autowall_bind);
				static bool auto_stop = true;
				widgets::Checkbox("Auto stop", &auto_stop, "Stop before firing");
			}
			widgets::EndGroupBox();

			ImGui::SetCursorPos(ImVec2(content_pos.x + col_w + col_gap, content_pos.y + weapons_h + box_gap));
			if (widgets::BeginGroupBox("Behavior", col_w, 2))
			{
				static bool recoil = true;
				static bool scope_only = false;
				widgets::Checkbox("Remove recoil", &recoil, "Compensate view kick");
				widgets::Checkbox("Scope only", &scope_only, "Only when scoped");
			}
			widgets::EndGroupBox();
		}
		else if (m_tab == 1)
		{
			const float box_gap = 8.0f * dpi;

			ImGui::SetCursorPos(content_pos);
			if (widgets::BeginGroupBox("Players", col_w, 5))
			{
				static bool box = true;
				static bool name = true;
				static bool health = true;
				static widgets::Keybind esp_bind = { ImGuiKey_None, widgets::KeybindMode::Hold };
				widgets::Checkbox("Box", &box, "Draw a 2D box", &esp_bind);
				widgets::Checkbox("Name", &name, "Player name label");
				widgets::Checkbox("Health", &health, "Health bar");
				static bool flags_sel[5] = { true, false, true, false, false };
				const char* flags_items[] = { "Armor", "Kit", "Scoped", "Flashed", "Money" };
				widgets::ComboMulti("Flags", flags_sel, flags_items, 5, "Info flags to display");
				static int max_dist = 250;
				widgets::SliderInt("Max dist", &max_dist, 50, 500, "Limit ESP distance");
			}
			widgets::EndGroupBox();

			float players_h = widgets::CalcGroupBoxHeight(5, "Players");
			ImGui::SetCursorPos(ImVec2(content_pos.x, content_pos.y + players_h + box_gap));
			if (widgets::BeginGroupBox("World", col_w, 3))
			{
				static bool dropped = true;
				static bool projectiles = false;
				static bool bomb = true;
				widgets::Checkbox("Dropped items", &dropped, "Show weapons and items");
				widgets::Checkbox("Projectiles", &projectiles, "Show grenades");
				widgets::Checkbox("Objective", &bomb, "Highlight objective");
			}
			widgets::EndGroupBox();

			ImGui::SetCursorPos(ImVec2(content_pos.x + col_w + col_gap, content_pos.y));
			if (widgets::BeginGroupBox("Style", col_w, 3))
			{
				static int glow = 20;
				static int opacity = 85;
				widgets::SliderInt("Glow", &glow, 0, 100, "Glow strength");
				widgets::SliderInt("Opacity", &opacity, 20, 100, "ESP alpha");
				static bool outlines = true;
				widgets::Checkbox("Outlines", &outlines, "Outline on text");
			}
			widgets::EndGroupBox();

			float style_h = widgets::CalcGroupBoxHeight(3, "Style");
			ImGui::SetCursorPos(ImVec2(content_pos.x + col_w + col_gap, content_pos.y + style_h + box_gap));
			if (widgets::BeginGroupBox("ESP Elements", col_w, 6))
			{
				static int esp_element = 0;
				const char* esp_elements[] = { "Bounding Box", "Skeleton", "Glow", "Snaplines", "Head Dot", "Barrel", "Flags", "Armor Bar" };
				widgets::Listbox("Elements", &esp_element, esp_elements, 8, 5, "Select ESP element to configure");
			}
			widgets::EndGroupBox();
		}
		else if (m_tab == 2)
		{
			const float box_gap = 8.0f * dpi;
			float movement_h = widgets::CalcGroupBoxHeight(4, "Movement");
			float utility_h = widgets::CalcGroupBoxHeight(9, "Utility");

			ImGui::SetCursorPos(content_pos);
			if (widgets::BeginGroupBox("Movement", col_w, 4))
			{
				static bool bhop = true;
				static bool strafe = true;
				static widgets::Keybind bhop_bind = { ImGuiKey_None, widgets::KeybindMode::Hold };
				static widgets::Keybind strafe_bind = { ImGuiKey_None, widgets::KeybindMode::Toggle };
				widgets::Checkbox("Auto jump", &bhop, "Hold jump to bunny hop", &bhop_bind);
				widgets::Checkbox("Auto strafe", &strafe, "Air strafe assist", &strafe_bind);
				static float speed_mult = 1.0f;
				widgets::SliderFloat("Speed", &speed_mult, 0.5f, 2.0f, "%.2f", "Movement speed multiplier");
				static float edge_dist = 3.5f;
				widgets::SliderFloat("Edge dist", &edge_dist, 0.0f, 10.0f, "%.1f", "Edge jump distance");
			}
			widgets::EndGroupBox();

			ImGui::SetCursorPos(ImVec2(content_pos.x, content_pos.y + movement_h + box_gap));
			if (widgets::BeginGroupBox("Utility", col_w, 10))
			{
				widgets::Checkbox("Watermark", &watermark_enabled_t, "Corner brand label");
				static const char* wm_positions[] = { "Top Left", "Top Right", "Bottom Left", "Bottom Right" };
				widgets::Combo("WM pos", &watermark_pos_t, wm_positions, 4, "Watermark corner");
				widgets::Checkbox("Player List", &playerlist_overlay_enabled_t, "Show player list overlay");
				widgets::Checkbox("Spectators", &spectator_overlay_enabled_t, "Show spectator overlay");
				widgets::Checkbox("Hotkey overlay", &hotkey_overlay_enabled_t, "Show active keybinds");
				widgets::Checkbox("Confetti on tab", &confetti_on_tab_t, "Burst confetti on tab switch");
				if (widgets::Button("Confetti!", col_w - 16.0f * dpi, 20.0f * dpi))
				{
					ImVec2 wpos = ImGui::GetWindowPos();
					ImVec2 wsz = ImGui::GetWindowSize();
					widgets::SpawnConfetti(wpos.x + wsz.x * 0.5f, wpos.y + wsz.y * 0.5f, 50);
				}
				static char clan_tag[32] = "mirage";
				widgets::InputText("Clan tag", clan_tag, sizeof(clan_tag), "Custom clan tag");
				static char name_buf[32] = "Player";
				widgets::InputText("Name", name_buf, sizeof(name_buf), "Custom name override");
				if (widgets::Button("Apply changes", col_w - 16.0f * dpi, 20.0f * dpi))
				{
				}
			}
			widgets::EndGroupBox();

			ImGui::SetCursorPos(ImVec2(content_pos.x + col_w + col_gap, content_pos.y));
			if (widgets::BeginGroupBox("Visuals Misc", col_w, 5))
			{
				static float fov_value = 90.0f;
				widgets::SliderFloat("View FOV", &fov_value, 60.0f, 140.0f, "%.0f", "Camera field of view");
				static float aspect = 1.0f;
				widgets::SliderFloat("Aspect", &aspect, 0.5f, 2.0f, "%.2f", "Screen aspect ratio");
				static float viewmodel_fov = 68.0f;
				widgets::SliderFloat("VM FOV", &viewmodel_fov, 54.0f, 110.0f, "%.0f", "Viewmodel field of view");
				static bool third_person = false;
				widgets::Checkbox("Third person", &third_person, "Toggle third person view");
				static int tp_distance = 150;
				widgets::SliderInt("TP dist", &tp_distance, 50, 300, "Third person camera distance");
			}
			widgets::EndGroupBox();

			float visuals_misc_h = widgets::CalcGroupBoxHeight(5, "Visuals Misc");
			ImGui::SetCursorPos(ImVec2(content_pos.x + col_w + col_gap, content_pos.y + visuals_misc_h + box_gap));
			if (widgets::BeginGroupBox("Colors", col_w, 4))
			{
				static float col_enemy[4] = { 1.0f, 0.3f, 0.3f, 1.0f };
				static float col_team[4] = { 0.3f, 0.8f, 0.3f, 1.0f };
				static float col_weapon[4] = { 1.0f, 0.9f, 0.4f, 1.0f };
				static float col_accent[4] = { 0.4f, 0.5f, 0.9f, 1.0f };
				widgets::ColorEdit("Enemy", col_enemy, "Enemy highlight color");
				widgets::ColorEdit("Team", col_team, "Team highlight color");
				widgets::ColorEdit("Weapon", col_weapon, "Dropped weapon color");
				widgets::ColorEdit("Accent", col_accent, "UI accent color");
			}
			widgets::EndGroupBox();
		}
		else if (m_tab == 3)
		{
			const float box_gap = 8.0f * dpi;
			float config_h = widgets::CalcGroupBoxHeight(5, "Config");

			ImGui::SetCursorPos(content_pos);
			if (widgets::BeginGroupBox("Config", col_w, 5))
			{
				static int config = 0;
				const char* configs[] = { "default", "legit", "rage", "sniper", "hvh" };
				widgets::Combo("Profile", &config, configs, 5, "Select a profile");
				static char config_name[32] = "my_config";
				widgets::InputText("Name", config_name, sizeof(config_name), "Config file name");
				static bool autosave = true;
				widgets::Checkbox("Auto save", &autosave, "Save on exit");
				if (widgets::Button("Save config", (col_w - 20.0f * dpi) * 0.5f, 20.0f * dpi))
				{
				}
				ImGui::SameLine();
				if (widgets::Button("Load config", (col_w - 20.0f * dpi) * 0.5f, 20.0f * dpi))
				{
				}
			}
			widgets::EndGroupBox();

			ImGui::SetCursorPos(ImVec2(content_pos.x, content_pos.y + config_h + box_gap));
			if (widgets::BeginGroupBox("Binds", col_w, 4))
			{
				static int menu_key = 0;
				const char* keys[] = { "Insert", "Home", "Delete", "End", "F1", "F2" };
				widgets::Combo("Menu key", &menu_key, keys, 6, "Key to toggle menu");
				static int aim_key = 1;
				const char* aim_keys[] = { "Mouse 1", "Mouse 2", "Mouse 4", "Mouse 5", "Shift" };
				widgets::Combo("Aim key", &aim_key, aim_keys, 5, "Hold to aim");
				static int panic_key = 2;
				widgets::Combo("Panic key", &panic_key, keys, 6, "Disable all features");
				static bool hold_aim = true;
				widgets::Checkbox("Hold to aim", &hold_aim, "Require holding key");
			}
			widgets::EndGroupBox();

			float binds_h = widgets::CalcGroupBoxHeight(4, "Binds");
			ImGui::SetCursorPos(ImVec2(content_pos.x, content_pos.y + config_h + box_gap + binds_h + box_gap));
			if (widgets::BeginGroupBox("Priority", col_w, 5, true))
			{
				static int prio_order[4] = { 0, 1, 2, 3 };
				static const char* prio_items[] = { "Aimbot", "Visuals", "Anti-aim", "Misc" };
				widgets::ReorderList("Feature order", prio_order, prio_items, 4);
			}
			widgets::EndGroupBox();

			ImGui::SetCursorPos(ImVec2(content_pos.x + col_w + col_gap, content_pos.y));
			if (widgets::BeginGroupBox("Theme", col_w, 4))
			{
				static int accent = 2;
				const char* accents[] = { "Blue", "Gray", "Orange", "Green", "Red", "Purple", "Pink", "Teal" };
				if (widgets::Combo("Accent", &accent, accents, 8, "Theme accent"))
					gui::colors::SetTheme((gui::colors::Theme)accent);

				static int density = 75;
				if (widgets::SliderInt("Density", &density, 0, 100, "Panel darkness"))
					gui::colors::SetDensity(density);

				widgets::SliderFloat("UI scale", &dpi_preview_t, 0.8f, 1.5f, "%.1f", "DPI scale multiplier");
				if (!ImGui::IsMouseDown(0))
					widgets::dpi_scale_t = dpi_preview_t;
			}
			widgets::EndGroupBox();

			float theme_h = widgets::CalcGroupBoxHeight(4, "Theme");
			ImGui::SetCursorPos(ImVec2(content_pos.x + col_w + col_gap, content_pos.y + theme_h + box_gap));
			if (widgets::BeginGroupBox("Debug", col_w, 11))
			{
				static bool show_fps = false;
				static bool show_ping = false;
				widgets::Checkbox("Show FPS", &show_fps, "Display frame counter");
				widgets::Checkbox("Show ping", &show_ping, "Display network latency");

				static int log_level = 1;
				const char* log_levels[] = { "Verbose", "Info", "Warning", "Error", "Fatal" };
				widgets::Listbox("Log level", &log_level, log_levels, 5, 4, "Filter log output");

				if (widgets::Button("Reset settings", col_w - 16.0f * dpi, 20.0f * dpi))
				{
					widgets::PushNotification("Settings Reset", "All values restored to defaults.", widgets::ToastType::Success, 3.0f);
				}
				if (widgets::Button("Open folder", col_w - 16.0f * dpi, 20.0f * dpi))
				{
					widgets::PushNotification("Folder", "Config directory opened.", widgets::ToastType::Info, 3.0f);
				}
				if (widgets::Button("Test warning", col_w - 16.0f * dpi, 20.0f * dpi))
				{
					widgets::PushNotification("Warning", "VAC authentication error detected.", widgets::ToastType::Warning, 4.0f);
				}
				if (widgets::Button("Test error", col_w - 16.0f * dpi, 20.0f * dpi))
				{
					widgets::PushNotification("Error", "Failed to inject module into process.", widgets::ToastType::Error, 5.0f);
				}
			}
			widgets::EndGroupBox();
		}

		ImGui::PopStyleVar();

		ImGui::End();
	}






	widgets::UpdateAndDrawParticles(ImGui::GetForegroundDrawList());

	io.FontGlobalScale = 1.0f;
}

void GUI::renderLogs()
{
	ImGui::SetNextWindowSize(ImVec2(680.0f, 420.0f), ImGuiCond_Once);
	ImVec2 display = ImGui::GetIO().DisplaySize;
	if (display.x < 100.0f) display.x = 100.0f;
	if (display.y < 100.0f) display.y = 100.0f;
	ImGui::SetNextWindowPos(ImVec2(display.x * 0.5f - 340.0f, display.y * 0.5f - 210.0f), ImGuiCond_Once);
	ImGui::Begin("Logs", nullptr, ImGuiWindowFlags_NoCollapse);

	static char s_log_search[128] = "";
	static bool s_filter_success = true;
	static bool s_filter_error = true;
	static bool s_filter_debug = true;
	static bool s_filter_prompt = true;
	static bool s_auto_scroll = true;

	widgets::InputText("Search", s_log_search, sizeof(s_log_search));
	widgets::Checkbox("Auto-scroll", &s_auto_scroll);
	ImGui::Separator();
	widgets::Checkbox("Success", &s_filter_success);
	widgets::Checkbox("Error", &s_filter_error);
	widgets::Checkbox("Debug", &s_filter_debug);
	widgets::Checkbox("Prompt", &s_filter_prompt);

	auto entries = Logger::GetEntries();

	float avail_w = ImGui::GetContentRegionAvail().x;
	if (avail_w < 10.0f) avail_w = 10.0f;
	if (widgets::BeginGroupBox("Log Output", avail_w, 12, false)) {
		Logger::Render();	
		widgets::EndGroupBox();
	}

	if (widgets::Button("Clear", 100.0f, 20.0f)) {
		Logger::ClearEntries();
	}
	ImGui::SameLine();
	ImGui::Text("Log backup: mirage_logs.txt");

	ImGui::End();

}

void GUI::shutdown()
{
	gui::font::Shutdown();
	m_inited = false;
}
