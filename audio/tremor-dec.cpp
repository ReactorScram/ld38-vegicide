#include "tremor-dec.h"

//#include "ivorbisfile.h"

extern "C" {
#include "stb_vorbis.c"
}

using namespace std;

StbWrapper::StbWrapper (const std::vector <uint8_t> & buffer) {
	stb = stb_vorbis_open_memory (&buffer [0], buffer.size (), nullptr, nullptr);
}

StbWrapper::~StbWrapper () {
	if (stb != nullptr) {
		free (stb);
	}
	stb = nullptr;
}

stb_vorbis * StbWrapper::operator *() const {
	return stb;
}

VorbisDecoder::VorbisDecoder (const vector<uint8_t> &vorbisBuffer): 
	as (this, VorbisDecoder::fill), buffer (vorbisBuffer)
{
	reset ();
}

void VorbisDecoder::reset () {
	dec = unique_ptr <StbWrapper> (new StbWrapper (buffer));
}

int VorbisDecoder::fill (int16_t *buffer, int length) {
	if (! playing) {
		return 0;
	}
	
	// Don't particularly care since we own all the audio in a game
	/*
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
	*/
	
	int rc = stb_vorbis_get_samples_short_interleaved (**dec, 2, buffer, length);
	
	if (rc == 0) {
		if (looping) {
			reset ();
		}
		else {
			playing = false;
		}
	}
	
	return rc * 2;
}

int VorbisDecoder::fill (void *d, int16_t *buffer, int length) {
	VorbisDecoder * that = (VorbisDecoder *)d;
	
	return that->fill (buffer, length);
}

PcmSound VorbisDecoder::decode_all (const vector <uint8_t> & vorbisBuffer) {
	int frequency = 44100;
	int channels = 2;
	short * output = nullptr;
	int samplesDecoded = stb_vorbis_decode_memory (
		vorbisBuffer.data (), vorbisBuffer.size (), 
		&channels, &frequency, &output);
	
	vector <int16_t> result;
	result.resize (samplesDecoded * channels);
	memcpy (&result [0], output, result.size ());
	
	if (output != nullptr) {
		free (output);
	}
	
	return PcmSound { result, channels, frequency };
}
