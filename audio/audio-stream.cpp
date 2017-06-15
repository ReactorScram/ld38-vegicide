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
	
	const int size_multiplier = 4;
	partialBuffer.resize (channels * 11520 * size_multiplier);
	bufferFill = 0;
	lastFilled = buffers [1];
}

AudioStream::~AudioStream () {
	alDeleteBuffers (2, buffers);
	alDeleteSources (1, &source);
}

void AudioStream::update () {
	ALuint backBuffer = buffers [0];
	
	ALint currentBuffer = 0;
	alGetSourcei (source, AL_BUFFER, &currentBuffer);
	
	if (currentBuffer == AL_NONE) {
		// No buffers queued yet - Probably just constructed
	}
	else if (currentBuffer == (ALint)buffers [0]) {
		// On buffer 0  - fill 1 instead
		backBuffer = buffers [1];
	}
	else if (currentBuffer == (ALint)buffers [1]) {
		// On buffer 1 - Fill 0
	}
	else {
		// uh-oh
	}
	
	ALint buffersProcessed = 0;
	ALint buffersQueued = 0;
	
	alGetSourcei (source, AL_BUFFERS_PROCESSED, &buffersProcessed);
	alGetSourcei (source, AL_BUFFERS_QUEUED, &buffersQueued);

	//bool shouldWrite = buffersQueued - buffersProcessed <= 1;
	/*
	if (! shouldWrite) {
		return;
	}
	*/
	//cout << "Queued: " << buffersQueued << ", Processed: " << buffersProcessed << ", Current: " << currentBuffer << endl;
	
	if (buffersProcessed >= 1) {
		vector <ALuint> unused;
		unused.resize (buffersProcessed);
		//cout << "Unqueued " << buffersProcessed << endl;
		alSourceUnqueueBuffers (source, buffersProcessed, unused.data ());
	}
	
	alGetSourcei (source, AL_BUFFERS_PROCESSED, &buffersProcessed);
	alGetSourcei (source, AL_BUFFERS_QUEUED, &buffersQueued);
	
	for (int i = 0; i < 4; i++) {
		fill ();
		//cout << "Filled " << bufferFill << endl;
	}
	
	if (buffersQueued <= 1) {
		submit (backBuffer);
		//cout << "Submitted" << endl;
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
	}
	
	lastFilled = i;
}
