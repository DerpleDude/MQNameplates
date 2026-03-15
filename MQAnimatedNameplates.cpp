// MQAnimatedNameplates.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup.

// By: Derple derple@ntsj.com

#include "Config.h"
#include "NamePlate.h"
#include "SettingsPanel.h"

#include "eqlib/graphics/CameraInterface.h"
#include "mq/Plugin.h"
#include "mq/imgui/ImGuiUtils.h"

#include "imgui/imgui.h"
#include "imgui/imanim/im_anim.h"
#include "imgui_internal.h"
#include "sol/sol.hpp"

#include <map>

PreSetup("MQAnimatedNameplates");
PLUGIN_VERSION(0.1);

iam_context* context = nullptr;

std::map<unsigned int, Ui::Nameplate> nameplatesBySpawnId;

void DrawNameplates(PlayerClient* pSpawn, Ui::HPBarStyle style)
{
    if (!pSpawn)
        return;

    std::string hpBarID  = fmt::format("TargetHPBar_{}", pSpawn->SpawnID);
    ImU32       conColor = GetColorForChatColor(ConColor(pSpawn)).ToImU32();

    auto [it, inserted] =
        nameplatesBySpawnId.try_emplace(pSpawn->SpawnID, Ui::Nameplate{hpBarID, "", "", pSpawn, conColor});

    Ui::Nameplate& nameplate = it->second;

    const CVector3 targetPos(pSpawn->Y, pSpawn->X, pSpawn->Z + pSpawn->Height);
    float          targetNameplatePosX, targetNameplatePosY;

    pDisplay->pCamera->ProjectWorldCoordinatesToScreen(targetPos, targetNameplatePosX, targetNameplatePosY);
    targetNameplatePosY = std::max(targetNameplatePosY, 75.0f);
    
    const ImVec2 padding = ImGui::GetStyle().FramePadding;
    const ImVec2 targetNameplatePos{targetNameplatePosX, targetNameplatePosY};
    const ImVec2 canvasSize(Ui::Config::Get().NameplateWidth, 50);

    float pctHP = pSpawn->HPMax == 0 ? 0 : pSpawn->HPCurrent * 100.0f / pSpawn->HPMax;

    ImVec2 baseHeadOffset{ 0, Ui::Config::Get().NameplateHeightOffset };

    nameplate.Render(targetNameplatePos - baseHeadOffset, canvasSize, pctHP, style, pTarget == pSpawn);
}

PLUGIN_API void InitializePlugin()
{
    context = iam_context_create();
    AddSettingsPanel("plugins/Nameplates", Ui::RenderSettingsPanel);
}

PLUGIN_API void ShutdownPlugin()
{
    iam_context_destroy(context);
    RemoveSettingsPanel("plugins/Nameplates");
}

PLUGIN_API void OnUpdateImGui()
{
    iam_context_set_current(context);

    // cleanup stale nameplates
    for (auto it = nameplatesBySpawnId.begin(); it != nameplatesBySpawnId.end();)
    {
        if (time(nullptr) - it->second.GetLastRenderTime() > 30)
            it = nameplatesBySpawnId.erase(it);
        else
            ++it;
    }

    if (GetGameState() == GAMESTATE_INGAME)
    {
        if (!pDisplay)
            return;

        if (Ui::Config::Get().RenderForTarget)
            DrawNameplates(pTarget, static_cast<Ui::HPBarStyle>(Ui::Config::Get().HPBarStyleTarget.get()));
        if (Ui::Config::Get().RenderForSelf)
            DrawNameplates(pLocalPlayer, static_cast<Ui::HPBarStyle>(Ui::Config::Get().HPBarStyleSelf.get()));
        if (Ui::Config::Get().RenderForGroup && pLocalPC->pGroupInfo)
        {
            for (int i = 0; i < MAX_GROUP_SIZE; i++)
            {
                CGroupMember* pGroupMember = pLocalPC->pGroupInfo->GetGroupMember(i);
                if (pGroupMember && pGroupMember->GetPlayer() &&
                    pGroupMember->GetPlayer()->SpawnID != pLocalPlayer->SpawnID)
                    DrawNameplates(pGroupMember->GetPlayer(),
                                   static_cast<Ui::HPBarStyle>(Ui::Config::Get().HPBarStyleGroup.get()));
            }
        }
        if (Ui::Config::Get().RenderForAllHaters)
        {
            if (pLocalPC)
            {
                ExtendedTargetList* xtm = pLocalPC->pExtendedTargetList;

                for (int i = 0; i < xtm->GetNumSlots(); i++)
                {
                    ExtendedTargetSlot* xts = xtm->GetSlot(i);

                    if (xts->SpawnID && xts->xTargetType == XTARGET_AUTO_HATER)
                    {
                        if (!Ui::Config::Get().RenderForTarget || !pTarget || xts->SpawnID != pTarget->SpawnID)
                        {
                            if (PlayerClient* pSpawn = GetSpawnByID(xts->SpawnID))
                            {
                                DrawNameplates(pSpawn,
                                               static_cast<Ui::HPBarStyle>(Ui::Config::Get().HPBarStyleHaters.get()));
                            }
                        }
                    }
                }
            }
        }
    }
}

PLUGIN_API void OnPulse()
{
    Ui::Config::Get().SaveSettings();
}

sol::object DoCreateModule(sol::this_state s)
{
    sol::state_view L(s);

    sol::table module = L.create_table();

    module["ProjectWorldCoordinatesToScreen"] = [](sol::this_state L, const float x, const float y,
                                                   const float z) -> ImVec2
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