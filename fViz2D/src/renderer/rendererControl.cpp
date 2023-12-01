// OpenGL3 renderer using glfw and imGui, based on imGui's example
// NOTE: loads a image located on the solutions "res" folder:
// =======> May fail depending on working directory
// TODO: make it checked and etc;

#include "timeHelpers.hpp"

#include "layers/glfwOglLayer.hpp"
#include "layers/imGuiLayer.hpp"

#include "FV2_API.hpp"
#include "GUI_API.hpp"

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

void F_V2::defaultSynchCallback() {
    return;
}

void rendererMenu(GUI::hookList_t hooks, F_V2::rendererControlPtrs_t* rendererControl_ptr) {

    auto io_ptr = (const ImGuiIO*)rendererControl_ptr->ImGuiIO_ptr;

    *rendererControl_ptr->keepRendering_ptr = !ImGui::Button("Exit");

    ImGui::SameLine();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io_ptr->Framerate, io_ptr->Framerate);    

    ImGui::SameLine();
    if(ImGui::Button("Save image")) { *rendererControl_ptr->shouldSave_ptr = true; }

    ImGui::SameLine();
    ImGui::Checkbox("Interpolate colors?", rendererControl_ptr->shouldInterpolateColors_ptr);    
}            

void render(GLFWwindow* window, TEX::textureID_t* dynamicTexture_ptr, GUI::menuDefinition_t userDef,
            TEX::textureID_t* bannerTexture_ptr, COLOR::rgbaF_t* clearColor_ptr, 
            F_V2::rendererControlPtrs_t* rendererControl_ptr, bool createDefaultRendererMenu) {

    GUI::menuDefinition_t rendererMenuDef;
    rendererMenuDef.menuFunc_ptr = rendererMenu;
    rendererMenuDef.menuName = "Renderer Menu";

    GUI::imGuiNewFrame();
    GUI::createTransparentDockNodeOverMainViewport();      
    if(userDef.menuFunc_ptr != nullptr) { GUI::imGuiCreateMenu(userDef); }
    if(createDefaultRendererMenu) { GUI::imGuiCreateMenu(rendererMenuDef, rendererControl_ptr); }
    GUI::imGuiDrawTexture(bannerTexture_ptr);
    GUI::imGuiDrawTexture(dynamicTexture_ptr, "Dynamic Data");
    GUI::render();

    clearFrameBuffer(window, *clearColor_ptr);

    GUI::imGuiDrawGUIandUpdateOsWindows(ImGui::GetDrawData(), (ImGuiIO&)rendererControl_ptr->ImGuiIO_ptr);

    glfwSwapBuffers(window);
}

bool mightInterpolateColors(COLOR::colorInterpolation_t* scheme_ptr, IMG::kinds2Ddata kind) {
    return (scheme_ptr != nullptr) && (scheme_ptr->correspondences_ptr->size() >= 2) && 
           (kind == IMG::kinds2Ddata::FLOATS_FIELD || kind == IMG::kinds2Ddata::DOUBLES_FIELD);
}

bool shouldInterpolateColors(bool* shouldInterpolate_ptr, COLOR::colorInterpolation_t* scheme_ptr,
                                                                            IMG::kinds2Ddata kind) {

    bool should = *shouldInterpolate_ptr && mightInterpolateColors(scheme_ptr, kind);
    //If the user presses the button but mightInterpolateColors is false, we want the button to de-press:
    *shouldInterpolate_ptr = should; 

    return should;
}

