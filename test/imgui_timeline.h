#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"


static const float NODE_SLOT_RADIUS = 4.0f;


namespace ImGui
{

static float s_max_timeline_value;


bool BeginTimeline(const char* str_id, float max_value)
{
	s_max_timeline_value = max_value;

	return BeginChild(str_id);
}


static const float TIMELINE_RADIUS = 6;


bool TimelineEvent(const char* str_id, float* values)
{
	ImGuiWindow* win = GetCurrentWindow();
	const ImU32 inactive_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Button]);
	const ImU32 active_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_ButtonHovered]);
        const ImU32 line_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_ButtonActive]);
	bool changed = false;
	ImVec2 cursor_pos = win->DC.CursorPos;
	
	for (int i = 0; i < 2; ++i)
	{
		ImVec2 pos = cursor_pos;
		pos.x += win->Size.x * values[i] / s_max_timeline_value + TIMELINE_RADIUS;
		pos.y += TIMELINE_RADIUS;

		SetCursorScreenPos(pos - ImVec2(TIMELINE_RADIUS, TIMELINE_RADIUS));
		PushID(i);
		InvisibleButton(str_id, ImVec2(2 * TIMELINE_RADIUS, 2 * TIMELINE_RADIUS));
		if (IsItemActive() || IsItemHovered())
		{
			ImGui::SetTooltip("%f", values[i]);
			ImVec2 a(pos.x, GetWindowContentRegionMin().y + win->Pos.y);
			ImVec2 b(pos.x, GetWindowContentRegionMax().y + win->Pos.y);
			win->DrawList->AddLine(a, b, line_color);
		}
		if (IsItemActive() && IsMouseDragging(0))
		{
			values[i] += GetIO().MouseDelta.x / win->Size.x * s_max_timeline_value;
			changed = true;
		}
		PopID();
		win->DrawList->AddCircleFilled(
			pos, TIMELINE_RADIUS, IsItemActive() || IsItemHovered() ? active_color : inactive_color);
	}
	
	ImVec2 start = cursor_pos;
	start.x += win->Size.x * values[0] / s_max_timeline_value + 2 * TIMELINE_RADIUS;
	start.y += TIMELINE_RADIUS * 0.5f;
	ImVec2 end = start + ImVec2(win->Size.x * (values[1] - values[0]) / s_max_timeline_value - 2 * TIMELINE_RADIUS,
							 TIMELINE_RADIUS);

	PushID(-1);
	SetCursorScreenPos(start);
	InvisibleButton(str_id, end - start);
	if (IsItemActive() && IsMouseDragging(0))
	{
		values[0] += GetIO().MouseDelta.x / win->Size.x * s_max_timeline_value;
		values[1] += GetIO().MouseDelta.x / win->Size.x * s_max_timeline_value;
		changed = true;
	}
	PopID();

	SetCursorScreenPos(cursor_pos + ImVec2(0, GetTextLineHeightWithSpacing()));

	win->DrawList->AddRectFilled(start, end, IsItemActive() || IsItemHovered() ? active_color : inactive_color);
	
	if (values[0] > values[1])
	{
		float tmp = values[0];
		values[0] = values[1];
		values[1] = tmp;
	}
	if (values[1] > s_max_timeline_value) values[1] = s_max_timeline_value;
	if (values[0] < 0) values[0] = 0;
	return changed;
}


void EndTimeline()
{
	ImGuiWindow* win = GetCurrentWindow();
	
	ImU32 color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Button]);
	ImU32 line_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Border]);
	ImU32 text_color = ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Text]);
	float rounding = GImGui->Style.ScrollbarRounding;
        ImVec2 start(GetWindowContentRegionMin().x + win->Pos.x,
                GetWindowContentRegionMax().y - GetTextLineHeightWithSpacing() + win->Pos.y);
	ImVec2 end = GetWindowContentRegionMax() + win->Pos;

        win->DrawList->AddRectFilled(start, end, color, rounding);

	const int LINE_COUNT = 5;
	const ImVec2 text_offset(0, GetTextLineHeightWithSpacing());
	for (int i = 0; i < LINE_COUNT; ++i)
	{
		ImVec2 a = GetWindowContentRegionMin() + win->Pos + ImVec2(TIMELINE_RADIUS, 0);
		a.x += i * GetWindowContentRegionWidth() / LINE_COUNT;
		ImVec2 b = a;
		b.y = start.y;
		win->DrawList->AddLine(a, b, line_color);
		char tmp[256];
		ImFormatString(tmp, sizeof(tmp), "%.2f", i * s_max_timeline_value / LINE_COUNT);
		win->DrawList->AddText(b, text_color, tmp);
	}

        EndChild();
}


} // namespace ImGui

