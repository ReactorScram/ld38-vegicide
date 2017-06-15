#include "audio.h"

#define AL_ALEXT_PROTOTYPES
#include "AL/alext.h"

#include <cstdlib>
#include <cstring>

//#include "ivorbisfile.h"

#include "terf/terf.h"

#include <iostream>

using namespace std;

Audio::Audio (const Terf::Archive & terf)
{
	ALCint attrList [] = {
		ALC_FREQUENCY,
		44100,
		ALC_FORMAT_CHANNELS_SOFT,
		ALC_STEREO_SOFT,
		ALC_FORMAT_TYPE_SOFT,
		ALC_SHORT_SOFT,
		0
	};
	
	/*
	Daily reminder that 90% of audio and graphics APIs are
	full of retarded global mutable state and spooky action-at-a-distance
	and OpenAL is no exception.
	
	They have no excuse for this, because audio is mostly done in software,
	especially for smaller games. OpenGL's only excuse is that the API is
	older than I am.
	*/
	device = alcOpenDevice (NULL);
	context = alcCreateContext (device, attrList);
	alcMakeContextCurrent (context);
	
	loadVorbises (terf);
	
	encoded_music [EMusic::Ambient] = shared_ptr <VorbisDecoder> (new VorbisDecoder (terf.lookupFile ("music/ambient.ogg")));
	encoded_music [EMusic::Boss] = shared_ptr <VorbisDecoder> (new VorbisDecoder (terf.lookupFile ("music/boss.ogg")));
	
	vorbis_decoder = encoded_music.at (EMusic::Ambient);
	bgm = EMusic::Ambient;
	vorbis_decoder->looping = true;
	vorbis_decoder->playing = true;
	
	alSourcePlay (vorbis_decoder->as.source);
}

Audio::~Audio () {
	sounds.clear ();
	
	alcMakeContextCurrent (nullptr);
	alcDestroyContext (context);
	alcCloseDevice (device);
}

void Audio::update (const AudioFrame & frame) {
	if (bgm != frame.bgm) {
		vorbis_decoder->playing = false;
		alSourceStop (vorbis_decoder->as.source);
		vorbis_decoder = encoded_music.at (frame.bgm);
		vorbis_decoder->looping = true;
		vorbis_decoder->playing = true;
	}
	
	bgm = frame.bgm;
	
	vorbis_decoder->as.update ();
	
	for (int i = 0; i < (int)ESound::NUM_SOUNDS; i++) {
		bool play = frame.sounds [i];
		
		if (play) {
			sound_sources [i] = unique_ptr <VorbisDecoder> (new VorbisDecoder (sounds [(ESound)i]));
			
			sound_sources [i]->playing = true;
			sound_sources [i]->looping = false;
		}
		
		sound_sources [i]->as.update ();
	}
}

void Audio::loadVorbises (const Terf::Archive & terf) {
	// Match this to ESound
	vector <string> soundNames {
		"Bling",
		"Crunch",
		"Gasp",
		"King-laugh",
		"King-pain",
		"King-roar",
		"King-you",
		"Respawn",
		"Swooce",
		"Whoa",
	};
	
	for (int i = 0; i < (int)soundNames.size (); i++) {
		string fn ("sounds/");
		fn.append (soundNames.at (i));
		fn.append (".ogg");
		
		sounds [(ESound)i] = terf.lookupFile (fn);
		
		sound_sources [i] = unique_ptr <VorbisDecoder> (new VorbisDecoder (sounds [(ESound)i]));
		sound_sources [i]->looping = false;
		sound_sources [i]->playing = false;
	}
}

ALuint Audio::loadVorbis (const Terf::Archive & terf, string name) {
	string fn ("sounds/");
	fn.append (name);
	fn.append (".ogg");
	
	auto vorbisBuffer = terf.lookupFile (fn);
	
	auto output = VorbisDecoder::decode_all (vorbisBuffer);
	
	ALuint b = 0;
	alGenBuffers (1, &b);
	
	ALenum format = AL_FORMAT_STEREO16;
	if (output.channels == 1) {
		format = AL_FORMAT_MONO16;
	}
	
	alBufferData (b, format, 
		output.buffer.data (), 
		output.buffer.size () * sizeof (int16_t), 
		output.rate);
	
	return b;
}


