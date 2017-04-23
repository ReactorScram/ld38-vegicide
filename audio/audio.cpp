#include "audio.h"

#define AL_ALEXT_PROTOTYPES
#include "AL/alext.h"

#include <cstdlib>
#include <cstring>

#include "opusfile.h"
#include "ivorbisfile.h"

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
	
	encoded_music [EMusic::Ambient] = terf.lookupFile ("music/ambient.ogg");
	encoded_music [EMusic::Boss] = terf.lookupFile ("music/boss.ogg");
	music_opus = terf.lookupFile ("music/boss.opus");
	
	vorbis_decoder = unique_ptr <VorbisDecoder> (new VorbisDecoder (encoded_music.at (EMusic::Ambient)));
	opus_decoder = unique_ptr <OpusDecoder> (new OpusDecoder (music_opus));
	
	use_vorbis = true;
	use_opus = false;
	
	if (use_vorbis) {
		alSourcePlay (vorbis_decoder->as.source);
		cerr << "Vorbis" << endl;
	}
	else if (use_opus) {
		alSourcePlay (opus_decoder->as.source);
		cerr << "Opus" << endl;
	}
}

Audio::~Audio () {
	for (auto pair : sounds) {
		ALuint bufferName = pair.second;
		alDeleteBuffers (1, &bufferName);
	}
	
	sounds.clear ();
	
	alcMakeContextCurrent (nullptr);
	alcDestroyContext (context);
	alcCloseDevice (device);
}

void Audio::update (const AudioFrame & frame) {
	//alSourcef (throttleSource, AL_PITCH, 1.0f + 0.25f * frame.throttle);
	/*
	if (! enableBgm && frame.enableBgm) {
		alSourcePlay (streamSource->source);
	}
	else if (enableBgm && !frame.enableBgm) {
		alSourceStop (streamSource->source);
	}
	*/
	bgm = frame.bgm;
	/*
	if (sounds.find (frame.voiceTrigger) != sounds.end ()) {
		alSourceStop (voiceSource);
		//alSourcei (voiceSource, AL_BUFFER, sounds [frame.voiceTrigger]);
		//alSourcePlay (voiceSource);
	}
	*/
	if (use_vorbis) {
		vorbis_decoder->as.update ();
	}
	if (use_opus) {
		//opus_decoder->as.update ();
	}
}

