#pragma once

#include "utils/imageUtils.hpp"

namespace F_V2 {
	int rendererMain(bool* externalBool_ptr, IMG::rgbaImage_t* dynamicData_ptr);

	enum renrederRetCodes {OK, CONTEXT_ACQ_FAILED, IMAGE_LOAD_FAILED, DYNAMIC_IMAGE_INITIAL_LOAD_FAILED};
}