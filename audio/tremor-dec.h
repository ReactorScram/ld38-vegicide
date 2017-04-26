#ifndef TREMORDEC_H
#define TREMORDEC_H

#include <memory>

#include "audio-stream.h"

struct OggOpusFile;
struct OggVorbis_File;

extern "C" {
struct stb_vorbis;
}

struct StbWrapper {
	stb_vorbis * stb;
	
	StbWrapper (const std::vector <uint8_t> & b);
	~StbWrapper ();
	StbWrapper (const StbWrapper &) = delete;
	StbWrapper & operator = (const StbWrapper &) = delete;
	
	stb_vorbis * operator * () const;
};

struct COLORADO_DLLSPEC VorbisDecoder {
	// In order to ship Vegicide for Windows in a timely manner,
	// I am replacing Tremor with stb_vorbis
	std::unique_ptr <StbWrapper> dec;
	
	int decoder_error;
	AudioStream as;
	bool looping;
	bool playing;
	
	VorbisDecoder (const std::vector <uint8_t> & vorbisBuffer);
	void reset ();
	
	int fill (int16_t * buffer, int length);
	
	// Implements generic decoder
	static int fill (void * dec, int16_t * buffer, int length);
	static PcmSound decode_all (const std::vector <uint8_t> & data);
	
	// Virtual file cursor for Vorbis
	const std::vector <uint8_t> buffer;
	size_t cursor;
};

#endif // TREMORDEC_H
