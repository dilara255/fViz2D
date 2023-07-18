#pragma once

#include "GLFW/glfw3.h"

namespace IMG {

	static const char* resPathFromBinary = "../../../res/";
	static const char* bannerPathFromBinary = "../../../res/testBanner.jpg";

	typedef struct rgbaImage_st {
		unsigned char* data;
		int width, height;
	} rgbaImage_t;

	typedef struct rgbaTextureID_t {
		GLuint ID;
		bool initialized;
		int width, height;
	} rgbaTextureID_t;

	rgbaImage_t load4channel8bppImageFromFile(const char* filename);
	rgbaTextureID_t load4channelTextureFromFile(const char* filename);
}

namespace COLOR {
	typedef struct rgbaF_st {
		float r, g, b, a;
	} rgbaF_t;

	const rgbaF_t CLEAR = {0.263f, 0.141f, 0.384f, 1.00f};
}
