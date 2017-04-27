#ifndef COLORADO_VIDEO_ENCODER_H
#define COLORADO_VIDEO_ENCODER_H

#include <stdint.h>
#include <vector>

#include "colorado/dllspec.h"

namespace Colorado {
class VideoEncoderPrivate;

class COLORADO_DLLSPEC VideoEncoder {
public:
	VideoEncoder (const char *, int, int, int);
	~VideoEncoder ();
	
	void accumulateFrameFromGL ();
	void captureFrameFromGL (uint8_t *);
	
	void encodeFrame (uint8_t *, int64_t);
	void encodeAccumulatedFrame (int64_t);
	
	void encodeAudio (const std::vector <int16_t> &);
	
	void clearAccumulator ();
	
	// Tells whether Colorado was compiled with x264 support
	const static bool isAvailable;
	
protected:
	VideoEncoderPrivate * p;
};
}

#endif // VIDEOENCODER_H
