#pragma once

namespace F_V2 {
	int rendererMain(bool* externalBool_ptr);

	enum renrederRetCodes {OK, CONTEXT_ACQ_FAILED, IMAGE_LOAD_FAILED};
}