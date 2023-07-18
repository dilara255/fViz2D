#include "layers/glfwOglLayer.hpp"

GLFWwindow* initGlfwAndCreateWindow(GLFWerrorfun glfwErrorCallback, int width, int height, 
                                                            const char* title, bool vsync) {
    
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(vsync); // Enable vsync

    return window;
}

void shutDownGLFW(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();

    return;
}

void clearFrameBuffer(GLFWwindow* window, COLOR::rgbaF_t clearColor) {
    
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clearColor.r * clearColor.a, clearColor.g * clearColor.a, clearColor.b * clearColor.a, clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);

    return;
}