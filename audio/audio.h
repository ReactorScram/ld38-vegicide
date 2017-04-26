#ifndef IDK_AUDIO_H
#define IDK_AUDIO_H

#include <map>
#include <memory>
#include <vector>

#include "AL/alc.h"

#include "audio-frame.h"
#include "audio-stream.h"
#include "tremor-dec.h"

namespace Terf {
class Archive;
}

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
	std::shared_ptr <VorbisDecoder> sound_sources [(int)ESound::NUM_SOUNDS];
	
	std::map <EMusic, std::shared_ptr <VorbisDecoder> > encoded_music;
	std::shared_ptr <VorbisDecoder> vorbis_decoder;
	
	EMusic bgm;
	
	// I want these to be static
	// and they could be
	// but OpenAL shouldn't be global
	// so these technically shouldn't be static yet
	void loadVorbises (const Terf::Archive & terf);
	ALuint loadVorbis (const Terf::Archive & terf, std::string name);
};

#endif
