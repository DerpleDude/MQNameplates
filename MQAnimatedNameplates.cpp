// MQAnimatedNameplates.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup.

// By: Derple derple@ntsj.com

#include "Ui.h"

#include "eqlib/graphics/CameraInterface.h"
#include "imgui/imgui.h"
#include "mq/imgui/ImGuiUtils.h"
#include "mq/Plugin.h"

#include "sol/sol.hpp"

PreSetup("MQAnimatedNameplates");
PLUGIN_VERSION(0.1);

void DrawNameplates()
{
	const CVector3 targetPos(pTarget->Y, pTarget->X, pTarget->Z + pTarget->Height);
	float targetNameplatePosX, targetNameplatePosY;

	pDisplay->pCamera->ProjectWorldCoordinatesToScreen(targetPos, targetNameplatePosX, targetNameplatePosY);
	bool open = true;

	ImGui::PushFont(nullptr, Ui::Settings.FontSize);

	const char* targetName = pTarget->DisplayedName;
	int pctHP = pTarget->HPMax == 0 ? 0 : pTarget->HPCurrent * 100 / pTarget->HPMax;
	char targetPctHPs[16];
	sprintf_s(targetPctHPs, "%d%%", pctHP);

	char classInfo[64];
	sprintf_s(classInfo, "%d %s", pTarget->GetLevel(), GetClassDesc(pTarget->GetClass()));

	ImVec2 canvasSize(500, 50);

	int buffCount = GetCachedBuffCount(pTarget);

	ImVec2 assumedHeadOffset(
		0,
		35 + (ceil(buffCount / 10.0f) * (Ui::Settings.IconSize + Ui::Settings.Padding.y))
	);

	ImVec2 curPos(
		targetNameplatePosX - canvasSize.x * 0.5f - assumedHeadOffset.x,
		targetNameplatePosY - canvasSize.y * 0.5f - assumedHeadOffset.y
	);

	CursorState cursor{ curPos };

	int buffsPerRow = std::floor(canvasSize.x / (Ui::Settings.IconSize + Ui::Settings.Padding.x));

	for (int i = 0; i < buffCount; i++)
	{
		auto buff = GetCachedBuffAtSlot(pTarget, i);

		if (buff.has_value())
		{
			if (EQ_Spell* spell = GetSpellByID(buff->spellId))
			{
				Ui::DrawInspectableSpellIcon(cursor, spell);

				if (i == 0 || (i < (buffCount - 1) && i % buffsPerRow != 0))
					cursor.SameLine();
			}
		}
	}

	ImVec2 panelPos = cursor.GetPos();

	Ui::RenderNamePlateRect(cursor, canvasSize, IM_COL32(153, 153, 153, 204), 2, 0, true);

	panelPos.x += Ui::Settings.Padding.x;

	cursor.SetPos(panelPos);

	ImU32 textColor = IM_COL32(255, 255, 255, 255);

	ImU32 conColor = ConColorToARGB(ConColor(pTarget));

	curPos = cursor.GetPos();
	float startXPos = curPos.x;

	Ui::RenderNamePlateText(cursor, textColor, targetName);

	// center this text
	float classWidth = ImGui::CalcTextSize(classInfo).x;
	curPos.x = (curPos.x + canvasSize.x / 2) -
		(classWidth / 2 + Ui::Settings.Padding.x * 2);

	cursor.SetPos(curPos);
	Ui::RenderNamePlateText(cursor, textColor, classInfo);

	// right justify this text
	float hpWidth = ImGui::CalcTextSize(targetPctHPs).x;
	curPos.x = (startXPos + canvasSize.x) -
		(hpWidth + Ui::Settings.Padding.x * 2);

	cursor.SetPos(curPos);
	Ui::RenderNamePlateText(cursor, textColor, targetPctHPs);

	cursor.SetPos(ImVec2(startXPos, cursor.GetPos().y));

	std::string hpBarID =
		std::string("TargetHPBar_") +
		std::to_string(pTarget->GetId());

	Ui::RenderFancyHPBar(
		cursor,
		hpBarID,
		pctHP,
		ImGui::GetTextLineHeight() * 0.75f,
		canvasSize.x - Ui::Settings.Padding.x * 2,
		conColor,
		""
	);
	ImGui::PopFont();
}

PLUGIN_API void InitializePlugin()
{
}

PLUGIN_API void ShutdownPlugin()
{
}

PLUGIN_API void OnUpdateImGui()
{
	if (GetGameState() == GAMESTATE_INGAME)
	{
		if (!pTarget || !pDisplay)
			return;

		DrawNameplates();
	}
}

sol::object DoCreateModule(sol::this_state s)
{
	sol::state_view L(s);

	sol::table module = L.create_table();

	module["ProjectWorldCoordinatesToScreen"] = [](sol::this_state L, const float x, const float y, const float z) -> ImVec2
		{

			if (!pDisplay || !pDisplay->pCamera)
				return ImVec2(0, 0);

			float outX, outY;

			pDisplay->pCamera->ProjectWorldCoordinatesToScreen(CVector3(y, x, z), outX, outY);

			return ImVec2(outX, outY);
		};

	return sol::make_object(L, module);
}

PLUGIN_API bool CreateLuaModule(sol::this_state s, sol::object& object)
{
	object = DoCreateModule(s);
	return true;
}