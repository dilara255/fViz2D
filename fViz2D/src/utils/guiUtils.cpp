#include "utils/guiUtils.hpp"

void imGuiDrawTexture(IMG::rgbaTextureID_t texID) {

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("Texture Drawing", (bool*)0, ImGuiWindowFlags_AlwaysUseWindowPadding);   

    ImGui::BeginChild("Texture", ImVec2(0.0f, 0.0f), false); //child: more custmization   

    ImGui::Image((ImTextureID)texID.ID, ImGui::GetWindowSize());

    ImGui::EndChild();
    
    ImGui::End();

    ImGui::PopStyleVar(2);
}