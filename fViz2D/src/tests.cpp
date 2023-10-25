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
		IMG::rgbaImage_t tintedDynamicTestData = 
			IMG::load4channel8bppImageFromFile(F_V2::noiseColor512by512pathFromBinary);

		int returnCode = F_V2::renrederRetCodes::STILL_RUNNING;
		COLOR::rgbaF_t noiseTint = COLOR::FULL_WHITE;
		noiseTint.a = 0.6; //just to show a bit of the clear color by default
		COLOR::rgbaF_t clearColor = COLOR::CLEAR;
		std::thread testRendererThread(F_V2::rendererMainForSeparateThread, &passedVisualInspection, 
			                           &tintedDynamicTestData, &clearColor, &noiseTint, &returnCode);
		
		//Change the dynamic image while the rendering isn't done:
		const int microsToSleepPerCycle = MICROS_IN_A_SECOND / 200;
		while (returnCode == F_V2::renrederRetCodes::STILL_RUNNING) {
	
			const int totalPixels = dynamicTestData.width * dynamicTestData.height;
			int indexR, indexG, indexB;
			COLOR::rgbF_t clearContribution = { 255 * clearColor.r, 255 * clearColor.g, 255 * clearColor.b };

			for (int i = 0; i < totalPixels; i++) {
				
				indexR = i * dynamicTestData.channels;
				indexG = indexR + 1;
				indexB = indexR + 2;

				dynamicTestData.data[indexR] += 1;
				dynamicTestData.data[indexG] += 1;
				dynamicTestData.data[indexB] += 1;
		
				tintedDynamicTestData.data[indexR] = (char)( ( clearContribution.r * (1.0 - noiseTint.a) )
										     + ( dynamicTestData.data[indexR] * noiseTint.r * noiseTint.a) );
				tintedDynamicTestData.data[indexG] = (char)( ( clearContribution.g * (1.0 - noiseTint.a) )
										     + ( dynamicTestData.data[indexG] * noiseTint.g * noiseTint.a) );
				tintedDynamicTestData.data[indexB] = (char)( ( clearContribution.b * (1.0 - noiseTint.a) )
										     + ( dynamicTestData.data[indexB] * noiseTint.b * noiseTint.a) );
			}

			AZ::hybridBusySleepForMicros(std::chrono::microseconds(0));
		}

		testRendererThread.join(); //done rendering

		if (passedVisualInspection) { LOG_INFO("Visual test passed!"); }
		else { LOG_ERROR("Visual test failed!"); }

		GETCHAR_PAUSE;

		return passedVisualInspection && (returnCode == F_V2::renrederRetCodes::OK);
	}
}
