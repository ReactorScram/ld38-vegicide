#include "video-encoder/video-encoder.h"

// This file is used on the Pandora and Win32 where the x264 encoder is not compiled
// It does nothing

// For some reason NULL was not defined?
#define NULL 0

namespace Colorado {

const bool VideoEncoder::isAvailable = false;

VideoEncoder::VideoEncoder (const char *, int, int, int) {
	p = NULL;
}

VideoEncoder::~VideoEncoder () {
	
}

void VideoEncoder::accumulateFrameFromGL () {
	
}

void VideoEncoder::captureFrameFromGL (uint8_t *) {
	
}

void VideoEncoder::encodeFrame (uint8_t *, int64_t) {
	
}

void VideoEncoder::encodeAccumulatedFrame (int64_t) {
	
}

void VideoEncoder::encodeAudio (const std::vector <int16_t> &) {
	
}

void VideoEncoder::clearAccumulator () {
	
}

}
