#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "utils/imageUtils.hpp"
#include <algorithm>

F_V2::imageFileRetCode_st IMG::saveImage(const generic2DfieldPtr_t* image_ptr, std::string filename, 
		                                            imageType_t type, int quality, std::string path,
		                                                                    double min, double span) {

	kinds2Ddata kind = image_ptr->getKindOfField();
	if(kind == kinds2Ddata::UNINITIALIZED_UNION) { return F_V2::imageFileRetCode_st::BAD_DATA_TO_SAVE; }

	auto sizeInfo_ptr = image_ptr->getSizeInfo_ptr();
	int w = (int)sizeInfo_ptr->width;
	int h = (int)sizeInfo_ptr->height;
	int comp = sizeInfo_ptr->channels;

	std::string fullPath = path + filename;
	int internalReturn;

    //JPG AND PNG EXPECT 8BPC, so:
    IMG::grey8bpcImage_t tempGReyImage;
    const void* dataPtrToUse = image_ptr->getVoidData_ptr();
    auto sizePtrToUse =  sizeInfo_ptr;

    if (sizeInfo_ptr->bytesPerChannel != 1) {
        switch (kind) {

            case kinds2Ddata::FLOATS_FIELD: 
                tempGReyImage = 
                    IMG::copy2DfieldToNewGreyscaleImage(image_ptr->getConstFieldPtr().floatsField_ptr, min, span);
                    dataPtrToUse = tempGReyImage.data.get();
                    sizePtrToUse = &tempGReyImage.size;
            break;
            case kinds2Ddata::DOUBLES_FIELD:
                tempGReyImage = 
                    IMG::copy2DfieldToNewGreyscaleImage(image_ptr->getConstFieldPtr().doublesField_ptr, min, span);
                    dataPtrToUse = tempGReyImage.data.get();
                    sizePtrToUse = &tempGReyImage.size;
            break;
            default: 
                return F_V2::imageFileRetCode_st::BAD_DATA_TO_SAVE;
        }
    }

	switch (type) {
		case imageType_t::JPG:
            fullPath += ".jpg";
			internalReturn = 
				stbi_write_jpg(fullPath.c_str(), w, h, comp, dataPtrToUse, quality);
		break;
		case imageType_t::PNG:
            fullPath += ".png";
			internalReturn = 
				stbi_write_png(fullPath.c_str(), w, h, comp, dataPtrToUse, (int)sizePtrToUse->getStrideInBytes());
		break;
	}
		
	if(internalReturn) { return F_V2::imageFileRetCode_st::OK; }
	else { return F_V2::imageFileRetCode_st::SAVE_FAILED; }
}

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

[[nodiscard]] IMG::rgbaImage_t IMG::createEmpty4channel8bpcImage(size_t width, size_t height) {
    IMG::rgbaImage_t newField;

    newField.data = std::unique_ptr<unsigned char>((unsigned char*)createBufferFor2Dfield(width, height, &newField.size));
    
    return newField;
}

[[nodiscard]] IMG::grey8bpcImage_t IMG::createEmpty1channel8bpcImage(size_t width, size_t height) {
    IMG::grey8bpcImage_t newField;

    newField.data = std::unique_ptr<unsigned char>((unsigned char*)createBufferFor2Dfield(width, height, &newField.size));
    
    return newField;
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
    auto destSize = dest_ptr->size;
    size_t lastIndexOrigin = originSize.getMaxIndex();
    if (lastIndexOrigin != destSize.getMaxIndex()) {
        return F_V2::texRetCode_st::SIZES_DONT_MATCH_FOR_COPY;
    }

    for (size_t i = 0; i < lastIndexOrigin; i++) {
        dest_ptr->data.get()[i] = (double)origin_ptr->data.get()[i];
    }
    
    return F_V2::texRetCode_st::OK;
}

F_V2::texRetCode_st IMG::copy2Dfield(const IMG::doubles2Dfield_t* origin_ptr, 
                                               IMG::floats2Dfield_t* dest_ptr) {

    auto originSize = origin_ptr->size;
    auto destSize = dest_ptr->size;
    size_t lastIndexOrigin = originSize.getMaxIndex();
    if (lastIndexOrigin != destSize.getMaxIndex()) {
        return F_V2::texRetCode_st::SIZES_DONT_MATCH_FOR_COPY;
    }

    for (size_t i = 0; i < lastIndexOrigin; i++) {
        dest_ptr->data.get()[i] = (float)origin_ptr->data.get()[i];
    }
    
    return F_V2::texRetCode_st::OK;
}

