#include "utils/guiUtils.hpp"

void GUI::imGuiDrawTexture(TEX::textureID_t* texID_ptr, const char* windowName) {

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin(windowName, (bool*)0, ImGuiWindowFlags_AlwaysUseWindowPadding);   

    ImGui::BeginChild("Texture", ImVec2(0.0f, 0.0f), false); //child: more custmization   

    ImGui::Image((ImTextureID)texID_ptr->ID, ImGui::GetWindowSize());

    ImGui::EndChild();
    
    ImGui::End();

    ImGui::PopStyleVar(2);
}

void GUI::imGuiTestMenu(ImGuiIO& io, float* clearColorFirstElement_ptr, float* noiseTintColorFirstElement_ptr,
                                                                  bool* keepRendering_ptr, bool* testBool_ptr) {
    //static float f = 0.0f;
    //static int counter = 0;

    ImGui::Begin("Test Menu");                          

    ImGui::Text("This is a test menu."); 
    ImGui::SameLine();
    ImGui::Checkbox("Is this working?", testBool_ptr);
               
    ImGui::ColorEdit4("clear color", clearColorFirstElement_ptr); 
    ImGui::ColorEdit4("noise tint", noiseTintColorFirstElement_ptr); 

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    
    ImGui::SameLine();
    *keepRendering_ptr = !ImGui::Button("Exit");

    ImGui::End();
}