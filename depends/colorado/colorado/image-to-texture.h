#ifndef COLORADO_IMAGETOTEXTURE_H
#define COLORADO_IMAGETOTEXTURE_H

#include <stdint.h>
#include <vector>

#include "colorado/dllspec.h"
#include "colorado/gl.h"

namespace Colorado {
	enum ImageLoadError {
		ERROR_OK,			// Everything looks fine
		ERROR_CORRUPT_FILE,
		ERROR_EMPTY_FILE,	// File is empty or does not exist
		ERROR_SOIL			// Error while uploading image through SOIL
	};
	
	COLORADO_DLLSPEC ImageLoadError autoImageToTexture (const std::vector <uint8_t> &, GLuint &, int force_channels = 3);
	
	COLORADO_DLLSPEC ImageLoadError compressedImageToTexture (const std::vector <uint8_t> &, GLuint &);
	COLORADO_DLLSPEC ImageLoadError imageToTexture (const std::vector <uint8_t> &, GLuint &, int force_channels = 3);
	
	COLORADO_DLLSPEC ImageLoadError compressedImageToTexture (const char *, int, GLuint &);
	COLORADO_DLLSPEC ImageLoadError imageToTexture (const char *, int, GLuint &, int force_channels = 3);
}

#endif 
