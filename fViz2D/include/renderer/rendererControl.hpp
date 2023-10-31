#pragma once

#include "utils/imageUtils.hpp" //TODO: I don't want this to include any non-api header
#include "returnCodes.hpp"

namespace F_V2 {
	F_V2::rendererRetCode_st rendererMain(bool* externalBool_ptr, IMG::rgbaImage_t* dynamicData_ptr, 
		             COLOR::rgbaF_t* clearColor_ptr, COLOR::rgbaF_t* noiseTint_ptr,
		             const char* bannerPathFromBinary);

	void rendererMainForSeparateThread(bool* externalBool_ptr, IMG::rgbaImage_t* dynamicData_ptr, 
									   COLOR::rgbaF_t* clearColor_ptr, COLOR::rgbaF_t* noiseTint_ptr, 
		                               F_V2::rendererRetCode_st* returnCode_ptr, 
		                               const char* bannerPathFromBinary);
}