//Note: had to add "../" to "imgui.h"'s path on the backend cpp's and h's 

//IMGUI BUILDING:
#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui/imgui.h"

#ifdef IMGUI_ENABLE_FREETYPE
	static_assert("not implemented");
#endif

#include "imgui/imgui.cpp"

#include "imgui/backends/imgui_impl_opengl3.cpp"
#include "imgui/backends/imgui_impl_glfw.cpp"

#include "imgui/imgui_demo.cpp"
#include "imgui/imgui_draw.cpp"
#include "imgui/imgui_tables.cpp"
#include "imgui/imgui_widgets.cpp"

//IMGUI LAYER:
#include "layers/imGuiLayer.hpp"

ImGuiIO& GUI::initImgui(GLFWwindow* window, const char* glsl_version) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io; //TODO: why (void)io?
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style:
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return io;
}

void GUI::shutDownImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return;
}

void GUI::imGuiNewFrame() {
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::imGuiDrawGUIandUpdateOsWindows(ImDrawData* drawData_ptr, ImGuiIO& io) {

    ImGui_ImplOpenGL3_RenderDrawData(drawData_ptr);

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    return;
}

ImGuiID GUI::createTransparentDockNodeOverMainViewport() {
    return ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void GUI::render() { ImGui::Render(); }