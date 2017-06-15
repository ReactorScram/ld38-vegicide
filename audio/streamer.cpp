#include "streamer.h"

#define AL_ALEXT_PROTOTYPES
#include "AL/alext.h"

#include <cstdlib>
#include <cstring>

//#include "ivorbisfile.h"

#include "terf/terf.h"

#include <iostream>

using namespace std;

Streamer::Streamer (const Terf::Archive & terf)
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
	
	encoded_music [EMusic::Ambient] = shared_ptr <VorbisDecoder> (new VorbisDecoder (terf.lookupFile ("music/ambient.ogg")));
	encoded_music [EMusic::Boss] = shared_ptr <VorbisDecoder> (new VorbisDecoder (terf.lookupFile ("music/boss.ogg")));
	
	bgm = EMusic::NoMusic;
	
	cout << "Streamer constructed" << endl;
}

Streamer::~Streamer () {
	alcMakeContextCurrent (nullptr);
	alcDestroyContext (context);
	alcCloseDevice (device);
}

void Streamer::update (EMusic newBgm) {
	if (bgm != newBgm) {
		if (bgm != EMusic::NoMusic) {
			cout << "Stopped playing to change music" << endl;
			encoded_music [bgm]->playing = false;
			alSourceStop (encoded_music [bgm]->as.source);
		}
		
		bgm = newBgm;
		
		if (bgm != EMusic::NoMusic) {
			cout << "Started playing new music" << endl;
			encoded_music [bgm]->looping = true;
			encoded_music [bgm]->playing = true;
			alSourcePlay (encoded_music [bgm]->as.source);
		}
	}
	
	if (bgm != EMusic::NoMusic) {
		encoded_music [bgm]->as.update ();
	}
}
