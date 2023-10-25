#pragma once

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h" // Will drag system OpenGL headers

ImGuiIO& initImgui(GLFWwindow* window, const char* glsl_version);
void shutDownImGui();

void imGuiNewFrame();
void imGuiDrawGUIandUpdateOsWindows(ImDrawData* drawData_ptr, ImGuiIO& io);

void imGuiTestMenu(ImGuiIO& io, float* clearColorFirstElement_ptr, float* noiseTintColorFirstElement_ptr,
	                                                         bool* keepRendering_ptr, bool* testBool_ptr);