//TODO-ARQ: some of the stuff used here could be pulled into and kept by a renderer class
F_V2::rendererRetCode_st F_V2::rendererMain(IMG::generic2DfieldPtr_t* dynamicData_ptr,
                                            COLOR::rgbaF_t* clearColor_ptr,
                                            GUI::menuDefinition_t userMenuDef,
                                            GUI::filenameCallback_func* filenameFunc,
                                            COLOR::colorInterpolation_t* scheme_ptr,
                                            std::string windowName, 
                                            int width, int height,
                                            bool createDefaultRendererMenu,
                                            synchCallback_func synchCallback,
                                            const char* bannerPathFromBinary) {

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
    
    auto size_ptr = dynamicData_ptr->getSizeInfo_ptr();
    size_t imageWidth = size_ptr->width;
    size_t imageHeight = size_ptr->height;    

    IMG::rgbaImage_t rgbaImage;
    if(mightInterpolateColors(scheme_ptr, kind)) {
        rgbaImage = IMG::createEmpty4channel8bpcImage(imageWidth, imageHeight);
        colorInterpolationField.storeRGBAfield(&rgbaImage);
    }
    
    F_V2::texRetCode_st colorInterpReturn = F_V2::texRetCode_st::OK;

    //Prepare the rendererControlPtrs_t structure:
    bool shouldInterpolate = mightInterpolateColors(scheme_ptr, kind);
    bool shouldSave = false;
    int steps = 0;

    rendererControlPtrs_t rendererControl;
    rendererControl.loadPointers(&shouldInterpolate, &keepRunning, (void*)&io, &shouldSave, &steps);

    //RUN:
    std::string filename;
    while (!glfwWindowShouldClose(window) && keepRunning) {
        glfwPollEvents(); //for app: check io.WantCaptureMouse and io.WantCaptureKeyboard

        render(window, &dynamicTexture, userMenuDef, &bannerTexture, clearColor_ptr, 
                                        &rendererControl, createDefaultRendererMenu);

        if (shouldSave) { 
            if(filenameFunc != nullptr) { filename = filenameFunc(steps); }
            else(filename = std::to_string(steps));
            IMG::saveImage(fieldToPassToTexture_ptr, filename, IMG::imageType::JPG); 
            shouldSave = false;
        }

        //TODO: not really checking return of these
        if(shouldInterpolateColors(&shouldInterpolate, scheme_ptr, kind)) {
            fieldToPassToTexture_ptr = &colorInterpolationField;
            colorInterpReturn = IMG::translateValuesToInterpolatedColors(dynamicData_ptr, &rgbaImage, scheme_ptr);
            assert(colorInterpReturn == F_V2::texRetCode_st::OK);
        }
        else { fieldToPassToTexture_ptr = dynamicData_ptr; }

        TEX::loadTextureFromGeneric2DfieldPtr(fieldToPassToTexture_ptr, &dynamicTexture);

        steps++;

        synchCallback();
    }

    //END:
    GUI::shutDownImGui();
    shutDownGLFW(window);

    return rendererRetCode_st::OK;
}

void F_V2::rendererMainForSeparateThread(IMG::generic2DfieldPtr_t* dynamicData_ptr, 
		                                 F_V2::rendererRetCode_st* returnCode_ptr, 
									     COLOR::rgbaF_t* clearColor_ptr,
		                                 GUI::menuDefinition_t userMenuDef,
                                         GUI::filenameCallback_func* filenameFunc,
                                         COLOR::colorInterpolation_t* scheme_ptr,
                                         std::string windowName, 
                                         int width, int height,
                                         bool createDefaultRendererMenu,
                                         synchCallback_func synchCallback,
                                         const char* bannerPathFromBinary) {

    *returnCode_ptr = 
        F_V2::rendererMain(dynamicData_ptr, clearColor_ptr, userMenuDef, filenameFunc, scheme_ptr, windowName, 
                                width, height, createDefaultRendererMenu, synchCallback, bannerPathFromBinary);
    return;
}

[[nodiscard]] std::thread F_V2::spawnRendererOnNewThread(IMG::generic2DfieldPtr_t* dynamicData_ptr, 
		                                                 F_V2::rendererRetCode_st* returnCode_ptr, 
									                     COLOR::rgbaF_t* clearColor_ptr,
		                                                 GUI::menuDefinition_t userMenuDef,
                                                         GUI::filenameCallback_func* filenameFunc,
                                                         COLOR::colorInterpolation_t* scheme_ptr,
                                                         std::string windowName, 
                                                         int width, int height,
                                                         bool createDefaultRendererMenu,
                                                         synchCallback_func synchCallback,
                                                         const char* bannerPathFromBinary) {

    return std::thread(F_V2::rendererMainForSeparateThread, dynamicData_ptr, returnCode_ptr, 
                                                            clearColor_ptr, userMenuDef, filenameFunc,
                                                            scheme_ptr, windowName, width, height,
                                                            createDefaultRendererMenu, synchCallback,
                                                            bannerPathFromBinary);
}

F_V2::rendererRetCode_st F_V2::spawnRendererOnThisThread(IMG::generic2DfieldPtr_t* dynamicData_ptr,
                                                         COLOR::rgbaF_t* clearColor_ptr,
		                                                 GUI::menuDefinition_t userMenuDef,
                                                         GUI::filenameCallback_func* filenameFunc,
                                                         COLOR::colorInterpolation_t* scheme_ptr,
                                                         std::string windowName, 
                                                         int width, int height,
                                                         bool createDefaultRendererMenu,
                                                         synchCallback_func synchCallback,
                                                         const char* bannerPathFromBinary){

    return F_V2::rendererMain(dynamicData_ptr, clearColor_ptr, userMenuDef, filenameFunc, scheme_ptr, 
                              windowName, width, height, createDefaultRendererMenu, synchCallback, 
                              bannerPathFromBinary);
}