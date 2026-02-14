#include "widgets.hh"
#include "../../interface/sets/Colors.hh"
#include <cmath>

namespace widgets
{
	int group_depth_t = 0;

	std::unordered_map<ImGuiID, TabAnim> tab_anims_t;
	TabTransition tab_transition_t;

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

	static float EaseOutQuad(float t)
	{
		return 1.0f - (1.0f - t) * (1.0f - t);
	}

	std::unordered_map<ImGuiID, GroupBoxAnim> groupbox_anims_t;
	static bool groupbox_content_visible_t[16] = {};
	static int  groupbox_stack_depth_t = 0;

	float CalcGroupBoxHeight(int widget_count, const char* label)
	{
		float full_height = S(28.0f) + (widget_count * S(20.0f));

		if (label)
		{
			ImGuiID id = ImGui::GetID(label);
			auto it = groupbox_anims_t.find(id);
			if (it != groupbox_anims_t.end())
			{
				float title_h = S(20.0f);
				float t = it->second.collapse;
				return title_h + (full_height - title_h) * (1.0f - t);
			}
		}

		return full_height;
	}

	bool BeginGroupBox(const char* label, float width, int widget_count, bool collapsible)
	{
		ImGuiID id = ImGui::GetID(label);
		GroupBoxAnim& anim = groupbox_anims_t[id];

		float dt = ImGui::GetIO().DeltaTime;
		float speed = 8.0f;

		float collapse_target = anim.collapsed ? 1.0f : 0.0f;
		anim.collapse = anim.collapse + (collapse_target - anim.collapse) * Clamp(dt * speed, 0.0f, 1.0f);
		if (fabsf(anim.collapse - collapse_target) < 0.001f)
			anim.collapse = collapse_target;

		anim.arrow_rot = anim.arrow_rot + (collapse_target - anim.arrow_rot) * Clamp(dt * speed, 0.0f, 1.0f);
		if (fabsf(anim.arrow_rot - collapse_target) < 0.001f)
			anim.arrow_rot = collapse_target;

		float full_height = S(28.0f) + (widget_count * S(20.0f));
		float title_h = S(20.0f);
		float height = title_h + (full_height - title_h) * (1.0f - anim.collapse);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, S(2)));
		ImGui::BeginChild(label, ImVec2(width, height), false,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);

		ImVec2 pos = ImGui::GetWindowPos();
		ImDrawList* draw = ImGui::GetWindowDrawList();

		draw->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(15, 15, 15, 255));
		draw->AddRect(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(40, 40, 40, 255));
		draw->AddRect(ImVec2(pos.x + 1, pos.y + 1), ImVec2(pos.x + width - 1, pos.y + height - 1), IM_COL32(25, 25, 25, 255));

		ImVec2 mouse = ImGui::GetIO().MousePos;
		bool title_hovered = (mouse.x >= pos.x && mouse.x <= pos.x + width &&
		                      mouse.y >= pos.y && mouse.y <= pos.y + title_h);

		float hover_target = title_hovered ? 1.0f : 0.0f;
		anim.hover = anim.hover + (hover_target - anim.hover) * Clamp(dt * 10.0f, 0.0f, 1.0f);

		ImU32 title_col = LerpColor(IM_COL32(20, 20, 20, 255), IM_COL32(28, 28, 28, 255), anim.hover);

		if (collapsible && title_hovered && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemActive())
			anim.collapsed = !anim.collapsed;

		draw->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + title_h), title_col);
		draw->AddLine(ImVec2(pos.x, pos.y + title_h), ImVec2(pos.x + width, pos.y + title_h), IM_COL32(40, 40, 40, 255));

		if (collapsible)
		{
			float arrow_sz = S(4.0f);
			float ax = pos.x + S(8.0f);
			float ay = pos.y + title_h * 0.5f;
			float rot = anim.arrow_rot;

			ImVec2 p1, p2, p3;

			float dx1 = -arrow_sz, dy1 = -arrow_sz * 0.5f;
			float dx2 =  arrow_sz, dy2 = -arrow_sz * 0.5f;
			float dx3 =  0.0f,     dy3 =  arrow_sz * 0.5f;

			float rx1 = -arrow_sz * 0.5f, ry1 = -arrow_sz;
			float rx2 = -arrow_sz * 0.5f, ry2 =  arrow_sz;
			float rx3 =  arrow_sz * 0.5f, ry3 =  0.0f;

			p1 = ImVec2(ax + dx1 + (rx1 - dx1) * rot, ay + dy1 + (ry1 - dy1) * rot);
			p2 = ImVec2(ax + dx2 + (rx2 - dx2) * rot, ay + dy2 + (ry2 - dy2) * rot);
			p3 = ImVec2(ax + dx3 + (rx3 - dx3) * rot, ay + dy3 + (ry3 - dy3) * rot);

			ImU32 arrow_col = LerpColor(IM_COL32(120, 120, 120, 255), IM_COL32(200, 200, 200, 255), anim.hover);
			draw->AddTriangleFilled(p1, p2, p3, arrow_col);
		}

		float label_x = collapsible ? S(18.0f) : S(8.0f);
		ImVec2 text_pos(pos.x + label_x, pos.y + S(3.0f));
		draw->AddText(ImVec2(text_pos.x + 1, text_pos.y + 1), IM_COL32(0, 0, 0, 180), label);
		draw->AddText(text_pos, IM_COL32(200, 200, 200, 255), label);

		bool content_visible = anim.collapse < 0.99f;

		if (content_visible)
		{
			draw->PushClipRect(ImVec2(pos.x, pos.y + title_h), ImVec2(pos.x + width, pos.y + height), true);
			ImGui::SetCursorPosY(S(24.0f));
			ImGui::Indent(S(8.0f));
		}

		group_depth_t++;

		if (groupbox_stack_depth_t < 16)
			groupbox_content_visible_t[groupbox_stack_depth_t++] = content_visible;

		return content_visible;
	}

	void EndGroupBox()
	{
		if (group_depth_t > 0)
		{
			group_depth_t--;

			bool was_visible = false;
			if (groupbox_stack_depth_t > 0)
				was_visible = groupbox_content_visible_t[--groupbox_stack_depth_t];

			if (was_visible)
			{
				ImGui::Unindent(S(8.0f));
				ImGui::GetWindowDrawList()->PopClipRect();
			}

			ImGui::EndChild();
			ImGui::PopStyleVar();
		}
	}

	bool TabButton(const char* label, bool active, float width, float height)
	{
		ImGui::PushID(label);
		ImGuiID id = ImGui::GetID("##tab");

		TabAnim& anim = tab_anims_t[id];

		ImVec2 pos = ImGui::GetCursorScreenPos();
		const float rounding = 2.0f;

		ImGui::InvisibleButton("##tab", ImVec2(width, height));
		bool pressed = ImGui::IsItemClicked();
		bool hovered = ImGui::IsItemHovered();

		if (pressed)
		{
			anim.click = 1.0f;
		}

		float dt = ImGui::GetIO().DeltaTime;
		float hover_target = hovered ? 1.0f : 0.0f;
		float active_target = active ? 1.0f : 0.0f;
		float accent_target = active ? height : 0.0f;

		anim.hover = Lerp(anim.hover, hover_target, dt * 12.0f);
		anim.active = Lerp(anim.active, active_target, dt * 10.0f);
		anim.accent_width = Lerp(anim.accent_width, accent_target, dt * 14.0f);

		if (anim.click > 0.0f)
		{
			anim.click -= dt * 3.5f;
			if (anim.click < 0.0f) anim.click = 0.0f;
		}

		ImDrawList* draw = ImGui::GetWindowDrawList();

		ImU32 col_bg_idle = IM_COL32(12, 12, 12, 255);
		ImU32 col_bg_hover = IM_COL32(18, 18, 18, 255);
		ImU32 col_bg_active = IM_COL32(22, 22, 22, 255);
		ImU32 col_border = IM_COL32(35, 35, 35, 255);
		ImU32 col_text_idle = IM_COL32(100, 100, 100, 255);
		ImU32 col_text_hover = IM_COL32(160, 160, 160, 255);
		ImU32 col_text_active = IM_COL32(255, 255, 255, 255);
		const auto& pal = gui::colors::Get();
		ImU32 col_accent = pal.accent;

		ImU32 bg_col = LerpColor(col_bg_idle, col_bg_hover, anim.hover);
		bg_col = LerpColor(bg_col, col_bg_active, anim.active);

		ImU32 text_col = LerpColor(col_text_idle, col_text_hover, anim.hover);
		text_col = LerpColor(text_col, col_text_active, anim.active);

		draw->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), bg_col, rounding);

		if (anim.click > 0.01f)
		{
			float ripple_progress = 1.0f - anim.click;
			float ripple_radius = width * 0.7f * EaseOutQuad(ripple_progress);
			int ripple_alpha = (int)(60 * anim.click);
			ImU32 ripple_col = gui::colors::AccentA(ripple_alpha);

			float cx = pos.x + width * 0.5f;
			float cy = pos.y + height * 0.5f;

			draw->PushClipRect(pos, ImVec2(pos.x + width, pos.y + height), true);
			draw->AddCircleFilled(ImVec2(cx, cy), ripple_radius, ripple_col, 24);
			draw->PopClipRect();
		}

		draw->AddRect(pos, ImVec2(pos.x + width, pos.y + height), col_border, rounding);

		if (anim.accent_width > 0.5f)
		{
			float accent_y_center = pos.y + height * 0.5f;
			float accent_half = anim.accent_width * 0.5f;

			ImU32 glow_col = gui::colors::AccentA((int)(40 * anim.active));
			draw->AddRectFilled(
				ImVec2(pos.x - 1, accent_y_center - accent_half - S(2)),
				ImVec2(pos.x + S(4), accent_y_center + accent_half + S(2)),
				glow_col, 2.0f);

			draw->AddRectFilled(
				ImVec2(pos.x, accent_y_center - accent_half),
				ImVec2(pos.x + S(2), accent_y_center + accent_half),
				col_accent, 1.0f);
		}

		if (anim.hover > 0.01f && anim.active < 0.5f)
		{
			ImU32 top_highlight = gui::colors::AccentA((int)(30 * anim.hover));
			draw->AddRectFilled(
				ImVec2(pos.x + 1, pos.y + 1),
				ImVec2(pos.x + width - 1, pos.y + S(3)),
				top_highlight, rounding - 1);
		}

		ImVec2 text_size = ImGui::CalcTextSize(label);
		float text_x = pos.x + S(10.0f);
		float text_y = pos.y + (height - text_size.y) * 0.5f;

		text_x += anim.active * S(2.0f);

		draw->AddText(ImVec2(text_x + 1, text_y + 1), IM_COL32(0, 0, 0, 150), label);
		draw->AddText(ImVec2(text_x, text_y), text_col, label);

		ImGui::PopID();
		return pressed;
	}
}