IMG::grey8bpcImage_t IMG::copy2DfieldToNewGreyscaleImage(const floats2Dfield_t* floatOrigin_ptr, 
                                                                        double min, double span) {
    IMG::grey8bpcImage_t image;
    
    if(!floatOrigin_ptr->size.initialized || (span <= 0) ) { return image; }

    auto originSize = floatOrigin_ptr->size;
    size_t lastIndexOrigin = originSize.getMaxIndex();

    image = createEmpty1channel8bpcImage(originSize.width, originSize.height);
    auto dest_ptr = image.data.get();

    float value;
    for (size_t i = 0; i < lastIndexOrigin; i++) {
        value = floatOrigin_ptr->data.get()[i];
        //map [min, min + span] to [0, 1]:
        value -= min;
        value /= span;
        //and clamp:
        value = std::clamp(value, 0.0f, 1.0f);

        dest_ptr[i] = (unsigned char)255*value;
    }

    return image;
}

IMG::grey8bpcImage_t IMG::copy2DfieldToNewGreyscaleImage(const doubles2Dfield_t* doubleOrigin_ptr, 
                                                                         double min, double span) {
    IMG::grey8bpcImage_t image;
    
    if(!doubleOrigin_ptr->size.initialized || (span <= 0) ) { return image; }

    auto originSize = doubleOrigin_ptr->size;
    size_t lastIndexOrigin = originSize.getMaxIndex();

    image = createEmpty1channel8bpcImage(originSize.width, originSize.height);
    auto dest_ptr = image.data.get();

    double value;
    for (size_t i = 0; i < lastIndexOrigin; i++) {
        value = doubleOrigin_ptr->data.get()[i];
        //map [min, min + span] to [0, 1]:
        value -= min;
        value /= span;
        //and clamp:
        value = std::clamp(value, 0.0, 1.0);

        dest_ptr[i] = (unsigned char)255*value;
    }

    return image;
}

F_V2::texRetCode_st IMG::translateValuesToInterpolatedColors(const generic2DfieldPtr_t* valuesField_ptr, 
									                        rgbaImage_t* imageField_ptr, 
															const COLOR::colorInterpolation_t* scheme_ptr) {
    
    IMG::kinds2Ddata kind = valuesField_ptr->getKindOfField();

    switch (kind) {
        
        case IMG::kinds2Ddata::FLOATS_FIELD:
            return translateValuesToInterpolatedColors(valuesField_ptr->getConstFieldPtr().floatsField_ptr,
                                                                                imageField_ptr, scheme_ptr);
        case IMG::kinds2Ddata::DOUBLES_FIELD:
            return translateValuesToInterpolatedColors(valuesField_ptr->getConstFieldPtr().doublesField_ptr,
                                                                                 imageField_ptr, scheme_ptr);
        default:
            return F_V2::texRetCode_st::BAD_VALUE_FIELD;
    }
}

F_V2::texRetCode_st IMG::translateValuesToInterpolatedColors(const floats2Dfield_t* valuesField_ptr, 
									                        rgbaImage_t* imageField_ptr, 
															const COLOR::colorInterpolation_t* scheme_ptr) {

    auto originSize = valuesField_ptr->size;
    auto destSize = imageField_ptr->size;
    if (originSize.getTotalArea() != destSize.getTotalArea()) {
        return F_V2::texRetCode_st::SIZES_DONT_MATCH_FOR_COPY;
    }

    COLOR::rgbaC_t pixelColor;
    auto imageData_ptr = imageField_ptr->data.get();

    for (size_t i = 0; i < originSize.getMaxIndex(); i++) {
        pixelColor = COLOR::interpolatedColorFromValue(valuesField_ptr->data.get()[i], scheme_ptr);

        imageData_ptr[destSize.getLinearIndexOfChannel(i, COLOR::R)] = pixelColor.r;
        imageData_ptr[destSize.getLinearIndexOfChannel(i, COLOR::G)] = pixelColor.g;
        imageData_ptr[destSize.getLinearIndexOfChannel(i, COLOR::B)] = pixelColor.b;
        imageData_ptr[destSize.getLinearIndexOfChannel(i, COLOR::A)] = pixelColor.a;
    }

    return F_V2::texRetCode_st::OK;
}

