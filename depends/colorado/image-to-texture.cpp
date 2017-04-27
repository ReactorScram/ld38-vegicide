#include "colorado/image-to-texture.h"

#include <iostream>
using std::cerr;
using std::endl;

#include <stdint.h>

#include "SOIL.h"

static const uint32_t GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG = 0x8C00;
static const uint32_t GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG = 0x8C01;
static const uint32_t GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG = 0x8C02;
static const uint32_t GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG = 0x8C03;

static const uint32_t PVR_MAGIC = 0x34;

/*
// Got this from AstroMenace and it doesn't appear to correspond to 
// what the PVR tool is putting out, at least for me
#pragma pack(push,4)
struct PVRTextureHeaderV3{
	uint32_t u32Version;	//Version of the file header, used to identify it.
	uint32_t u32Flags;	//Various format flags.
	uint64_t u64PixelFormat;	//The pixel format, 8cc value storing the 4 channel identifiers and their respective sizes.
	uint32_t u32ColourSpace;	//The Colour Space of the texture, currently either linear RGB or sRGB.
	uint32_t u32ChannelType;	//Variable type that the channel is stored in. Supports signed/unsigned int/short/byte or float for now.
	uint32_t u32Height;	//Height of the texture.
	uint32_t u32Width;	//Width of the texture.
	uint32_t u32Depth;	//Depth of the texture. (Z-slices)
	uint32_t u32NumSurfaces;	//Number of members in a Texture Array.
	uint32_t u32NumFaces;	//Number of faces in a Cube Map. Maybe be a value other than 6.
	uint32_t u32MIPMapCount;	//Number of MIP Maps in the texture - NB: Includes top level.
	uint32_t u32MetaDataSize;	//Size of the accompanying meta data.
};
#pragma pack(pop)
*/

// The size of the AstroMenace header seems to be right, but some fields are re-arranged
// and there appears to be a dataSize field as well.
struct PvrHeader {
	// Always 0x34
	uint32_t magicNumber;
	// TODO: These might be swapped, I don't know because all my textures are square
	uint32_t width;
	uint32_t height;
	uint32_t mipMapCount;
	uint32_t unused1;
	uint32_t dataSize;
	// I'm not sure about this one because it seems odd they would use 32 bits to describe this
	uint32_t bpp;
	uint32_t unused3;
	uint32_t unused4;
	uint32_t unused5;
	uint32_t unused6;
	// Always 0x21525650?
	uint32_t unused7;
	// Always 0x1?
	uint32_t unused8;
};

namespace Colorado {

ImageLoadError autoImageToTexture (const std::vector<uint8_t> & buffer, GLuint & out, int force_channels) {
	PvrHeader * compressedHeader = (PvrHeader *)&(buffer [0]);
	if (compressedHeader->magicNumber == PVR_MAGIC) {
		return compressedImageToTexture (buffer, out);
	}
	else {
		return imageToTexture (buffer, out, force_channels);
	}
}

ImageLoadError compressedImageToTexture (const std::vector<uint8_t> & buffer, GLuint & out) {
	return compressedImageToTexture ((const char *)buffer.data (), buffer.size (), out);
}

ImageLoadError imageToTexture (const std::vector<uint8_t> & buffer, GLuint & out, int force_channels) {
	return imageToTexture ((const char *)buffer.data (), buffer.size (), out, force_channels);
}

ImageLoadError compressedImageToTexture (const char * buffer, int bufferSize, GLuint & out) {
	glGenTextures (1, &out);
	glBindTexture (GL_TEXTURE_2D, out);
	
	PvrHeader * header = (PvrHeader *)buffer;
	
	int width = header->width;
	int height = header->height;
	
	if (width > 2048 || height > 2048) {
		return ERROR_CORRUPT_FILE;
	}
	
	const char * textureData = &buffer [sizeof (PvrHeader)];
	
	int compressedFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
	switch (header->bpp) {
		case 2:
			compressedFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
			break;
		case 4:
			compressedFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
			break;
	}
	
	if (bufferSize <= 0) {
		out = 0;
		return ERROR_EMPTY_FILE;
	}
	else if (bufferSize < (int)sizeof (PvrHeader)) {
		return ERROR_CORRUPT_FILE;
	}
	else {
		for (uint32_t mipIndex = 0; mipIndex <= header->mipMapCount; ++mipIndex) {
			int mipLength = width * height * header->bpp / 8;
			if (mipLength < 32) {
				mipLength = 32;
			}
			
			glCompressedTexImage2D (GL_TEXTURE_2D, mipIndex, compressedFormat, 
				width, height, 0, mipLength, textureData);
			
			width >>= 1;
			height >>= 1;
			textureData += mipLength;
		}
		
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		return ERROR_OK;
	}
}

ImageLoadError imageToTexture (const char * buffer, int bufferSize, GLuint & out, int force_channels) {
	if (bufferSize <= 0) {
		out = 0;
		return ERROR_EMPTY_FILE;
	}
	else {
		out = SOIL_load_OGL_texture_from_memory ((const unsigned char *)buffer, bufferSize,
			force_channels, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
		
		if (0 == out) {
			return ERROR_SOIL;
		}
		else {
			return ERROR_OK;
		}
	}
}

}
