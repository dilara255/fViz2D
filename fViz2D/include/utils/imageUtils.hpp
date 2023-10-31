#pragma once

#include "GLFW/glfw3.h"
#include "stbImage/stb_image.h"

namespace IMG {

	typedef struct rgbaImage_st {
		unsigned char* data;
		int width, height;
		int channels = 4;

		int getIndex(uint32_t row, uint32_t collumn, uint8_t channel) {
			int stride = channels * width;
			return (row * stride) + (collumn * channels) + channel;
		}
		int maxIndex() { return ( (channels * width * height) - 1 ); }
		~rgbaImage_st() { stbi_image_free(data); }
	} rgbaImage_t;

	rgbaImage_t load4channel8bpcImageFromFile(const char* filename);
}

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
		char r, g, b, a;

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
}
