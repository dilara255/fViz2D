#include "FV2_testsAPI.hpp"

#include "renderer/rendererControl.hpp"
#include "resourcePaths.hpp"
#include "utils/consoleLogging.hpp"
#include "timeHelpers.hpp"
#include "miscDefines.hpp"

namespace F_V2 {

	bool linkAndLogTest(void) {
		LOG_INFO("HEY, THIS IS fViz2D!");
		return true;
	}

	bool rendererTest(void) {

		LOG_DEBUG("This is a visual test for fViz2D\n"); GETCHAR_PAUSE;

		bool passedVisualInspection = false;

		IMG::rgbaImage_t dynamicTestData = 
			IMG::load4channel8bppImageFromFile(F_V2::noiseColor512by512pathFromBinary);

		int returnCode = F_V2::renrederRetCodes::STILL_RUNNING;
		COLOR::rgbaF_t noiseTint = COLOR::FULL_WHITE;
		std::thread testRendererThread(F_V2::rendererMainForSeparateThread, &passedVisualInspection, 
			                                              &dynamicTestData, &noiseTint, &returnCode);
		
		//Change the dynamic image while the rendering isn't done:
		const int microsToSleepPerCycle = MICROS_IN_A_SECOND / 100;
		while (returnCode == F_V2::renrederRetCodes::STILL_RUNNING) {
	
			const int maxImageIndex = dynamicTestData.maxIndex();
			for (int i = 0; i < maxImageIndex; i++) {
				dynamicTestData.data[i] += 1;
			}

			AZ::hybridBusySleepForMicros(std::chrono::microseconds(microsToSleepPerCycle));
		}

		testRendererThread.join(); //done rendering

		if (passedVisualInspection) { LOG_INFO("Visual test passed!"); }
		else { LOG_ERROR("Visual test failed!"); }

		GETCHAR_PAUSE;

		return passedVisualInspection && (returnCode == F_V2::renrederRetCodes::OK);
	}
}
