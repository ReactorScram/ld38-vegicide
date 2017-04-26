#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include "stdint.h"
#include <vector>

#include "AL/al.h"

#include "colorado/dllspec.h"

struct PcmSound {
	std::vector <int16_t> buffer;
	int channels;
	int rate;
	
	void al_buffer (ALuint b) const;
};

struct COLORADO_DLLSPEC AudioStream {
	// Generic decoder interface, based on the opusfile API
	// Pass an opaque, an int16 buffer, and the buffer length,
	// and receive the number of samples decoded
	void * dec;
	int (*decoder_fill) (void *, int16_t *, int);
	
	ALuint buffers [2];
	ALuint source;
	
	std::vector <int16_t> partialBuffer;
	int bufferFill;
	
	int channels;
	int opusFrequency;
	
	ALuint lastFilled;
	
	// TODO: Ought to be a shared_ptr here I think
	AudioStream (void * d, int (*df) (void *, int16_t *, int));
	AudioStream (AudioStream &&) = default;
	~AudioStream ();
	
	// Checks on the buffer status, fills if necessary and submits if necessary
	void update ();
	
	// Pulls samples from dec into partialBuffer and increments bufferFill
	int fill ();
	
	// Loads samples from partialBuffer into OpenAL backbuffer
	void submit (ALuint i);
};

#endif // AUDIOSTREAM_H
