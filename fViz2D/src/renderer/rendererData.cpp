#include "renderer/rendererData.hpp"

bool TEX::createOGLtex(GLenum minFilter, GLenum magFilter, GLuint* ID_ptr) {

    glGenTextures(1, ID_ptr);
    glBindTexture(GL_TEXTURE_2D, *ID_ptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    //TODO: check ogl errors

    return true;
}

bool TEX::textureID_t::createOGLtexID(GLenum minFilter, GLenum magFilter) {

    this->initialized = TEX::createOGLtex(minFilter, magFilter, &(this->ID));

    return this->initialized;
}

void TEX::load4channelTextureFromRgbaImage(IMG::rgbaImage_t* image_ptr, TEX::textureID_t* texture_ptr) {

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

TEX::textureID_t TEX::load4channelTextureFromFile(const char* filename) {

    TEX::textureID_t textureID;
    textureID.initialized = false;

    IMG::rgbaImage_t image = IMG::load4channel8bpcImageFromFile(filename);
    if (image.data == NULL) {
        return textureID;
    }

    TEX::load4channelTextureFromRgbaImage(&image, &textureID);

    return textureID;
}