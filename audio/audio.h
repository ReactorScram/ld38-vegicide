#ifndef IDK_AUDIO_H
#define IDK_AUDIO_H

#include <map>
#include <memory>
#include <vector>

#include "AL/al.h"
#include "AL/alc.h"

#include "colorado/dllspec.h"
#include "audio-frame.h"

/*
enum class EVoiceover {
	None = 0,
	BoosterDeploy,
	Checkpoint,
	LapCompleted,
	LapRecord,
};
*/

namespace Terf {
class Archive;
}

struct OggOpusFile;
struct OggVorbis_File;

struct PcmSound {
	std::vector <int16_t> buffer;
	int channels;
	int rate;
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

struct COLORADO_DLLSPEC OpusDecoder {
	OggOpusFile * dec;
	AudioStream as;
	
	OpusDecoder (const std::vector <uint8_t> & opusBuffer);
	~OpusDecoder ();
	
	// Implements generic decoder
	static int fill (void * dec, int16_t * buffer, int length);
};

struct COLORADO_DLLSPEC VorbisDecoder {
	std::unique_ptr <OggVorbis_File> dec;
	int decoder_error;
	AudioStream as;
	bool looping;
	bool playing;
	
	VorbisDecoder (const std::vector <uint8_t> & vorbisBuffer);
	void reset ();
	~VorbisDecoder ();
	
	int fill (int16_t * buffer, int length);
	
	// Implements generic decoder
	static int fill (void * dec, int16_t * buffer, int length);
	static PcmSound decode_all (const std::vector <uint8_t> & data);
	
	// Virtual file cursor for Vorbis
	const std::vector <uint8_t> & buffer;
	size_t cursor;
};

extern "C" {
	size_t vorb_read (void * ptr, size_t size, size_t nmemb, void * datasource);
	int vorb_seek (void * datasource, int64_t offset, int whence);
	int vorb_close (void * datasource);
	long vorb_tell (void * datasource);
}

struct COLORADO_DLLSPEC Audio {
	public:
	Audio (const Terf::Archive & terf);
	~Audio ();
	
	void update (const AudioFrame &);
	
protected:
	ALCdevice * device;
	ALCcontext * context;
	
	std::map <ESound, std::vector <uint8_t> > sounds;
	std::unique_ptr <VorbisDecoder> sound_sources [(int)ESound::NUM_SOUNDS];
	
	std::map <EMusic, std::vector <uint8_t> > encoded_music;
	std::unique_ptr <VorbisDecoder> vorbis_decoder;
	
	std::vector <uint8_t> music_opus;
	std::unique_ptr <OpusDecoder> opus_decoder;
	
	EMusic bgm;
	
	bool use_vorbis, use_opus;
	
	// I want these to be static
	// and they could be
	// but OpenAL shouldn't be global
	// so these technically shouldn't be static yet
	void loadVorbises (const Terf::Archive & terf);
	ALuint loadVorbis (const Terf::Archive & terf, std::string name);
	
	ALuint loadOpus (const Terf::Archive & terf, std::string name);
};

#endif
