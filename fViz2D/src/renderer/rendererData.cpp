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

F_V2::rendererRetCode_st TEX::loadTextureFromGeneric2DfieldPtr(IMG::generic2DfieldPtr_t* dataPtr_ptr, 
                                                                       TEX::textureID_t* texture_ptr) {
    
    switch (dataPtr_ptr->getKindOfField()) {

        case(IMG::kinds2Ddata::RGBA_IMAGE):
            TEX::load4channelTextureFromRgbaImage(dataPtr_ptr->getFieldPtr().rgbaField_ptr, texture_ptr);
            return F_V2::rendererRetCode_st::OK;

        case(IMG::kinds2Ddata::FLOATS_FIELD):
            TEX::loadR32FtextureFromFloats(dataPtr_ptr->getFieldPtr().floatsField_ptr, texture_ptr);
            return F_V2::rendererRetCode_st::OK;

        default:
            return F_V2::rendererRetCode_st::BAD_DYNAMIC_DATA_FORMAT;
    }
}

void TEX::load4channelTextureFromRgbaImage(IMG::rgbaImage_t* image_ptr, TEX::textureID_t* texture_ptr) {
    if (!image_ptr->size.initialized) { texture_ptr->initialized = false; return; }
    if (!texture_ptr->initialized) { texture_ptr->createOGLtexID(); }

    // Select texture to be updated
    glBindTexture(GL_TEXTURE_2D, texture_ptr->ID);

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_ptr->size.width, image_ptr->size.height, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, image_ptr->data.get());

    //TODO: check OGL errors and add return codes

    texture_ptr->width = image_ptr->size.width;
    texture_ptr->height = image_ptr->size.height;

    return;
}

void TEX::loadR32FtextureFromFloats(IMG::floats2Dfield_t* field_ptr, TEX::textureID_t* texture_ptr) {
    if (!field_ptr->size.initialized) { texture_ptr->initialized = false; return; }
    if (!texture_ptr->initialized) { texture_ptr->createOGLtexID(); }

    // Select texture to be updated
    glBindTexture(GL_TEXTURE_2D, texture_ptr->ID);

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, field_ptr->size.width, field_ptr->size.height, 
                 0, GL_RED, GL_FLOAT, field_ptr->data.get());

    //TODO: check OGL errors and add return codes

    texture_ptr->width = field_ptr->size.width;
    texture_ptr->height = field_ptr->size.height;

    return;
}

TEX::textureID_t TEX::load4channelTextureFromFile(const char* filename) {

    TEX::textureID_t textureID;

    IMG::rgbaImage_t image = IMG::load4channel8bpcImageFromFile(filename);
    if (image.data == NULL) {
        return textureID;
    }

    TEX::load4channelTextureFromRgbaImage(&image, &textureID);

    return textureID;
}