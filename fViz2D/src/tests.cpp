#include "FV2_API.hpp"
#include "GUI_API.hpp"
#include "FV2_testsAPI.hpp"

#include "renderer/rendererControl.hpp"
#include "resourcePaths.hpp"
#include "utils/consoleLogging.hpp"
#include "timeHelpers.hpp"
#include "miscDefines.hpp"

#include "utils/guiUtils.hpp"

#include "prng.hpp"

#include <algorithm>

namespace F_V2 {

	bool linkAndLogTest(void) {
		LOG_INFO("HEY, THIS IS fViz2D!");
		return true;
	}

	//TODO: CRITICAL: As soon as rendering is made more general, review the next two tests

	std::string filenameFunc(int steps, bool calledFromGUI) {
		std::string calledFromGuiOrApi = "a";
		if(calledFromGUI) { calledFromGuiOrApi = "g"; }
		return "TEST3_" + calledFromGuiOrApi + "_" + std::to_string(steps);
	}















	bool rendererTestFromImage() {

		LOG_DEBUG("This is a visual test for fViz2D. It will hot-reload a texture from a dynamic image\n"); GETCHAR_PAUSE;

		bool passedVisualInspection = false;

		IMG::rgbaImage_t dynamicTestData = 
			IMG::load4channel8bpcImageFromFile(F_V2::noiseColor512by512pathFromBinary);
		IMG::rgbaImage_t tintedDynamicTestData = IMG::load4channel8bpcImageFromFile(F_V2::noiseColor512by512pathFromBinary);

		F_V2::rendererRetCode_st returnCode = F_V2::rendererRetCode_st::STILL_RUNNING;
		COLOR::rgbaF_t noiseTint = COLOR::FULL_WHITE;
		noiseTint.a = 0.6; //just to show a bit of the clear color by default
		COLOR::rgbaF_t clearColor = COLOR::CLEAR;

		IMG::generic2DfieldPtr_t tintedDynamicTestDataPtr;
		tintedDynamicTestDataPtr.storeRGBAfield(&tintedDynamicTestData);

		GUI::menuDefinition_t testMenu = GUI::getTestMenuDefinition(&passedVisualInspection, 
			                                                        &clearColor.r, &noiseTint.r);
		GUI::menuDefinitionList_t menuList;
		menuList.push_back(testMenu);

		std::thread testRendererThread = F_V2::spawnRendererOnNewThread(&tintedDynamicTestDataPtr, &returnCode,
			                                                            &clearColor, &menuList);	
			
		AZ::hybridBusySleepForMicros(std::chrono::microseconds(MICROS_IN_A_SECOND));
		F_V2::saveCurrentImage(1);

		//Change the dynamic image while the rendering isn't done:
		const int microsToSleepPerCycle = MICROS_IN_A_SECOND / 200;
		auto data_ptr = dynamicTestData.data.get();
		auto tintedData_ptr = tintedDynamicTestData.data.get();

		while (returnCode == F_V2::rendererRetCode_st::STILL_RUNNING) {
	
			const int totalPixels = dynamicTestData.size.width * dynamicTestData.size.height;
			int indexR, indexG, indexB;
			COLOR::rgbF_t clearContribution = { 255 * clearColor.r, 255 * clearColor.g, 255 * clearColor.b };

			for (int i = 0; i < totalPixels; i++) {
				
				indexR = i * dynamicTestData.size.channels;
				indexG = indexR + 1;
				indexB = indexR + 2;

				data_ptr[indexR] += 1;
				data_ptr[indexG] += 1;
				data_ptr[indexB] += 1;
		
				tintedData_ptr[indexR] = (char)( ( clearContribution.r * (1.0 - noiseTint.a) )
										       + ( data_ptr[indexR] * noiseTint.r * noiseTint.a) );
				tintedData_ptr[indexG] = (char)( ( clearContribution.g * (1.0 - noiseTint.a) )
										       + ( data_ptr[indexG] * noiseTint.g * noiseTint.a) );
				tintedData_ptr[indexB] = (char)( ( clearContribution.b * (1.0 - noiseTint.a) )
										       + ( data_ptr[indexB] * noiseTint.b * noiseTint.a) );
			}

			AZ::hybridBusySleepForMicros(std::chrono::microseconds(microsToSleepPerCycle));
		}

		testRendererThread.join(); //done rendering

		if (passedVisualInspection) { LOG_INFO("Visual test passed! (texture hot-reload from dynamic image)"); }
		else { LOG_ERROR("Visual test failed! (texture hot-reload from dynamic image)"); }

		LOG_DEBUG("Will try to save test images");

		F_V2::imageFileRetCode_st savesReturn;
		bool savedFine = true;

		savesReturn = IMG::saveImage(&tintedDynamicTestDataPtr, "imageSavingTest1_100", IMG::imageType::JPG, 100);
		if(savesReturn != F_V2::imageFileRetCode_st::OK) { savedFine &= false; }
		savesReturn = IMG::saveImage(&tintedDynamicTestDataPtr, "imageSavingTest1_10", IMG::imageType::JPG, 10);
		if(savesReturn != F_V2::imageFileRetCode_st::OK) { savedFine &= false; }
		savesReturn = IMG::saveImage(&tintedDynamicTestDataPtr, "imageSavingTest1", IMG::imageType::PNG);
		if(savesReturn != F_V2::imageFileRetCode_st::OK) { savedFine &= false; }

		if(savedFine) { LOG_INFO("Test images saved on the default folder!"); }
		else { LOG_ERROR("Error saving test images"); }

		GETCHAR_PAUSE;

		return passedVisualInspection && (returnCode == F_V2::rendererRetCode_st::OK) && savedFine;
	}








	







