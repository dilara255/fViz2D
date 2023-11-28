#pragma once

#include "utils/imageUtils.hpp" //TODO: I don't want this to include any non-api header
#include "returnCodes.hpp"

namespace F_V2 {

	//Returns BAD_DYNAMIC_DATA_FORMAT dynamicData_ptr's format is not supported, otherwise returns OK
	//Supported formats are the same as those supported by the texture loading functions in renderData.hpp
	F_V2::rendererRetCode_st rendererMain(bool* externalBool_ptr, bool* shouldInterpolate_ptr,
		             IMG::generic2DfieldPtr_t* dynamicData_ptr, 
		             COLOR::rgbaF_t* clearColor_ptr, COLOR::rgbaF_t* noiseTint_ptr,
		             COLOR::colorInterpolation_t* scheme_ptr,
		             std::string windowName, int width, int height, const char* bannerPathFromBinary);

	//Sets returnCode_ptr to BAD_DYNAMIC_DATA_FORMAT if dynamicData_ptr's format is not supported, otherwise to OK
	//Supported formats are the same as those supported by the texture loading functions in renderData.hpp
	void rendererMainForSeparateThread(bool* externalBool_ptr, bool* shouldInterpolate_ptr, 
		                               IMG::generic2DfieldPtr_t* dynamicData_ptr, 
									   COLOR::rgbaF_t* clearColor_ptr, COLOR::rgbaF_t* noiseTint_ptr, 
		                               F_V2::rendererRetCode_st* returnCode_ptr, 
		                               COLOR::colorInterpolation_t* scheme_ptr,
		                               std::string windowName, int width, int height, 
		                               const char* bannerPathFromBinary);

	void saveCurrentImage();
}