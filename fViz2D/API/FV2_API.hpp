#pragma once

#include "core.hpp"

#include "utils/imageUtils.hpp"
#include "utils/guiUtils.hpp"
#include "renderer/rendererData.hpp"

#include "resourcePaths.hpp"

namespace F_V2 {
	F_V2_API std::thread spawnRendererOnNewThread(bool* externalBool_ptr, IMG::rgbaImage_t* dynamicData_ptr, 
									          COLOR::rgbaF_t* clearColor_ptr, COLOR::rgbaF_t* noiseTint_ptr, 
		                                      F_V2::rendererRetCode_st* returnCode_ptr, 
		                                      const char* bannerPathFromBinary  = F_V2::testBannerPathFromBinary);

	//TODO: ADD TEST
	F_V2_API F_V2::rendererRetCode_st spawnRendererOnThisThread(bool* externalBool_ptr, 
		                               IMG::rgbaImage_t* dynamicData_ptr, COLOR::rgbaF_t* clearColor_ptr, 
		                               COLOR::rgbaF_t* noiseTint_ptr, 
		                               const char* bannerPathFromBinary  = F_V2::testBannerPathFromBinary);
}