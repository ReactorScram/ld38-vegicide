#include "colorado/audio-mixer.h"

#include <SDL/SDL.h>
#include <SDL_mixer.h>

namespace Colorado {
AudioChannel::AudioChannel () {
	chunk = NULL;
	enabled = false;
	offset = 0;
	repeat = false;
	speedRatio = 1.0f;
	volume = SDL_MIX_MAXVOLUME;
}

AudioMixer::AudioMixer () {
	
}

void AudioMixer::startRecording (std::string fn) {
	SDL_LockAudio ();
	audioRecording.open (fn.c_str ());
	SDL_UnlockAudio ();
}

void AudioMixer::stopRecording () {
	SDL_LockAudio ();
	audioRecording.close ();
	SDL_UnlockAudio ();
}

void AudioMixer::audioCallback (void * userData, Uint8 * stream, int len) {
	AudioMixer * that = (AudioMixer *)userData;
	
	memset (stream, 0, len);
	
	for (unsigned int i = 0; i < that->channels.size (); ++i) {
		AudioChannel & chan = that->channels [i];
		
		if (NULL != chan.chunk && chan.enabled) {
			Uint8 * doppleredSamples = new Uint8 [len];
			
			int numChunkSamples = chan.chunk->alen / 4;
			int numDestSamples = len / 4;
			for (int destIndex = 0; destIndex < numDestSamples; ++destIndex) {
				float srcOffsetF = chan.offset + (destIndex * chan.speedRatio);
				int srcOffset = (int)srcOffsetF % numChunkSamples;
				
				for (int i = 0; i < 4; ++i) {
					doppleredSamples [destIndex * 4 + i] = chan.chunk->abuf [srcOffset * 4 + i];
				}
			}
			
			chan.offset = (chan.offset + (int)(numDestSamples * chan.speedRatio)) % numChunkSamples;
			
			SDL_MixAudio (stream, doppleredSamples, len, chan.volume);
			delete [] doppleredSamples;
		}
	}
	
	if (that->audioRecording.is_open ()) {
		that->audioRecording.write ((const char *)stream, len);
	}
}
}
