#pragma once

#include "utils/imageUtils.hpp"

namespace F_V2 {
	int rendererMain(bool* externalBool_ptr, IMG::rgbaImage_t* dynamicData_ptr, 
		             COLOR::rgbaF_t* clearColor_ptr, COLOR::rgbaF_t* noiseTint_ptr);
	void rendererMainForSeparateThread(bool* externalBool_ptr, IMG::rgbaImage_t* dynamicData_ptr, 
									   COLOR::rgbaF_t* clearColor_ptr, COLOR::rgbaF_t* noiseTint_ptr, 
		                                                                         int* returnCode_ptr);

	enum renrederRetCodes {OK, STILL_RUNNING, CONTEXT_ACQ_FAILED, IMAGE_LOAD_FAILED, DYNAMIC_IMAGE_INITIAL_LOAD_FAILED};
}