#ifndef IDK_AUDIO_STREAMER_H
#define IDK_AUDIO_STREAMER_H

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

struct COLORADO_DLLSPEC Streamer {
	public:
	Streamer (const Terf::Archive & terf);
	~Streamer ();
	
	void update (EMusic newBgm);
	
protected:
	ALCdevice * device;
	ALCcontext * context;
	
	std::map <EMusic, std::shared_ptr <VorbisDecoder> > encoded_music;
	
	EMusic bgm;
};

#endif
