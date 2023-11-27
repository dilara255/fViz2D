#pragma once

#include "core.hpp"

#include "utils/imageUtils.hpp"
#include "utils/guiUtils.hpp"
#include "renderer/rendererData.hpp"

#include "resourcePaths.hpp"

namespace F_V2 {

	//dynamicData_ptr should hold either a rgbaImage_t or a floats2Dfield_t (see imageUtils.hpp)
	//In case of a bad dynamicData_ptr type, returns BAD_DYNAMIC_DATA_FORMAT, otherwise returns OK
	//TODO: ADD TEST
	F_V2_API F_V2::rendererRetCode_st spawnRendererOnThisThread(bool* externalBool_ptr, 
		                               IMG::generic2DfieldPtr_t* dynamicData_ptr, COLOR::rgbaF_t* clearColor_ptr, 
		                               COLOR::rgbaF_t* noiseTint_ptr, 
		                               COLOR::colorInterpolation_t* scheme_ptr = {},
		                               std::string windowName = "fViz2D Window",
		                               int width = 800, int height = 600, 
		                               const char* bannerPathFromBinary  = F_V2::testBannerPathFromBinary);

	//dynamicData_ptr should hold either a rgbaImage_t or a floats2Dfield_t (see imageUtils.hpp)
	//In case of a bad dynamicData_ptr type, returns "empty" thread and sets returnCode to BAD_DYNAMIC_DATA_FORMAT
	F_V2_API std::thread spawnRendererOnNewThread(bool* externalBool_ptr, IMG::generic2DfieldPtr_t* dynamicData_ptr, 
									          COLOR::rgbaF_t* clearColor_ptr, COLOR::rgbaF_t* noiseTint_ptr, 
		                                      F_V2::rendererRetCode_st* returnCode_ptr, 
		                                      COLOR::colorInterpolation_t* scheme_ptr = {},
											  std::string windowName = "fViz2D Window",                           
											  int width = 800, int height = 600, 
		                                      const char* bannerPathFromBinary  = F_V2::testBannerPathFromBinary);
}