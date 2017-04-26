#ifndef STBVORBISDECODER_H
#define STBVORBISDECODER_H

#include <memory>

#include "audio-stream.h"

struct COLORADO_DLLSPEC StbVorbisDecoder {
	static PcmSound decode_vorbis (const std::vector <uint8_t> & data);
};

#endif // STBVORBISDECODER_H
