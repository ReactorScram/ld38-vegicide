#ifndef COLORADO_VIDEO_ENCODER_C_H
#define COLORADO_VIDEO_ENCODER_C_H

#include "stdint.h"

#include "colorado/dllspec.h"

extern "C" {
typedef struct VideoEncoder Colorado_VideoEncoder;

COLORADO_DLLSPEC Colorado_VideoEncoder * Colorado_VideoEncoder_new (const char *, int, int);
COLORADO_DLLSPEC void Colorado_VideoEncoder_delete (Colorado_VideoEncoder *);

COLORADO_DLLSPEC void Colorado_VideoEncoder_accumulateFrameFromGL (Colorado_VideoEncoder *);
COLORADO_DLLSPEC void Colorado_VideoEncoder_captureFrameFromGL (Colorado_VideoEncoder *, uint8_t *);

COLORADO_DLLSPEC void Colorado_VideoEncoder_encodeFrame (Colorado_VideoEncoder *, uint8_t *, int64_t);
COLORADO_DLLSPEC void Colorado_VideoEncoder_encodeAccumulatedFrame (Colorado_VideoEncoder *, int64_t);

COLORADO_DLLSPEC void Colorado_VideoEncoder_clearAccumulator (Colorado_VideoEncoder *);

COLORADO_DLLSPEC bool Colorado_VideoEncoder_isAvailable ();
}

#endif
