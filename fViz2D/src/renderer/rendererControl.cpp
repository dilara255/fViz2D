// OpenGL3 renderer using glfw and imGui, based on imGui's example
// NOTE: loads a image located on the solutions "res" folder:
// =======> May fail depending on working directory
// TODO: make it checked and etc;

#include "layers/glfwOglLayer.hpp"
#include "layers/imGuiLayer.hpp"

#include "renderer/rendererControl.hpp"
#include "resourcePaths.hpp"

#include "utils/imageUtils.hpp"
#include "utils/guiUtils.hpp"

#include <stdio.h>

static void tmpGlfwErrorCallback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void render(GLFWwindow* window, ImGuiIO& io, COLOR::rgbaF_t* clearColor_ptr, 
            IMG::rgbaTextureID_t* bannerTexture_ptr, IMG::rgbaTextureID_t* dynamicTexture_ptr, 
            COLOR::rgbaF_t* noiseTint_ptr, bool* keepRendering_ptr, bool* testBool_ptr) {

    imGuiNewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    imGuiTestMenu(io, &(clearColor_ptr->r), &(noiseTint_ptr->r), keepRendering_ptr, testBool_ptr);
    imGuiDrawTexture(bannerTexture_ptr);
    imGuiDrawTexture(dynamicTexture_ptr, "Dynamic Data");
    ImGui::Render();

    clearFrameBuffer(window, *clearColor_ptr);

    imGuiDrawGUIandUpdateOsWindows(ImGui::GetDrawData(), io);

    glfwSwapBuffers(window);
}

int F_V2::rendererMain(bool* externalBool_ptr, IMG::rgbaImage_t* dynamicData_ptr, 
                       COLOR::rgbaF_t* clearColor_ptr, COLOR::rgbaF_t* noiseTint_ptr) {

    //INIT:
    GLFWwindow* window = initGlfwAndCreateWindow(tmpGlfwErrorCallback, 800, 600, 
                                              "Ogl3 Render Test - imGui + Glfw");
    if(window == nullptr) { return renrederRetCodes::CONTEXT_ACQ_FAILED; }

    ImGuiIO& io = initImgui(window, "#version 330");
    //TODO: check return?

    //Load test bannerTexture:

    IMG::rgbaTextureID_t bannerTexture = IMG::load4channelTextureFromFile(F_V2::bannerPathFromBinary);
    if (!bannerTexture.initialized) { return renrederRetCodes::IMAGE_LOAD_FAILED; }

    //Create texture resorce for dynamic data:

    IMG::rgbaTextureID_t dynamicTexture;
    IMG::load4channelTextureFromRgbaImage(dynamicData_ptr, &dynamicTexture);
    if(!dynamicTexture.initialized) { return renrederRetCodes::DYNAMIC_IMAGE_INITIAL_LOAD_FAILED; }

    //RUN:
    bool keepRunning = true;
    while (!glfwWindowShouldClose(window) && keepRunning) {
        glfwPollEvents(); //for app: check io.WantCaptureMouse and io.WantCaptureKeyboard
        render(window, io, clearColor_ptr, &bannerTexture, &dynamicTexture, noiseTint_ptr, 
                                                           &keepRunning, externalBool_ptr);
        IMG::load4channelTextureFromRgbaImage(dynamicData_ptr, &dynamicTexture);
    }

    //END:
    shutDownImGui();
    shutDownGLFW(window);

    return renrederRetCodes::OK;
}

void F_V2::rendererMainForSeparateThread(bool* externalBool_ptr, IMG::rgbaImage_t* dynamicData_ptr, 
                                         COLOR::rgbaF_t* clearColor_ptr, COLOR::rgbaF_t* noiseTint_ptr, 
                                                                                   int* returnCode_ptr) {

    *returnCode_ptr = F_V2::rendererMain(externalBool_ptr, dynamicData_ptr, clearColor_ptr, noiseTint_ptr);
    return;
}