	#define TEST_WIDTH 512
	#define TEST_HEIGHT 512

	static bool g_testBool = false;
	static float g_testFloat[2] = {0.0f, 0.0f};
	void testMenuFunc2(F_V2::rendererControlPtrs_t* rendererControl_ptr) {
		ImGui::Text("This is another test menu!"); 
		ImGui::SameLine();
		ImGui::Checkbox("Is this button also working?", &g_testBool);
               
		ImGui::DragFloat("This is a way to change a float", &g_testFloat[0]);
		ImGui::DragFloat2("This is a way to change a float[2]", &g_testFloat[0]);
	}

	bool rendererTestFromDoubles2Dfield() {

		LOG_DEBUG("This is a visual test for fViz2D. It will hot-reload a texture from a field of doubles\n"); GETCHAR_PAUSE;

		bool passedVisualInspection = false;

		IMG::doubles2Dfield_st noiseInternal = IMG::createDoubles2Dfield(TEST_WIDTH, TEST_HEIGHT);
		IMG::floats2Dfield_st noiseIntermediate = IMG::createFloats2Dfield(TEST_WIDTH, TEST_HEIGHT);
		IMG::floats2Dfield_st noiseToRender = IMG::createFloats2Dfield(TEST_WIDTH, TEST_HEIGHT);
		if (!noiseInternal.size.initialized || !noiseToRender.size.initialized) {
			LOG_ERROR("Couldn't initialize fields\n"); GETCHAR_PAUSE;
			return false;
		}

		F_V2::rendererRetCode_st returnCode = F_V2::rendererRetCode_st::STILL_RUNNING;
		COLOR::rgbaF_t noiseTint = COLOR::FULL_WHITE;
		noiseTint.a = (float)100/255; //just to show a bit of the clear color by default
		COLOR::rgbaF_t clearColor = COLOR::FULL_WHITE;

		IMG::generic2DfieldPtr_t noiseDataPtr;
		noiseDataPtr.storeFloatsField(&noiseToRender);
		COLOR::colorInterpolation_t scheme;
		scheme.loadScheme(&COLOR::defaultBWscheme);

		GUI::menuDefinition_t testMenu = GUI::getTestMenuDefinition(&passedVisualInspection, 
			                                                        &clearColor.r, &noiseTint.r);
		GUI::menuDefinition_t testMenu2;
		testMenu2.menuFunc_ptr = testMenuFunc2;

		GUI::menuDefinitionList_t menuList;
		menuList.push_back(testMenu);
		menuList.push_back(testMenu2);

		std::thread testRendererThread = F_V2::spawnRendererOnNewThread(&noiseDataPtr, &returnCode,
			                                                &clearColor, &menuList, nullptr, &scheme,
															std::string("Visual Test without Color Interpolation"),
			                                                1024, 768, false);
			
		//TODO: prngg.hpp and then this : )
		std::vector<double> drawnPRNs;
		size_t elements = noiseInternal.size.getTotalElements();
		drawnPRNs.reserve(elements);
		uint64_t seeds[4] = { DEFAULT_PRNG_SEED0, DEFAULT_PRNG_SEED1, DEFAULT_PRNG_SEED2, DEFAULT_PRNG_SEED3};
		uint32_t drawnTmp[4];

		auto internalNoiseData_ptr = noiseInternal.data.get();
		for (size_t i = 0; i < elements; i++) {
			internalNoiseData_ptr[i] = (double)AZ::draw1spcg32(&seeds[0])/UINT32_MAX;
		}
		
		//Change the dynamic image while the rendering isn't done:
		const int microsToSleepPerCycle = MICROS_IN_A_SECOND / 200;
		while (returnCode == F_V2::rendererRetCode_st::STILL_RUNNING) {
	
			drawnPRNs.clear();

			for (size_t i = 0; i < elements/4; i++) {
				AZ::draw4spcg32s(&seeds[0], &seeds[1], &seeds[2], &seeds[3], 
								 &drawnTmp[0], &drawnTmp[1], &drawnTmp[2], &drawnTmp[3]);
				drawnPRNs.push_back((double)drawnTmp[0]/UINT32_MAX);
				drawnPRNs.push_back((double)drawnTmp[1]/UINT32_MAX);
				drawnPRNs.push_back((double)drawnTmp[2]/UINT32_MAX);
				drawnPRNs.push_back((double)drawnTmp[3]/UINT32_MAX);
			}
			for (size_t i = 0; i < elements%4; i++) {
				drawnPRNs.push_back((double)(AZ::draw1spcg32(&seeds[0])/UINT32_MAX));
			}

			for (size_t i = 0; i < elements; i++) {
				internalNoiseData_ptr[i] += (drawnPRNs[i] / 120.0);
				internalNoiseData_ptr[i] -= 1 * (internalNoiseData_ptr[i] > 1.0);
			}

			IMG::copy2Dfield(&noiseInternal, &noiseIntermediate);

			float effectiveNoiseTint = noiseTint.r/3 + noiseTint.g/3 + noiseTint.b/3;
			noiseTint.r = effectiveNoiseTint;
			noiseTint.g = effectiveNoiseTint;
			noiseTint.b = effectiveNoiseTint;

			float effectiveClearColor = clearColor.r/3 + clearColor.g/3 + clearColor.b/3;
			clearColor.r = effectiveClearColor;
			clearColor.g = effectiveClearColor;
			clearColor.b = effectiveClearColor;

			auto noiseToRenderData_ptr = noiseToRender.data.get();
			auto intermediateNoiseData_ptr = noiseIntermediate.data.get();
			for (size_t i = 0; i < elements; i++) {
				
				noiseToRenderData_ptr[i] = intermediateNoiseData_ptr[i] * effectiveNoiseTint * noiseTint.a;
				noiseToRenderData_ptr[i] += clearColor.r * (1.0 - noiseTint.a);
			}

			AZ::hybridBusySleepForMicros(std::chrono::microseconds(microsToSleepPerCycle));
		}

		testRendererThread.join(); //done rendering

		if (passedVisualInspection) { LOG_INFO("Visual test passed! (texture hot-reload from field of doubles)"); }
		else { LOG_ERROR("Visual test failed! (texture hot-reload from field of doubles)"); }

		LOG_DEBUG("Will try to save test images");

		F_V2::imageFileRetCode_st savesReturn;
		bool savedFine = true;

		savesReturn = IMG::saveImage(&noiseDataPtr, "imageSavingTest2_100", IMG::imageType::JPG, 100);
		if(savesReturn != F_V2::imageFileRetCode_st::OK) { savedFine &= false; }
		savesReturn = IMG::saveImage(&noiseDataPtr, "imageSavingTest2_10", IMG::imageType::JPG, 10);
		if(savesReturn != F_V2::imageFileRetCode_st::OK) { savedFine &= false; }
		savesReturn = IMG::saveImage(&noiseDataPtr, "imageSavingTest2", IMG::imageType::PNG);
		if(savesReturn != F_V2::imageFileRetCode_st::OK) { savedFine &= false; }

		if(savedFine) { LOG_INFO("Test images saved on the default folder!"); }
		else { LOG_ERROR("Error saving test images"); }

		GETCHAR_PAUSE;

		return passedVisualInspection && (returnCode == F_V2::rendererRetCode_st::OK);
	}














