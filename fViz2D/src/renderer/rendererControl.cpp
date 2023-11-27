// OpenGL3 renderer using glfw and imGui, based on imGui's example
// NOTE: loads a image located on the solutions "res" folder:
// =======> May fail depending on working directory
// TODO: make it checked and etc;

#include "layers/glfwOglLayer.hpp"
#include "layers/imGuiLayer.hpp"

#include "FV2_API.hpp"

#include "renderer/rendererControl.hpp"
#include "renderer/rendererData.hpp"
#include "resourcePaths.hpp"

#include "utils/imageUtils.hpp"
#include "utils/guiUtils.hpp"

#include <stdio.h>

static void tmpGlfwErrorCallback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void render(GLFWwindow* window, ImGuiIO& io, COLOR::rgbaF_t* clearColor_ptr, 
            TEX::textureID_t* bannerTexture_ptr, TEX::textureID_t* dynamicTexture_ptr, 
            COLOR::rgbaF_t* noiseTint_ptr, bool* keepRendering_ptr, bool* testBool_ptr) {

    GUI::imGuiNewFrame();
    GUI::createTransparentDockNodeOverMainViewport();      
    GUI::imGuiTestMenu(io, &(clearColor_ptr->r), &(noiseTint_ptr->r), keepRendering_ptr, testBool_ptr);
    GUI::imGuiDrawTexture(bannerTexture_ptr);
    GUI::imGuiDrawTexture(dynamicTexture_ptr, "Dynamic Data");
    GUI::render();

    clearFrameBuffer(window, *clearColor_ptr);

    GUI::imGuiDrawGUIandUpdateOsWindows(ImGui::GetDrawData(), io);

    glfwSwapBuffers(window);
}

F_V2::rendererRetCode_st F_V2::rendererMain(bool* externalBool_ptr, IMG::generic2DfieldPtr_t* dynamicData_ptr,
                                            COLOR::rgbaF_t* clearColor_ptr, COLOR::rgbaF_t* noiseTint_ptr,
                                            COLOR::colorInterpolation_t* scheme_ptr = nullptr,
                                            std::string windowName = "Ogl3 Render Test - imGui + Glfw", 
                                            int width = 800, int height = 600,
                                            const char* bannerPathFromBinary = F_V2::testBannerPathFromBinary) {

    //INIT:
    GLFWwindow* window = initGlfwAndCreateWindow(tmpGlfwErrorCallback, width, height, windowName.c_str());
    if(window == nullptr) { return rendererRetCode_st::CONTEXT_ACQ_FAILED; }

    ImGuiIO& io = GUI::initImgui(window, "#version 330");
    //TODO: check return?

    //Load test bannerTexture:

    TEX::textureID_t bannerTexture = TEX::load4channelTextureFromFile(bannerPathFromBinary);
    if (!bannerTexture.initialized) { return rendererRetCode_st::IMAGE_LOAD_FAILED; }

    //Create texture resorce for dynamic data:
    TEX::textureID_t dynamicTexture;
    F_V2::rendererRetCode_st retCode = TEX::loadTextureFromGeneric2DfieldPtr(dynamicData_ptr, &dynamicTexture);
    if(retCode != F_V2::rendererRetCode_st::OK) { return retCode; }
    if(!dynamicTexture.initialized) { return rendererRetCode_st::DYNAMIC_IMAGE_INITIAL_LOAD_FAILED; }

    //Prepare rgba buffer in case the scheme and data type will need it:
    bool keepRunning = true;
    IMG::kinds2Ddata kind = dynamicData_ptr->getKindOfField();
    IMG::generic2DfieldPtr_t colorInterpolationField;
    IMG::generic2DfieldPtr_t* fieldToPassToTexture_ptr = dynamicData_ptr; //may switch later
    bool shouldInterpolateColors = (scheme_ptr != nullptr) && (scheme_ptr->correspondences_ptr->size() >= 2) && 
                             (kind == IMG::kinds2Ddata::FLOATS_FIELD || kind == IMG::kinds2Ddata::DOUBLES_FIELD);
    
    auto size_ptr = dynamicData_ptr->getSizeInfo_ptr();
    size_t imageWidth = size_ptr->width;
    size_t imageHeight = size_ptr->height;    

    IMG::rgbaImage_t rgbaImage;
    if(shouldInterpolateColors) {
        rgbaImage = IMG::createEmpty4channel8bpcImage(imageWidth, imageHeight);
        colorInterpolationField.storeRGBAfield(&rgbaImage);
        fieldToPassToTexture_ptr = &colorInterpolationField;
    }
    
    F_V2::texRetCode_st colorInterpReturn = F_V2::texRetCode_st::OK;

    //RUN:
    while (!glfwWindowShouldClose(window) && keepRunning) {
        glfwPollEvents(); //for app: check io.WantCaptureMouse and io.WantCaptureKeyboard
        render(window, io, clearColor_ptr, &bannerTexture, &dynamicTexture, noiseTint_ptr, 
                                                           &keepRunning, externalBool_ptr);
        
        //TODO: not really checking return of these
        if(shouldInterpolateColors) {
            colorInterpReturn = IMG::translateValuesToInterpolatedColors(dynamicData_ptr, &rgbaImage, scheme_ptr);
            assert(colorInterpReturn == F_V2::texRetCode_st::OK);
        }

        TEX::loadTextureFromGeneric2DfieldPtr(fieldToPassToTexture_ptr, &dynamicTexture);
    }

    //END:
    GUI::shutDownImGui();
    shutDownGLFW(window);

    return rendererRetCode_st::OK;
}

