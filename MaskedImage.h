#pragma once

#include "imgui/imgui.h"
#include "mq/api/Textures.h"

#include <string>

namespace Ui {

class MaskedImage
{
public:
    MaskedImage() = default;
    MaskedImage(const std::string& sourcePath, const std::string& maskPath);

    bool IsValid() const;

    void Render(ImDrawList* dl, const ImVec2& min, const ImVec2& max,
        ImU32 tint = IM_COL32_WHITE) const;

    void RenderNineSlice(ImDrawList* dl, const ImVec2& min, const ImVec2& max, const ImVec2& maskSize,
        const ImVec4& margins, ImU32 tint = IM_COL32_WHITE) const;

    void RenderMask(ImDrawList* dl, const ImVec2& min, const ImVec2& max,
        ImU32 tint = IM_COL32_WHITE) const;

    void RenderMaskNineSlice(ImDrawList* dl, const ImVec2& min, const ImVec2& max, const ImVec2& maskSize,
        const ImVec4& margins, ImU32 tint = IM_COL32_WHITE) const;

    static void ReleaseShader();

private:
    mq::MQTexturePtr m_pSource;
    mq::MQTexturePtr m_pMask;
};

} // namespace Ui
