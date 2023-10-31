#define STB_IMAGE_IMPLEMENTATION
#include "utils/imageUtils.hpp"

IMG::rgbaImage_t IMG::load4channel8bpcImageFromFile(const char* filename) {

    IMG::rgbaImage_t image;
    image.data = stbi_load(filename, (int*)&image.size.width, (int*)&image.size.height, NULL, 4);
    image.size.initialized = true;

    return image;
}

void* createBufferFor2Dfield(size_t width, size_t height, IMG::imgSizeInfo_t* size_ptr) {
    if(size_ptr->bytesPerChannel <= 0) { return NULL; }

    size_ptr->width = width;
    size_ptr->height = height;

    void* data_ptr = malloc(size_ptr->getTotalElements() * size_ptr->bytesPerChannel);
    if(data_ptr != NULL) { size_ptr->initialized = true; }

    return data_ptr;
}

IMG::doubles2Dfield_t IMG::createDoubles2Dfield(size_t width, size_t height) {
    IMG::doubles2Dfield_t newField;

    newField.data = (double*)createBufferFor2Dfield(width, height, &newField.size);
    
    return newField;
}

IMG::floats2Dfield_t IMG::createFloat2Dfield(size_t width, size_t height) {
    IMG::floats2Dfield_t newField;

    newField.data = (float*)createBufferFor2Dfield(width, height, &newField.size);
    
    return newField;
}

F_V2::texRetCode_st IMG::copy2Dfield(const IMG::floats2Dfield_t* origin_ptr, 
                                           IMG::doubles2Dfield_t* dest_ptr) {

    auto originSize = origin_ptr->size;
    auto destSize = origin_ptr->size;
    size_t lastIndexOrigin = originSize.getMaxIndex();
    if (lastIndexOrigin != destSize.getMaxIndex()) {
        return F_V2::texRetCode_st::SIZES_DONT_MATCH_FOR_COPY;
    }

    for (int i = 0; i < lastIndexOrigin; i++) {
        dest_ptr->data[i] = (double)origin_ptr->data[i];
    }
    
    return F_V2::texRetCode_st::OK;
}

F_V2::texRetCode_st IMG::copy2Dfield(const IMG::doubles2Dfield_t* origin_ptr, 
                                               IMG::floats2Dfield_t* dest_ptr) {

    auto originSize = origin_ptr->size;
    auto destSize = origin_ptr->size;
    int lastIndexOrigin = originSize.getMaxIndex();
    if (lastIndexOrigin != destSize.getMaxIndex()) {
        return F_V2::texRetCode_st::SIZES_DONT_MATCH_FOR_COPY;
    }

    for (int i = 0; i < lastIndexOrigin; i++) {
        dest_ptr->data[i] = (float)origin_ptr->data[i];
    }
    
    return F_V2::texRetCode_st::OK;
}