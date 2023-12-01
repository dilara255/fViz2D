#include "GUI_API.hpp"
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

void GUI::imGuiCreateMenu(menuDefinition_t menuDefinition, F_V2::rendererControlPtrs_t* rendererControl_ptr) {
    
    if(menuDefinition.menuName == "") { menuDefinition.menuName = "Menu"; }

    ImGui::Begin(menuDefinition.menuName.c_str());             
    
    menuDefinition.menuFunc_ptr(menuDefinition.hooks, rendererControl_ptr);

    ImGui::End();
}

void testMenu(GUI::hookList_t menuElements, F_V2::rendererControlPtrs_t* rendererControl_ptr);
GUI::menuDefinition_t GUI::getTestMenuDefinition(bool* testBool_ptr, float* clearColorFirstElement_ptr, 
                                                 float* noiseTintColorFirstElement_ptr) {

    GUI::menuDefinition_t definition;
    definition.menuFunc_ptr = testMenu;

    definition.hooks.push_back((void*)testBool_ptr);
    definition.hooks.push_back((void*)clearColorFirstElement_ptr);
    definition.hooks.push_back((void*)noiseTintColorFirstElement_ptr);

    definition.menuName = "Test Menu";

    return definition;
}
void testMenu(GUI::hookList_t menuElements, F_V2::rendererControlPtrs_t* rendererControl_ptr) { 
    enum hookEnum { TEST_BOOL, CLEAR_COLOR, NOISE_TINT, TOTAL };
    if(menuElements.size() != TOTAL ) { return; }
   
    ImGui::Text("This is a test menu."); 
    ImGui::SameLine();
    ImGui::Checkbox("Is this working?", (bool*)menuElements.at(TEST_BOOL));
               
    ImGui::ColorEdit4("clear color", (float*)menuElements.at(CLEAR_COLOR)); 
    ImGui::ColorEdit4("noise tint", (float*)menuElements.at(NOISE_TINT));
}