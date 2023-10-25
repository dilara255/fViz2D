#pragma once

#include "GLFW/glfw3.h"
#include "stbImage/stb_image.h"

namespace IMG {

	static const char* bannerPathFromBinary = "../../../res/testBanner.jpg";

	typedef struct rgbaImage_st {
		unsigned char* data;
		int width, height;

		~rgbaImage_st() { stbi_image_free(data); }
	} rgbaImage_t;

	typedef struct rgbaTextureID_t {
		GLuint ID = 0;
		bool initialized = false;
		int width, height;

		void createOGLtexID(GLenum minFilter = GL_LINEAR, GLenum magFilter = GL_LINEAR);
		~rgbaTextureID_t() { glDeleteTextures(1, &ID); }
	} rgbaTextureID_t;

	rgbaImage_t load4channel8bppImageFromFile(const char* filename);
	void load4channelTextureFromRgbaImage(rgbaImage_t* image_ptr, IMG::rgbaTextureID_t* texture_ptr);
	rgbaTextureID_t load4channelTextureFromFile(const char* filename);	
}

namespace COLOR {
	typedef struct rgbaF_st {
		float r, g, b, a;
	} rgbaF_t;

	const rgbaF_t CLEAR = {0.263f, 0.141f, 0.384f, 1.00f};
}
