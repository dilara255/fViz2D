#define STB_IMAGE_IMPLEMENTATION
#include "utils/imageUtils.hpp"

// Loads rgbaImage_t from a file. The data buffer becomes a responsability of the caller.
IMG::rgbaImage_t IMG::load4channel8bppImageFromFile(const char* filename) {

    IMG::rgbaImage_t image;
    image.data = stbi_load(filename, &image.width, &image.height, NULL, 4);

    return image;
}

// Creates an OpenGL texture identifier and associates it with the rgbaTextureID_t
void IMG::rgbaTextureID_t::createOGLtexID(GLenum minFilter, GLenum magFilter) {

    glGenTextures(1, &(this->ID));
    glBindTexture(GL_TEXTURE_2D, this->ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    this->initialized = true;
}

//Loads an IMG::rgbaImage_t image into a texture to be rendered. 
//If the textureID is unitialized, it's initialized first, creating an OpenGL texture identifier.
void IMG::load4channelTextureFromRgbaImage(IMG::rgbaImage_t* image_ptr, IMG::rgbaTextureID_t* texture_ptr) {

    if (!texture_ptr->initialized) { texture_ptr->createOGLtexID(); }

    // Select texture to be updated
    glBindTexture(GL_TEXTURE_2D, texture_ptr->ID);

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_ptr->width, image_ptr->height, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, image_ptr->data);

    texture_ptr->width = image_ptr->width;
    texture_ptr->height = image_ptr->height;

    return;
}

// Loads an image into an OpenGL texture with common settings. Returns an unitialized rgbaTextureID_t on failure.
IMG::rgbaTextureID_t IMG::load4channelTextureFromFile(const char* filename) {

    IMG::rgbaTextureID_t textureID;
    textureID.initialized = false;

    IMG::rgbaImage_t image = load4channel8bppImageFromFile(filename);
    if (image.data == NULL) {
        return textureID;
    }

    IMG::load4channelTextureFromRgbaImage(&image, &textureID);

    return textureID;
}