F_V2::texRetCode_st IMG::translateValuesToInterpolatedColors(const doubles2Dfield_t* valuesField_ptr, 
															rgbaImage_t* imageField_ptr, 
															const COLOR::colorInterpolation_t* scheme_ptr) {

    auto originSize = valuesField_ptr->size;
    auto destSize = imageField_ptr->size;
    if (originSize.getTotalArea() != destSize.getTotalArea()) {
        return F_V2::texRetCode_st::SIZES_DONT_MATCH_FOR_COPY;
    }

    COLOR::rgbaC_t pixelColor;
    auto imageData_ptr = imageField_ptr->data.get();

    for (size_t i = 0; i < originSize.getMaxIndex(); i++) {
        pixelColor = COLOR::interpolatedColorFromValue(valuesField_ptr->data.get()[i], scheme_ptr);

        imageData_ptr[destSize.getLinearIndexOfChannel(i, COLOR::R)] = pixelColor.r;
        imageData_ptr[destSize.getLinearIndexOfChannel(i, COLOR::G)] = pixelColor.g;
        imageData_ptr[destSize.getLinearIndexOfChannel(i, COLOR::B)] = pixelColor.b;
        imageData_ptr[destSize.getLinearIndexOfChannel(i, COLOR::A)] = pixelColor.a;
    }

    return F_V2::texRetCode_st::OK;
}

COLOR::rgbaC_t COLOR::interpolateTwoColors(double t, const rgbaC_t* colorBefore_ptr, const rgbaC_t* colorAfter_ptr) {

    double oneMinusT = 1 - t;

    return { 
        (unsigned char)( oneMinusT*colorBefore_ptr->r + t*colorAfter_ptr->r ) , 
        (unsigned char)( oneMinusT*colorBefore_ptr->g + t*colorAfter_ptr->g ) ,
        (unsigned char)( oneMinusT*colorBefore_ptr->b + t*colorAfter_ptr->b ) ,
        (unsigned char)( oneMinusT*colorBefore_ptr->a + t*colorAfter_ptr->a )
    };
}

//If an empty scheme is passed, returns the "value" clamped and cast to 8 bits in the red channel
//If an scheme with a single correspondence is passed or there's another error, returns DEBUG_PINK_8B
COLOR::rgbaC_t COLOR::interpolatedColorFromValue(double value, const colorInterpolation_t* scheme_ptr) {

    size_t schemeSize = scheme_ptr->correspondences_ptr->size();

    if(schemeSize == 0) { return { (unsigned char)(std::clamp(value, 0.0, 1.0)*255), 0, 0, 255 }; }
    if(schemeSize == 1) { return COLOR::DEBUG_PINK_8B; }
   
    size_t indexLargerValue = 0;
    auto correspondences_ptr = scheme_ptr->correspondences_ptr;
    while ( (indexLargerValue < schemeSize) && (correspondences_ptr->at(indexLargerValue).value < value)) {
        indexLargerValue++;
    }

    //We first guess the extremes:
    rgbaC_t colorBefore = correspondences_ptr->at(0).color;
    double valueBefore = correspondences_ptr->at(0).value;
    rgbaC_t colorAfter = correspondences_ptr->back().color;
    double valueAfter = correspondences_ptr->back().value;

    //In case the value is into or after the first interval, we set the before values:
    if(indexLargerValue > 0) { 
        colorBefore = correspondences_ptr->at(indexLargerValue - 1).color; 
        valueBefore = correspondences_ptr->at(indexLargerValue - 1).value;
    }
    
    //In case the value is into or before the last interval, we set the after values:
    if(indexLargerValue < schemeSize) {
        colorAfter = correspondences_ptr->at(indexLargerValue).color; 
        valueAfter = correspondences_ptr->at(indexLargerValue).value; 
    }

    rgbaC_t finalColor;

    //Then we deal with the cases where the value is outside the extremes:
    if(value <= correspondences_ptr->at(0).value) { finalColor = correspondences_ptr->at(0).color; }
    else if(value >= correspondences_ptr->back().value) { finalColor = correspondences_ptr->back().color; }
    //Or interpolate between the values we got before in case it's within the extremes:
    else {
    
        double valueSpan = valueAfter - valueBefore;

        assert(valueSpan > 0);

        double t = (value - valueBefore)/valueSpan;

        finalColor = interpolateTwoColors(t, &colorBefore, &colorAfter);
    }

    return finalColor;
}