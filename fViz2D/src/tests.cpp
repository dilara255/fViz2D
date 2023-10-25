#include "FV2_testsAPI.hpp"

#include "renderer/rendererControl.hpp"
#include "utils/consoleLogging.hpp"

namespace F_V2 {

	bool linkAndLogTest(void) {
		LOG_INFO("HEY, THIS IS fViz2D!");
		return true;
	}

	bool rendererTest(void) {

		LOG_DEBUG("This is a visual test for fViz2D\n"); GETCHAR_PAUSE;

		bool passed = false;

		int returnCode = rendererMain(&passed);

		if (passed) { LOG_INFO("Visual test passed!"); }
		else { LOG_ERROR("Visual test failed!"); }

		GETCHAR_PAUSE;

		return passed && (returnCode == F_V2::renrederRetCodes::OK);
	}
}
