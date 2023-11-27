#define STB_IMAGE_IMPLEMENTATION
#include "utils/imageUtils.hpp"

[[nodiscard]] IMG::rgbaImage_t IMG::load4channel8bpcImageFromFile(const char* filename) {

    IMG::rgbaImage_t image;
    image.data = 
        std::unique_ptr<unsigned char>(stbi_load(filename, (int*)&image.size.width, (int*)&image.size.height, NULL, 4));
    image.size.initialized = true;

    return image;
}

[[nodiscard]] void* createBufferFor2Dfield(size_t width, size_t height, IMG::imgSizeInfo_t* size_ptr) {
    if(size_ptr->bytesPerChannel <= 0) { return NULL; }

    size_ptr->width = width;
    size_ptr->height = height;

    void* data_ptr = malloc(size_ptr->getTotalElements() * size_ptr->bytesPerChannel);
    if(data_ptr != NULL) { size_ptr->initialized = true; }

    return data_ptr;
}

[[nodiscard]] IMG::doubles2Dfield_t IMG::createDoubles2Dfield(size_t width, size_t height) {
    IMG::doubles2Dfield_t newField;

    newField.data = std::unique_ptr<double>((double*)createBufferFor2Dfield(width, height, &newField.size));
    
    return newField;
}

[[nodiscard]] IMG::floats2Dfield_t IMG::createFloats2Dfield(size_t width, size_t height) {
    IMG::floats2Dfield_t newField;

    newField.data = std::unique_ptr<float>((float*)createBufferFor2Dfield(width, height, &newField.size));
    
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
        dest_ptr->data.get()[i] = (double)origin_ptr->data.get()[i];
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
        dest_ptr->data.get()[i] = (float)origin_ptr->data.get()[i];
    }
    
    return F_V2::texRetCode_st::OK;
}