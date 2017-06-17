#ifndef TREMORDEC_H
#define TREMORDEC_H

#include <memory>

#include "audio-stream.h"

struct OggOpusFile;
struct OggVorbis_File;

struct COLORADO_DLLSPEC VorbisDecoder {
	std::unique_ptr <OggVorbis_File> dec;
	int decoder_error;
	AudioStream as;
	bool looping;
	bool playing;
	
	VorbisDecoder (const std::vector <uint8_t> & vorbisBuffer);
	VorbisDecoder (const VorbisDecoder &) = delete;
	VorbisDecoder & operator = (const VorbisDecoder &) = delete;
	~VorbisDecoder ();
	void reset ();
	
	int fill (int16_t * buffer, int length);
	
	// Implements generic decoder
	// Return value is multiplied by channel count
	// i.e. actual samples decoded
	static int fill (void * dec, int16_t * buffer, int length);
	static PcmSound decode_all (const std::vector <uint8_t> & data);
	
	// I left these public cause otherwise 
	// I'd have to learn how friend functions work
	// Virtual file cursor for Vorbis
	const std::vector <uint8_t> buffer;
	size_t cursor;
};

#endif // TREMORDEC_H
