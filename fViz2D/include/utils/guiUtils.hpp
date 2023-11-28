#pragma once

#include "GUI_API.hpp"
#include "layers/imGuiLayer.hpp"

#include "renderer/rendererData.hpp"
#include "utils/imageUtils.hpp"

namespace GUI {

    void imGuiDrawTexture(TEX::textureID_t* texID_ptr, const char* windowName = "Texture Drawing");

    void imGuiCreateMenu(menuDefinition_t menuDefinition);

    menuDefinition_t getTestMenuDefinition(bool* testBool_ptr,  float* clearColorFirstElement_ptr, 
                                           float* noiseTintColorFirstElement_ptr);
}

