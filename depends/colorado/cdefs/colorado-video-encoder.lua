return [[
// Colorado VideoEncoder
typedef struct VideoEncoder Colorado_VideoEncoder;

Colorado_VideoEncoder * Colorado_VideoEncoder_new (const char *, int, int);
void Colorado_VideoEncoder_delete (Colorado_VideoEncoder *);

void Colorado_VideoEncoder_accumulateFrameFromGL (Colorado_VideoEncoder *);
void Colorado_VideoEncoder_captureFrameFromGL (Colorado_VideoEncoder *, uint8_t *);

void Colorado_VideoEncoder_encodeFrame (Colorado_VideoEncoder *, uint8_t *, int64_t);
void Colorado_VideoEncoder_encodeAccumulatedFrame (Colorado_VideoEncoder *, int64_t);

void Colorado_VideoEncoder_clearAccumulator (Colorado_VideoEncoder *);

bool Colorado_VideoEncoder_isAvailable ();
]]