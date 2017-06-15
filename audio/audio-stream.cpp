#include "audio-stream.h"

#include <iostream>

using namespace std;

void PcmSound::al_buffer (ALuint b) const {
	ALenum format = AL_FORMAT_STEREO16;
	if (channels == 1) {
		format = AL_FORMAT_MONO16;
	}
	
	alBufferData (b, format, &buffer [0], buffer.size () * sizeof (int16_t), rate);
}

AudioStream::AudioStream (void * d, int (*df)(void *, int16_t *, int)) {
	dec = d;
	decoder_fill = df;
	channels = 2;
	
	alGenBuffers (2, buffers);
	alGenSources (1, &source);
	
	//cout << "Generated buffers: " << buffers [0] << ", " << buffers [1] << endl;
	
	const int size_multiplier = 1;
	partialBuffer.resize (channels * 11520 * size_multiplier);
	bufferFill = 0;
	
	lastFilled = buffers [1];
}

AudioStream::~AudioStream () {
	alDeleteBuffers (2, buffers);
	alDeleteSources (1, &source);
}

void AudioStream::update () {
	ALint buffersProcessed = 0;
	ALint buffersQueued = 0;
	
	alGetSourcei (source, AL_BUFFERS_PROCESSED, &buffersProcessed);
	alGetSourcei (source, AL_BUFFERS_QUEUED, &buffersQueued);
	
	ALuint backBuffer = AL_NONE;
	
	if (buffersProcessed >= 1) {
		vector <ALuint> unqueued;
		unqueued.resize (buffersProcessed);
		//cout << "Unqueued " << buffersProcessed << endl;
		alSourceUnqueueBuffers (source, buffersProcessed, unqueued.data ());
		backBuffer = unqueued [0];
	}
	else if (buffersQueued <= 1) {
		//cout << "lastFilled " << lastFilled << endl;
		// Used during startup
		if (lastFilled == buffers [0]) {
			backBuffer = buffers [1];
		}
		else if (lastFilled == buffers [1]) {
			backBuffer = buffers [0];
		}
	}
	else {
		return;
	}
	
	if (backBuffer != AL_NONE) {
		for (int i = 0; i < 4; i++) {
			fill ();
		}
		
		submit (backBuffer);
		//cout << "Submitted " << backBuffer << endl;
	}
	
	alGetSourcei (source, AL_BUFFERS_PROCESSED, &buffersProcessed);
	alGetSourcei (source, AL_BUFFERS_QUEUED, &buffersQueued);
	
	ALint state = AL_STOPPED;
	alGetSourcei (source, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING && buffersQueued > 0 ) {
		alSourcePlay (source);
		//cout << "Kicked the source" << endl;
	}
}

int AudioStream::fill () {
	if (bufferFill < (int)partialBuffer.size () / 2) {
		int num_samples = decoder_fill (dec, &partialBuffer [bufferFill], partialBuffer.size () - bufferFill);
		
		bufferFill += num_samples;
		return num_samples;
	}
	
	return 0;
}

void AudioStream::submit (ALuint i) {
	if (bufferFill > 0) {
		ALenum format = AL_FORMAT_STEREO16;
		if (channels == 1) {
			format = AL_FORMAT_MONO16;
		}
		
		alBufferData (i, format, partialBuffer.data (), bufferFill * 
		sizeof (int16_t), 44100); //opusFrequency);
		alSourceQueueBuffers (source, 1, &i);
		//cout << "Queued " << i << endl;
		bufferFill = 0;
		
		lastFilled = i;
	}
}
