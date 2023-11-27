#pragma once

//TODO: review: some stuff here probably could be made more general to support more types (template?)
//TODO-CRITICAL: write tests for this stuff

#include "GLFW/glfw3.h"
#include "stbImage/stb_image.h"

#include "returnCodes.hpp"

#include <assert.h>
#include <memory>
#include <vector>

namespace COLOR {
	
	typedef enum {R, G, B, A} channel_t;

	typedef struct rgbF_st {
		float r, g, b;

		float getChannel(channel_t channel) {
			switch (channel) {
				case channel_t::R: return r;
				case channel_t::G: return g;
				case channel_t::B: return b;
				case channel_t::A: return (r *g * b); //TODO: this is ugly
			}
		}
	} rgbF_t;

	typedef struct rgbaF_st {
		float r, g, b, a;

		float getChannel(channel_t channel) {
			switch (channel) {
				case channel_t::R: return r;
				case channel_t::G: return g;
				case channel_t::B: return b;
				case channel_t::A: return a;
			}
		}
	} rgbaF_t;

	typedef struct rgbaC_st {
		unsigned char r, g, b, a;

		char getChannel(channel_t channel) {
			switch (channel) {
				case channel_t::R: return r;
				case channel_t::G: return g;
				case channel_t::B: return b;
				case channel_t::A: return a;
			}
		}
	} rgbaC_t;

	//TODO: a reasonable way to have defaults for both formats
	const rgbaF_t CLEAR = {0.263f, 0.141f, 0.384f, 1.0f};
	const rgbaF_t FULL_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
	const rgbaF_t FULL_BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
	const rgbaF_t DEBUG_PINK = {1.0f, (1.0f/3), (13.0f/15), 1.0f};
	const rgbaC_t CLEAR_8B = {67, 36, 98, 255};
	const rgbaC_t FULL_WHITE_8B = {255, 255, 255, 255};
	const rgbaC_t FULL_BLACK_8B = {0, 0, 0, 255};
	const rgbaC_t FULL_BLUE_8B = {0, 0, 255, 255};
	const rgbaC_t DARK_BLUE_8B = {0, 0, 127, 255};
	const rgbaC_t FULL_YELLOW_8B = {255, 255, 0, 255};
	const rgbaC_t FULL_GREEN_8B = {0, 255, 0, 255};
	const rgbaC_t FULL_RED_8B = {255, 0, 0, 255};
	const rgbaC_t DARK_RED_8B = {127, 0, 0, 255};
	const rgbaC_t DEBUG_PINK_8B = {255, 85, 221, 255};

	typedef struct value8bitColorPair_st {
		double value;
		COLOR::rgbaC_t color;
	} value8bitColorPair_t;

	typedef std::vector<value8bitColorPair_t> schemeVector_t;
	
	static const schemeVector_t defaultBWscheme = { value8bitColorPair_t{ 0.0, FULL_BLACK_8B }, 
		                                            value8bitColorPair_t{ 1.0, FULL_WHITE_8B } };
	static const schemeVector_t defaultBlueRedScheme = { value8bitColorPair_t{-0.5, FULL_BLUE_8B }, 
		                                                 value8bitColorPair_t{ 1.5, FULL_RED_8B } };
	static const schemeVector_t defaultBlueYellowRedScheme = { value8bitColorPair_t{-0.5, FULL_BLACK_8B }, 
															   value8bitColorPair_t{ 0.0, FULL_BLUE_8B },  
		                                                       value8bitColorPair_t{ 0.5, FULL_YELLOW_8B }, 
															   value8bitColorPair_t{ 1.0, FULL_RED_8B }, 
		                                                       value8bitColorPair_t{ 1.5, FULL_WHITE_8B } };

	//Insertions guarantees that values are in increasing order
	typedef struct colorInterpolation_st {
		private:
			schemeVector_t correspondences;
		
	public:
			const schemeVector_t* const correspondences_ptr = &correspondences;

			bool addCorrespondence(double newValue, COLOR::rgbaC_t newColor) {  
				if(correspondences.size() == 0 || correspondences.back().value < newValue) { 
					correspondences.push_back({newValue, newColor});
					return true; 
				}
				else { return false; }
			}

			void loadScheme(const schemeVector_t* schemeToLoad_ptr) { 
				correspondences.clear();
				correspondences = *schemeToLoad_ptr;
			}

	} colorInterpolation_t;
		
	rgbaC_t interpolateTwoColors(double t, const rgbaC_t* colorBefore_ptr, const rgbaC_t* colorAfter_ptr);

	//Returns DEBUG_PINK_8B if an empty scheme is sent
	rgbaC_t interpolatedColorFromValue(double value, const  colorInterpolation_t* scheme_ptr);
}

