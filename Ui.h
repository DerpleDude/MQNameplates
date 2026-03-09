#pragma once

#include "imgui.h"

#include <unordered_map>
#include <string>

struct CursorState;

namespace eqlib
{
	class EQ_Spell;
	class PlayerClient;
}

class Ui
{
public:
	struct TooltipState
	{
		int was_hovered;
		float tooltip_time;
	};

	struct TrendState
	{
		float lastPct;
		int direction;
	};

	struct AnimState
	{
		float lastTarget;
	};

	struct SettingsStruct
	{
		ImVec2 Padding = ImVec2(8, 4);
		int FontSize = 20;
		float IconSize = 20.0f;

		float BarRounding = 6.0f;
		float BarBorderThickness = 2.5f;

		std::unordered_map<std::string, TrendState> ProgBarTrendState;
		std::unordered_map<std::string, AnimState> ProgBarAnimState;

		TooltipState TooltipAnimationState;
	};

	static void RenderNamePlateText(CursorState& cursor, ImU32 color, const char* text);

	static void RenderNamePlateRect(
		CursorState& cursor,
		const ImVec2& size,
		ImU32 color,
		float rounding,
		float thickness,
		bool filled
	);

	static void DrawInspectableSpellIcon(
		CursorState& cursor,
		eqlib::EQ_Spell* pSpell
	);

	static void RenderAnimatedPercentage(
		CursorState& cursor,
		const std::string& id,
		float barPct,
		float height,
		float width,
		const ImVec4& colLow,
		const ImVec4& colMid,
		const ImVec4& colHigh,
		ImU32 colHighlight,
		const std::string& label = ""
	);

	static void RenderFancyHPBar(
		CursorState& cursor,
		const std::string& id,
		float hpPct,
		float height,
		float width,
		ImU32 hpHighlight,
		const std::string& label = ""
	);

	static SettingsStruct Settings;
};

struct CursorState
{
	ImVec2 CursorPos = ImVec2(0, 0);
	ImVec2 LastCursorLinePos = ImVec2(0, 0);
	float LineStartXPos = 0.0f;

	explicit CursorState(const ImVec2& startingPos)
	{
		SetPos(startingPos + Ui::Settings.Padding);
	}

	void SetPos(const ImVec2& pos)
	{
		CursorPos = pos;
		LastCursorLinePos = pos;
		LineStartXPos = pos.x;
	}

	const ImVec2& GetPos() const
	{
		return CursorPos;
	}

	void Move(const ImVec2& pos)
	{
		ImVec2 p = Ui::Settings.Padding + pos;

		LastCursorLinePos.x = CursorPos.x + p.x;
		LastCursorLinePos.y = CursorPos.y;

		CursorPos.y += p.y;
		CursorPos.x = LineStartXPos;
	}

	void SameLine()
	{
		CursorPos = LastCursorLinePos;
	}
};