void Audio::loadVorbises (const Terf::Archive & terf) {
	// Match this to ESound
	vector <string> soundNames {
		"king-laugh",
		"king-pain",
		"king-roar",
		"king-you",
	};
	
	for (int i = 0; i < (int)soundNames.size (); i++) {
		sounds [(ESound)i] = loadVorbis (terf, soundNames.at (i));
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

ALuint Audio::loadOpus (const Terf::Archive & terf, string name) {
	string fn ("sounds/");
	fn.append (name);
	fn.append (".opus");
	
	auto opusBuffer = terf.lookupFile (fn);
	int channels = 2;
	int opusFrequency = 48000;
	
	vector <opus_int16> pcm_data;
	{
		int error = 0;
		OggOpusFile * dec;
		
		dec = op_open_memory (opusBuffer.data (), opusBuffer.size (), &error);
		
		auto pcm_length = channels * op_pcm_total (dec, -1);
		
		pcm_data.resize (pcm_length);
		
		int i = 0;
		int num_samples = 0;
		do {
			num_samples = op_read_stereo (dec, &pcm_data [i], pcm_length - i);
			//cout << "Samples: " << num_samples << endl;
			i += channels * num_samples;
		} while (num_samples > 0);
		
		op_free (dec);
	}
	
	ALuint b = 0;
	alGenBuffers (1, &b);
	
	ALenum format = AL_FORMAT_STEREO16;
	if (channels == 1) {
		format = AL_FORMAT_MONO16;
	}
	
	alBufferData (b, format, pcm_data.data (), pcm_data.size () * sizeof (opus_int16), opusFrequency);
	
	return b;
}

OpusDecoder::OpusDecoder (const std::vector <uint8_t> & opusBuffer):
	as (this, OpusDecoder::fill)
{
	int error = 0;
	dec = op_open_memory (opusBuffer.data (), opusBuffer.size (), &error);
}

OpusDecoder::~OpusDecoder () {
	op_free (dec);
}

int OpusDecoder::fill (void *opaque, int16_t *buffer, int length) {
	OpusDecoder * dec = (OpusDecoder *)opaque;
	int num_samples = op_read_stereo (dec->dec, buffer, length);
	if (num_samples == 0) {
		// TODO: My feeling is, this is a bug
		// The number of samples could be 0 for any reason, right?
		op_raw_seek (dec->dec, 0);
		num_samples = op_read_stereo (dec->dec, buffer, length);
	}
	
	const int channels = 2;
	return num_samples * channels;
}

VorbisDecoder::VorbisDecoder (const std::vector<uint8_t> &vorbisBuffer): 
	as (this, VorbisDecoder::fill), buffer (vorbisBuffer)
{
	reset ();
}

void VorbisDecoder::reset () {
	dec = unique_ptr <OggVorbis_File> (new OggVorbis_File ());
	
	ov_callbacks cb;
	cb.seek_func = vorb_seek;
	cb.tell_func = vorb_tell;
	cb.close_func = nullptr;
	cb.read_func = vorb_read;
	
	cursor = 0;
	
	decoder_error = ov_open_callbacks (this, dec.get (), nullptr, 0, cb);
}

VorbisDecoder::~VorbisDecoder () {
	ov_clear (dec.get ());
}

int VorbisDecoder::fill (int16_t *buffer, int length) {
	// Don't particularly care since we own all the audio in a game
	int bitstream = 0;
	long rc = ov_read (dec.get (), (char *)buffer, length, &bitstream);
	if (rc > 0) {
		// Divide by 2 for 16-bit
		return rc / sizeof (int16_t);
	}
	else {
		reset ();
		return 0;
	}
}

int VorbisDecoder::fill (void *d, int16_t *buffer, int length) {
	VorbisDecoder * that = (VorbisDecoder *)d;
	
	return that->fill (buffer, length);
}

PcmSound VorbisDecoder::decode_all (const std::vector <uint8_t> & data) {
	VorbisDecoder dec (data);
	
	PcmSound result;
	
	// For fug's sake
	auto * info = ov_info (dec.dec.get (), -1);
	result.channels = info->channels;
	result.rate = (int)info->rate;
	
	ogg_int64_t pcm_length = ov_pcm_total (dec.dec.get (), -1);
	
	result.buffer.resize (pcm_length * result.channels);
	
	ogg_int64_t cursor = 0;
	
	while (true) {
		int samples_read = dec.fill (&result.buffer [cursor], 
			result.buffer.size () - cursor);
		
		cursor += samples_read;
		
		if (samples_read == 0 || cursor >= (ogg_int64_t)result.buffer.size ()) {
			break;
		}
	}
	
	return result;
}

extern "C" {
size_t vorb_read (void *ptr, size_t size, size_t nmemb, void *datasource) {
	VorbisDecoder * that = (VorbisDecoder *)datasource;
	
	int remaining_bytes = that->buffer.size () - that->cursor;
	int desired_bytes = size * nmemb;
	
	if (desired_bytes > remaining_bytes) {
		memcpy (ptr, &(that->buffer [that->cursor]), remaining_bytes);
		that->cursor += remaining_bytes;
		return remaining_bytes;
	}
	else {
		memcpy (ptr, &(that->buffer [that->cursor]), desired_bytes);
		that->cursor += desired_bytes;
		return desired_bytes;
	}
}

int vorb_seek (void *datasource, int64_t offset, int whence) {
	VorbisDecoder * that = (VorbisDecoder *)datasource;
	
	switch (whence) {
		case SEEK_SET:
			that->cursor = offset;
			break;
		case SEEK_CUR:
			that->cursor += offset;
			break;
		case SEEK_END:
			that->cursor = that->buffer.size () + offset;
			break;
	}
	
	return 0;
}

long vorb_tell (void *datasource) {
	VorbisDecoder * that = (VorbisDecoder *)datasource;
	
	return that->cursor;
}
}

AudioStream::AudioStream (void * d, int (*df)(void *, int16_t *, int)) {
	dec = d;
	decoder_fill = df;
	channels = 2;
	opusFrequency = 48000;
	
	alGenBuffers (2, buffers);
	alGenSources (1, &source);
	
	const int size_multiplier = 8;
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
	
	for (int i = 0; i < 10; i++) {
		//cout << "Filling" << endl;
		fill ();
	}
	
	if (buffersQueued <= 1) {
		submit (backBuffer);
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
		sizeof (opus_int16), 44100); //opusFrequency);
		alSourceQueueBuffers (source, 1, &i);
		//cout << "Queued " << i << endl;
		bufferFill = 0;
	}
	
	lastFilled = i;
}