	bool rendererTestFromDoubles2DfieldWithColorInterp() {

		LOG_DEBUG("This is a visual test for fViz2D. It will hot-reload a texture from a field of doubles and show it with a color scheme\n"); GETCHAR_PAUSE;

		bool passedVisualInspection = false;

		IMG::doubles2Dfield_st noiseInternal = IMG::createDoubles2Dfield(TEST_WIDTH, TEST_HEIGHT);
		IMG::floats2Dfield_st noiseToRender = IMG::createFloats2Dfield(TEST_WIDTH, TEST_HEIGHT);
		if (!noiseInternal.size.initialized || !noiseToRender.size.initialized) {
			LOG_ERROR("Couldn't initialize fields\n"); GETCHAR_PAUSE;
			return false;
		}

		F_V2::rendererRetCode_st returnCode = F_V2::rendererRetCode_st::STILL_RUNNING;
		COLOR::rgbaF_t noiseTint = COLOR::FULL_WHITE;
		noiseTint.a = (float)100/255; //just to show a bit of the clear color by default
		COLOR::rgbaF_t clearColor = COLOR::FULL_WHITE;

		IMG::generic2DfieldPtr_t noiseDataPtr;
		noiseDataPtr.storeFloatsField(&noiseToRender);
		COLOR::colorInterpolation_t scheme;
		scheme.loadScheme(&COLOR::defaultBlueYellowRedScheme);
		scheme.normalizeSpan();
			
		GUI::menuDefinition_t testMenu = GUI::getTestMenuDefinition(&passedVisualInspection, 
			                                                        &clearColor.r, &noiseTint.r);
		GUI::menuDefinitionList_t menuList;
		menuList.push_back(testMenu);

		std::thread testRendererThread = F_V2::spawnRendererOnNewThread(&noiseDataPtr, &returnCode,
			                                                &clearColor, &menuList, filenameFunc, &scheme,
															std::string("Visual Test without Color Interpolation"));

		//TODO: prngg.hpp and then this : )
		std::vector<double> drawnPRNs;
		size_t elements = noiseInternal.size.getTotalElements();
		drawnPRNs.reserve(elements);
		uint64_t seed = DEFAULT_PRNG_SEED0;

		auto internalNoiseData_ptr = noiseInternal.data.get();
		for (size_t i = 0; i < elements; i++) {
			internalNoiseData_ptr[i] = (double)AZ::draw1spcg32(&seed)/UINT32_MAX;
		}
		
		//Change the dynamic image while the rendering isn't done:
		const int microsToSleepPerCycle = MICROS_IN_A_SECOND / 200;
		
		while (returnCode == F_V2::rendererRetCode_st::STILL_RUNNING) {
	
			for (size_t i = 0; i < elements; i++) {
				internalNoiseData_ptr[i] += ( ((double)AZ::draw1spcg32(&seed)/UINT32_MAX) / 120.0);
				internalNoiseData_ptr[i] -= 1 * (internalNoiseData_ptr[i] > 1.0);
			}

			float effectiveNoiseTint = noiseTint.r/3 + noiseTint.g/3 + noiseTint.b/3;
			noiseTint.r = effectiveNoiseTint;
			noiseTint.g = effectiveNoiseTint;
			noiseTint.b = effectiveNoiseTint;

			float effectiveClearColor = clearColor.r/3 + clearColor.g/3 + clearColor.b/3;
			clearColor.r = effectiveClearColor;
			clearColor.g = effectiveClearColor;
			clearColor.b = effectiveClearColor;

			for (size_t i = 0; i < elements; i++) {
				
				noiseToRender.data.get()[i] = internalNoiseData_ptr[i] * effectiveNoiseTint * noiseTint.a;
				noiseToRender.data.get()[i] += effectiveClearColor * (1.0 - noiseTint.a);
			}

			AZ::hybridBusySleepForMicros(std::chrono::microseconds(microsToSleepPerCycle));
		}

		testRendererThread.join(); //done rendering

		if (passedVisualInspection) { LOG_INFO("Visual test passed! (texture hot-reload from field of doubles with color scheme)"); }
		else { LOG_ERROR("Visual test failed! (texture hot-reload from field of doubles with color scheme)"); }

		F_V2::imageFileRetCode_st savesReturn;
		bool savedFine = true;

		savesReturn = IMG::saveImage(&noiseDataPtr, "imageSavingTest3_100", IMG::imageType::JPG, 100);
		if(savesReturn != F_V2::imageFileRetCode_st::OK) { savedFine &= false; }
		savesReturn = IMG::saveImage(&noiseDataPtr, "imageSavingTest3_10", IMG::imageType::JPG, 10);
		if(savesReturn != F_V2::imageFileRetCode_st::OK) { savedFine &= false; }
		savesReturn = IMG::saveImage(&noiseDataPtr, "imageSavingTest3", IMG::imageType::PNG);
		if(savesReturn != F_V2::imageFileRetCode_st::OK) { savedFine &= false; }

		if(savedFine) { LOG_INFO("Test images saved on the default folder!"); }
		else { LOG_ERROR("Error saving test images"); }

		GETCHAR_PAUSE;

		return passedVisualInspection && (returnCode == F_V2::rendererRetCode_st::OK);
	}
}
