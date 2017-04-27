-- The code below was contributed by David Hollander along with OpenALUT.cpp
-- To run on Windows, there are few choices, easiest one is to download
-- http://connect.creativelabs.com/openal/Downloads/oalinst.zip
-- and run the executable from inside of it (I've seen couple of games use it).

-- I found this on GitHub with no license, I don't think they'll mind

ffi = require 'ffi'
local al = ffi.load (ffi.os=="Windows" and "openal32" or "openal")
local alure = ffi.load ("alure")

ffi.cdef[[
typedef char ALboolean;         /** 8-bit boolean */
typedef char ALchar;            /** character */
typedef signed char ALbyte;     /** signed 8-bit 2's complement integer */
typedef unsigned char ALubyte;  /** unsigned 8-bit integer */
typedef short ALshort;          /** signed 16-bit 2's complement integer */
typedef unsigned short ALushort;/** unsigned 16-bit integer */
typedef int ALint;              /** signed 32-bit 2's complement integer */
typedef unsigned int ALuint;    /** unsigned 32-bit integer */
typedef int ALsizei;            /** non-negative 32-bit binary integer size */
typedef int ALenum;             /** enumerated 32-bit value */
typedef float ALfloat;          /** 32-bit IEEE754 floating-point */
typedef double ALdouble;        /** 64-bit IEEE754 floating-point */
typedef void ALvoid;            /** void type (for opaque pointers only) */
enum {
  AL_INVALID = -1,
  AL_NONE = 0,
  AL_FALSE = 0,
  AL_TRUE = 1,
  AL_SOURCE_RELATIVE = 0x202,
  AL_CONE_INNER_ANGLE = 0x1001,
  AL_CONE_OUTER_ANGLE = 0x1002,
  AL_PITCH = 0x1003,
  AL_POSITION = 0x1004,
  AL_DIRECTION = 0x1005,
  AL_VELOCITY = 0x1006,
  AL_LOOPING = 0x1007,
  AL_BUFFER = 0x1009,
  AL_GAIN = 0x100A,
  AL_MIN_GAIN = 0x100D,
  AL_MAX_GAIN = 0x100E,
  AL_ORIENTATION = 0x100F,
  AL_SOURCE_STATE = 0x1010,
  AL_INITIAL = 0x1011,
  AL_PLAYING = 0x1012,
  AL_PAUSED = 0x1013,
  AL_STOPPED = 0x1014,
  AL_BUFFERS_QUEUED = 0x1015,
  AL_BUFFERS_PROCESSED = 0x1016,
  AL_SEC_OFFSET = 0x1024,
  AL_SAMPLE_OFFSET = 0x1025,
  AL_BYTE_OFFSET = 0x1026,
  AL_SOURCE_TYPE = 0x1027,
  AL_STATIC = 0x1028,
  AL_STREAMING = 0x1029,
  AL_UNDETERMINED = 0x1030,
  AL_FORMAT_MONO8 = 0x1100,
  AL_FORMAT_MONO16 = 0x1101,
  AL_FORMAT_STEREO8 = 0x1102,
  AL_FORMAT_STEREO16 = 0x1103,
  AL_REFERENCE_DISTANCE = 0x1020,
  AL_ROLLOFF_FACTOR = 0x1021,
  AL_CONE_OUTER_GAIN = 0x1022,
  AL_MAX_DISTANCE = 0x1023,
  AL_FREQUENCY = 0x2001,
  AL_BITS = 0x2002,
  AL_CHANNELS = 0x2003,
  AL_SIZE = 0x2004,
  AL_UNUSED = 0x2010,
  AL_PENDING = 0x2011,
  AL_PROCESSED = 0x2012,
  AL_NO_ERROR = 0,
  AL_INVALID_NAME = 0xA001,
  AL_INVALID_ENUM = 0xA002,
  AL_INVALID_VALUE = 0xA003,
  AL_INVALID_OPERATION = 0xA004,
  AL_OUT_OF_MEMORY = 0xA005,
  AL_VENDOR = 0xB001,
  AL_VERSION = 0xB002,
  AL_RENDERER = 0xB003,
  AL_EXTENSIONS = 0xB004,
  AL_DOPPLER_FACTOR = 0xC000,
  AL_DOPPLER_VELOCITY = 0xC001,
  AL_SPEED_OF_SOUND = 0xC003,
  AL_DISTANCE_MODEL = 0xD000,
  AL_INVERSE_DISTANCE = 0xD001,
  AL_INVERSE_DISTANCE_CLAMPED = 0xD002,
  AL_LINEAR_DISTANCE = 0xD003,
  AL_LINEAR_DISTANCE_CLAMPED = 0xD004,
  AL_EXPONENT_DISTANCE = 0xD005,
  AL_EXPONENT_DISTANCE_CLAMPED = 0xD006,
};
void alEnable( ALenum capability );
void alDisable( ALenum capability ); 
ALboolean alIsEnabled( ALenum capability ); 
const ALchar* alGetString( ALenum param );
void alGetBooleanv( ALenum param, ALboolean* data );
void alGetIntegerv( ALenum param, ALint* data );
void alGetFloatv( ALenum param, ALfloat* data );
void alGetDoublev( ALenum param, ALdouble* data );
ALboolean alGetBoolean( ALenum param );
ALint alGetInteger( ALenum param );
ALfloat alGetFloat( ALenum param );
ALdouble alGetDouble( ALenum param );
ALenum alGetError( void );
ALboolean alIsExtensionPresent( const ALchar* extname );
void* alGetProcAddress( const ALchar* fname );
ALenum alGetEnumValue( const ALchar* ename );
void alListenerf( ALenum param, ALfloat value );
void alListener3f( ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
void alListenerfv( ALenum param, const ALfloat* values ); 
void alListeneri( ALenum param, ALint value );
void alListener3i( ALenum param, ALint value1, ALint value2, ALint value3 );
void alListeneriv( ALenum param, const ALint* values );
void alGetListenerf( ALenum param, ALfloat* value );
void alGetListener3f( ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3 );
void alGetListenerfv( ALenum param, ALfloat* values );
void alGetListeneri( ALenum param, ALint* value );
void alGetListener3i( ALenum param, ALint *value1, ALint *value2, ALint *value3 );
void alGetListeneriv( ALenum param, ALint* values );
void alGenSources( ALsizei n, ALuint* sources ); 
void alDeleteSources( ALsizei n, const ALuint* sources );
ALboolean alIsSource( ALuint sid ); 
void alSourcef( ALuint sid, ALenum param, ALfloat value ); 
void alSource3f( ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
void alSourcefv( ALuint sid, ALenum param, const ALfloat* values ); 
void alSourcei( ALuint sid, ALenum param, ALint value ); 
void alSource3i( ALuint sid, ALenum param, ALint value1, ALint value2, ALint value3 );
void alSourceiv( ALuint sid, ALenum param, const ALint* values );
void alGetSourcef( ALuint sid, ALenum param, ALfloat* value );
void alGetSource3f( ALuint sid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
void alGetSourcefv( ALuint sid, ALenum param, ALfloat* values );
void alGetSourcei( ALuint sid,  ALenum param, ALint* value );
void alGetSource3i( ALuint sid, ALenum param, ALint* value1, ALint* value2, ALint* value3);
void alGetSourceiv( ALuint sid,  ALenum param, ALint* values );
void alSourcePlayv( ALsizei ns, const ALuint *sids );
void alSourceStopv( ALsizei ns, const ALuint *sids );
void alSourceRewindv( ALsizei ns, const ALuint *sids );
void alSourcePausev( ALsizei ns, const ALuint *sids );
void alSourcePlay( ALuint sid );
void alSourceStop( ALuint sid );
void alSourceRewind( ALuint sid );
void alSourcePause( ALuint sid );
void alSourceQueueBuffers( ALuint sid, ALsizei numEntries, const ALuint *bids );
void alSourceUnqueueBuffers( ALuint sid, ALsizei numEntries, ALuint *bids );
void alGenBuffers( ALsizei n, ALuint* buffers );
void alDeleteBuffers( ALsizei n, const ALuint* buffers );
ALboolean alIsBuffer( ALuint bid );
void alBufferData( ALuint bid, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq );
void alBufferf( ALuint bid, ALenum param, ALfloat value );
void alBuffer3f( ALuint bid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
void alBufferfv( ALuint bid, ALenum param, const ALfloat* values );
void alBufferi( ALuint bid, ALenum param, ALint value );
void alBuffer3i( ALuint bid, ALenum param, ALint value1, ALint value2, ALint value3 );
void alBufferiv( ALuint bid, ALenum param, const ALint* values );
void alGetBufferf( ALuint bid, ALenum param, ALfloat* value );
void alGetBuffer3f( ALuint bid, ALenum param, ALfloat* value1, ALfloat* value2, ALfloat* value3);
void alGetBufferfv( ALuint bid, ALenum param, ALfloat* values );
void alGetBufferi( ALuint bid, ALenum param, ALint* value );
void alGetBuffer3i( ALuint bid, ALenum param, ALint* value1, ALint* value2, ALint* value3);
void alGetBufferiv( ALuint bid, ALenum param, ALint* values );
void alDopplerFactor( ALfloat value );
void alDopplerVelocity( ALfloat value );
void alSpeedOfSound( ALfloat value );
void alDistanceModel( ALenum distanceModel );

/** Opaque device handle */
typedef struct ALCdevice_struct ALCdevice;
/** Opaque context handle */
typedef struct ALCcontext_struct ALCcontext;

/** 8-bit boolean */
typedef char ALCboolean;

/** character */
typedef char ALCchar;

/** signed 8-bit 2's complement integer */
typedef signed char ALCbyte;

/** unsigned 8-bit integer */
typedef unsigned char ALCubyte;

/** signed 16-bit 2's complement integer */
typedef short ALCshort;

/** unsigned 16-bit integer */
typedef unsigned short ALCushort;

/** signed 32-bit 2's complement integer */
typedef int ALCint;

/** unsigned 32-bit integer */
typedef unsigned int ALCuint;

/** non-negative 32-bit binary integer size */
typedef int ALCsizei;

/** enumerated 32-bit value */
typedef int ALCenum;

/** 32-bit IEEE754 floating-point */
typedef float ALCfloat;

/** 64-bit IEEE754 floating-point */
typedef double ALCdouble;

/** void type (for opaque pointers only) */
typedef void ALCvoid;

enum {
ALC_FALSE = 0,
ALC_TRUE = 1,
ALC_FREQUENCY = 0x1007,
ALC_REFRESH = 0x1008,
ALC_SYNC = 0x1009,
ALC_MONO_SOURCES = 0x1010,
ALC_STEREO_SOURCES = 0x1011,
ALC_NO_ERROR = 0,
ALC_INVALID_DEVICE = 0xA001,
ALC_INVALID_CONTEXT = 0xA002,
ALC_INVALID_ENUM = 0xA003,
ALC_INVALID_VALUE = 0xA004,
ALC_OUT_OF_MEMORY = 0xA005,
ALC_MAJOR_VERSION = 0x1000,
ALC_MINOR_VERSION = 0x1001,
ALC_ATTRIBUTES_SIZE = 0x1002,
ALC_ALL_ATTRIBUTES = 0x1003,
ALC_DEFAULT_DEVICE_SPECIFIER = 0x1004,
ALC_DEVICE_SPECIFIER = 0x1005,
ALC_EXTENSIONS = 0x1006,
ALC_EXT_CAPTURE = 1,
ALC_CAPTURE_DEVICE_SPECIFIER = 0x310,
ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER = 0x311,
ALC_CAPTURE_SAMPLES = 0x312,
ALC_ENUMERATE_ALL_EXT = 1,
ALC_DEFAULT_ALL_DEVICES_SPECIFIER = 0x1012,
ALC_ALL_DEVICES_SPECIFIER = 0x1013
};

/** Context management. */
ALCcontext* alcCreateContext(ALCdevice *device, const ALCint* attrlist);
ALCboolean  alcMakeContextCurrent(ALCcontext *context);
void        alcProcessContext(ALCcontext *context);
void        alcSuspendContext(ALCcontext *context);
void        alcDestroyContext(ALCcontext *context);
ALCcontext* alcGetCurrentContext(void);
ALCdevice*  alcGetContextsDevice(ALCcontext *context);

/** Device management. */
ALCdevice* alcOpenDevice(const ALCchar *devicename);
ALCboolean alcCloseDevice(ALCdevice *device);


/**
 * Error support.
 *
 * Obtain the most recent Device error.
 */
ALCenum alcGetError(ALCdevice *device);

/**
 * Extension support.
 *
 * Query for the presence of an extension, and obtain any appropriate
 * function pointers and enum values.
 */
ALCboolean alcIsExtensionPresent(ALCdevice *device, const ALCchar *extname);
void*      alcGetProcAddress(ALCdevice *device, const ALCchar *funcname);
ALCenum    alcGetEnumValue(ALCdevice *device, const ALCchar *enumname);

/** Query function. */
const ALCchar* alcGetString(ALCdevice *device, ALCenum param);
void           alcGetIntegerv(ALCdevice *device, ALCenum param, ALCsizei size, ALCint *values);

/** Capture function. */
ALCdevice* alcCaptureOpenDevice(const ALCchar *devicename, ALCuint frequency, ALCenum format, ALCsizei buffersize);
ALCboolean alcCaptureCloseDevice(ALCdevice *device);
void       alcCaptureStart(ALCdevice *device);
void       alcCaptureStop(ALCdevice *device);
void       alcCaptureSamples(ALCdevice *device, ALCvoid *buffer, ALCsizei samples);

typedef int64_t alureInt64;
typedef uint64_t alureUInt64;

typedef struct alureStream alureStream;

void alureGetVersion(ALuint *major, ALuint *minor);
const ALchar* alureGetErrorString(void);

const ALCchar** alureGetDeviceNames(ALCboolean all, ALCsizei *count);
ALvoid alureFreeDeviceNames(const ALCchar **names);

ALboolean alureInitDevice(const ALCchar *name, const ALCint *attribs);
ALboolean alureShutdownDevice(void);

ALenum alureGetSampleFormat(ALuint channels, ALuint bits, ALuint floatbits);

ALboolean alureSleep(ALfloat duration);

ALboolean alureStreamSizeIsMicroSec(ALboolean useus);

ALuint alureCreateBufferFromFile(const ALchar *fname);
ALuint alureCreateBufferFromMemory(const ALubyte *data, ALsizei length);
ALboolean alureBufferDataFromFile(const ALchar *fname, ALuint buffer);
ALboolean alureBufferDataFromMemory(const ALubyte *fdata, ALsizei length, ALuint buffer);

alureStream* alureCreateStreamFromFile(const ALchar *fname, ALsizei chunkLength, ALsizei numBufs, ALuint *bufs);
alureStream* alureCreateStreamFromMemory(const ALubyte *data, ALuint length, ALsizei chunkLength, ALsizei numBufs, ALuint *bufs);
alureStream* alureCreateStreamFromStaticMemory(const ALubyte *data, ALuint length, ALsizei chunkLength, ALsizei numBufs, ALuint *bufs);
alureStream* alureCreateStreamFromCallback(
    ALuint (*callback)(void *userdata, ALubyte *data, ALuint bytes),
    void *userdata, ALenum format, ALuint samplerate,
    ALsizei chunkLength, ALsizei numBufs, ALuint *bufs);
alureInt64 alureGetStreamLength(alureStream *stream);
ALsizei alureGetStreamFrequency(alureStream *stream);
ALsizei alureBufferDataFromStream(alureStream *stream, ALsizei numBufs, ALuint *bufs);
ALboolean alureRewindStream(alureStream *stream);
ALboolean alureSetStreamOrder(alureStream *stream, ALuint order);
ALboolean alureSetStreamPatchset(alureStream *stream, const ALchar *patchset);
ALboolean alureDestroyStream(alureStream *stream, ALsizei numBufs, ALuint *bufs);

void alureUpdate(void);
ALboolean alureUpdateInterval(ALfloat interval);

ALboolean alurePlaySourceStream(ALuint source,
    alureStream *stream, ALsizei numBufs, ALsizei loopcount,
    void (*eos_callback)(void *userdata, ALuint source), void *userdata);
ALboolean alurePlaySource(ALuint source,
    void (*callback)(void *userdata, ALuint source), void *userdata);
ALboolean alureStopSource(ALuint source, ALboolean run_callback);
ALboolean alurePauseSource(ALuint source);
ALboolean alureResumeSource(ALuint source);

ALboolean alureInstallDecodeCallbacks(ALint index,
    void*     (*open_file)(const ALchar*),
    void*     (*open_mem)(const ALubyte*,ALuint),
    ALboolean (*get_format)(void*,ALenum*,ALuint*,ALuint*),
    ALuint    (*decode)(void*,ALubyte*,ALuint),
    ALboolean (*rewind)(void*),
    void      (*close)(void*));
ALboolean alureSetIOCallbacks(
      void* (*open)(const char*,ALuint),
      void (*close)(void*),
      ALsizei (*read)(void*,ALubyte*,ALuint),
      ALsizei (*write)(void*,const ALubyte*,ALuint),
      alureInt64 (*seek)(void*,alureInt64,int));

void* alureGetProcAddress(const ALchar *funcname);
]]

return {
	al = al,
	alure = alure,
}
