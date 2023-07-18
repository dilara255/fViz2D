#define STB_IMAGE_IMPLEMENTATION
#include "stbImage/stb_image.h"
#include "GLFW/glfw3.h"

#include "utils/imageUtils.hpp"

IMG::rgbaImage_t IMG::load4channel8bppImageFromFile(const char* filename) {

    IMG::rgbaImage_t image;
    image.data = stbi_load(filename, &image.width, &image.height, NULL, 4);

    return image;
}

// Simple helper function to load an image into a OpenGL texture with common settings
// Modified from https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
IMG::rgbaTextureID_t IMG::load4channelTextureFromFile(const char* filename) {

    IMG::rgbaTextureID_t texture;
    texture.initialized = false;

    IMG::rgbaImage_t image = load4channel8bppImageFromFile(filename);
    if (image.data == NULL) {
        return texture;
    }

    // Create a OpenGL texture identifier
    GLuint imageTexture;
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__) //TODO: do I need to keep this condition?
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
    stbi_image_free(image.data);

    texture.ID = imageTexture;
    texture.width = image.width;
    texture.height = image.height;
    
    texture.initialized = true;

    return texture;
}