void F_V2::rendererMainForSeparateThread(bool* externalBool_ptr, IMG::generic2DfieldPtr_t* dynamicData_ptr, 
                                         COLOR::rgbaF_t* clearColor_ptr, COLOR::rgbaF_t* noiseTint_ptr, 
                                         F_V2::rendererRetCode_st* returnCode_ptr, 
                                         COLOR::colorInterpolation_t* scheme_ptr = {},
                                         std::string windowName = "Ogl3 Render Test - imGui + Glfw", 
                                         int width = 800, int height = 600, 
                                         const char* bannerPathFromBinary = F_V2::testBannerPathFromBinary) {

    *returnCode_ptr = 
        F_V2::rendererMain(externalBool_ptr, dynamicData_ptr, clearColor_ptr, noiseTint_ptr, scheme_ptr, 
                                                        windowName, width, height, bannerPathFromBinary);
    return;
}

[[nodiscard]] std::thread F_V2::spawnRendererOnNewThread(bool* externalBool_ptr, 
                                                         IMG::generic2DfieldPtr_t* dynamicData_ptr, 
                                                         COLOR::rgbaF_t* clearColor_ptr, 
                                                         COLOR::rgbaF_t* noiseTint_ptr, 
                                                         F_V2::rendererRetCode_st* returnCode_ptr, 
                                                         COLOR::colorInterpolation_t* scheme_ptr,
                                                         std::string windowName,
                                                         int width, int height, 
                                                         const char* bannerPathFromBinary) {

    return std::thread(F_V2::rendererMainForSeparateThread, externalBool_ptr, dynamicData_ptr, clearColor_ptr, 
                                                            noiseTint_ptr, returnCode_ptr, scheme_ptr,
                                                            windowName, width, height, bannerPathFromBinary);
}

F_V2::rendererRetCode_st F_V2::spawnRendererOnThisThread(bool* externalBool_ptr, 
                                                         IMG::generic2DfieldPtr_t* dynamicData_ptr, 
									                     COLOR::rgbaF_t* clearColor_ptr, 
                                                         COLOR::rgbaF_t* noiseTint_ptr, 
                                                         COLOR::colorInterpolation_t* scheme_ptr,
                                                         std::string windowName,
                                                         int width, int height, 
		                                                 const char* bannerPathFromBinary){

    return F_V2::rendererMain(externalBool_ptr, dynamicData_ptr, clearColor_ptr, noiseTint_ptr, scheme_ptr,
                                                           windowName, width, height, bannerPathFromBinary);
}