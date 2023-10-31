#define STB_IMAGE_IMPLEMENTATION
#include "utils/imageUtils.hpp"

// Loads rgbaImage_t from a file. The data buffer becomes a responsability of the caller.
IMG::rgbaImage_t IMG::load4channel8bpcImageFromFile(const char* filename) {

    IMG::rgbaImage_t image;
    image.data = stbi_load(filename, &image.width, &image.height, NULL, 4);

    return image;
}