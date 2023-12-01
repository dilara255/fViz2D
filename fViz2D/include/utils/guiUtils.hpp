#pragma once

#include "FV2_API.hpp"
#include "layers/imGuiLayer.hpp"

#include "renderer/rendererData.hpp"
#include "utils/imageUtils.hpp"

namespace GUI {

    void imGuiDrawTexture(TEX::textureID_t* texID_ptr, const char* windowName = "Texture Drawing");

    void imGuiCreateMenu(menuDefinition_t menuDefinition, F_V2::rendererControlPtrs_t* rendererControl = {} );
}

