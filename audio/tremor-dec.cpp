#include "tremor-dec.h"

#include "tremor/ivorbisfile.h"
#include <iostream>
#include <string.h>

using namespace std;

extern "C" {
size_t vorb_read (void *ptr, size_t size, size_t nmemb, void *datasource);
int vorb_seek (void *datasource, int64_t offset, int whence);
long vorb_tell (void *datasource);
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
	//cout << "Tremor error " << decoder_error << endl;
}

VorbisDecoder::~VorbisDecoder () {
	ov_clear (dec.get ());
}

int VorbisDecoder::fill (int16_t *buffer, int length) {
	if (! playing) {
		return 0;
	}
	
	// Don't particularly care since we own all the audio in a game
	int bitstream = 0;
	long rc = ov_read (dec.get (), (char *)buffer, length, &bitstream);
	if (rc > 0) {
		// Divide by 2 for 16-bit
		return rc / sizeof (int16_t);
	}
	else {
		if (looping) {
			reset ();
		}
		else {
			playing = false;
		}
		return 0;
	}
}

int VorbisDecoder::fill (void *d, int16_t *buffer, int length) {
	VorbisDecoder * that = (VorbisDecoder *)d;
	
	int rc = that->fill (buffer, length);
	//cout << "Vorbis filled " << rc << " samples of " << length << endl;
	return rc;
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

// These are passed as callbacks to Tremor for its virtual files
size_t vorb_read (void *ptr, size_t size, size_t nmemb, void *datasource) {
	VorbisDecoder * that = (VorbisDecoder *)datasource;
	
	int remaining_bytes = that->buffer.size () - that->cursor;
	int desired_bytes = size * nmemb;
	
	int bytes_to_read = min (desired_bytes, remaining_bytes);
	
	copy (&(that->buffer [that->cursor]), &(that->buffer [that->cursor + bytes_to_read]), (uint8_t *)ptr);
	
	//memcpy (ptr, &(that->buffer [that->cursor]), bytes_to_read);
	
	that->cursor += bytes_to_read;
	
	//cerr << "vorb_read " << bytes_to_read << endl;
	
	return bytes_to_read;
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