namespace IMG {

	typedef struct imgSizeInfo_st {
		size_t width = 0, height = 0;
		size_t channels;
		size_t bytesPerChannel;
		bool initialized = false;

		size_t getStride() { return channels * width; }
		//Will return a bad index if bad parameters are passed on release
		size_t getIndex(uint32_t row, uint32_t collumn, uint8_t channel) {
			assert( (row < height) && (collumn < width) && (channel < channels) );
			return (row * getStride()) + (collumn * channels) + channel;
		}
		//Will return a bad index if bad parameters are passed on release
		size_t getLinearIndexOfChannel(size_t element, size_t channel) {
			assert( (element < height * width) && (channel < channels) );
			return ( (channels * element) + channel);
		}
		size_t getMaxIndex() { return ( (channels * width * height) - 1 ); }
		size_t getTotalElements() { return channels * width * height; }
		size_t getTotalArea() { return width * height; }
	} imgSizeInfo_t;

	typedef struct rgbaImage_st {
		std::unique_ptr<unsigned char> data = NULL;
		imgSizeInfo_t size = {0, 0, 4, 1};
	} rgbaImage_t;

	// Loads rgbaImage_t from a file. The data buffer becomes a responsability of the caller.
	rgbaImage_t load4channel8bpcImageFromFile(const char* filename);
	//In case alocation fails, the field's "initialized" member will be false and width/height will be zero
	rgbaImage_t createEmpty4channel8bpcImage(size_t width, size_t height);

	typedef struct floats2Dfield_st {
		std::unique_ptr<float> data = NULL;
		imgSizeInfo_t size = {0, 0, 1, 4};
	} floats2Dfield_t;

	typedef struct doubles2Dfield_st {
		std::unique_ptr<double> data = NULL;
		imgSizeInfo_t size = {0, 0, 1, 8};
	} doubles2Dfield_t;

	//TODO: using union to represent general data type. Possibly should turn these into classes
	enum class kinds2Ddata { UNINITIALIZED_UNION, RGBA_IMAGE, FLOATS_FIELD, DOUBLES_FIELD };
	union generic2DfieldPtrs_u { 
		rgbaImage_t* rgbaField_ptr = NULL; floats2Dfield_t* floatsField_ptr; doubles2Dfield_t* doublesField_ptr; 
	};

	typedef struct generic2DfieldPtr_st {

		void storeRGBAfield(rgbaImage_t* rgbaField_ptr) { 
			field_ptr.rgbaField_ptr = rgbaField_ptr; 
			kindOfField = kinds2Ddata::RGBA_IMAGE; 
		}

		void storeFloatsField(floats2Dfield_t* floatsField_ptr) { 
			field_ptr.floatsField_ptr = floatsField_ptr; 
			kindOfField = kinds2Ddata::FLOATS_FIELD; 
		}

		void storeDoublesField(doubles2Dfield_t* doublesField_ptr) { 
			field_ptr.doublesField_ptr = doublesField_ptr; 
			kindOfField = kinds2Ddata::DOUBLES_FIELD; 
		}

		generic2DfieldPtrs_u getFieldPtr() { return field_ptr; }
		kinds2Ddata getKindOfField() { return kindOfField; }

		private:
			kinds2Ddata kindOfField = kinds2Ddata::UNINITIALIZED_UNION;
			generic2DfieldPtrs_u field_ptr;

	} generic2DfieldPtr_t;
	

	//In case alocation fails, the field's "initialized" member will be false and width/height will be zero
	doubles2Dfield_t createDoubles2Dfield(size_t width, size_t height);
	floats2Dfield_t createFloats2Dfield(size_t width, size_t height);

	//Expects origin and destination to have the same amount of elements
	F_V2::texRetCode_st copy2Dfield(const floats2Dfield_t* floatOrigin_ptr, doubles2Dfield_t* doubleDest_ptr);
	F_V2::texRetCode_st copy2Dfield(const doubles2Dfield_t* doubleOrigin_ptr, floats2Dfield_t* floatDest_ptr);

	//Expects values and image fields to have the same amount of "pixels" (same width * height)
	//Expects the scheme to have at least two points
	F_V2::texRetCode_st translateValuesToInterpolatedColors(const floats2Dfield_t* valuesField_ptr, 
									                        rgbaImage_t* imageField_ptr, 
															const COLOR::colorInterpolation_t* scheme_ptr);
	F_V2::texRetCode_st translateValuesToInterpolatedColors(const doubles2Dfield_t* valuesField_ptr, 
															rgbaImage_t* imageField_ptr, 
															const COLOR::colorInterpolation_t* scheme_ptr);
}
