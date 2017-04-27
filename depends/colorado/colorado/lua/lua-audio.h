#ifndef COLORADO_LUA_AUDIO_H
#define COLORADO_LUA_AUDIO_H

#include <stdint.h>

#include "colorado/dllspec.h"

struct lua_State;

extern "C" {
	// Callbacks give Lua some trouble so I want to catch them in C++
	// and pass them through the event queue
	void COLORADO_DLLSPEC Colorado_Lua_hookMusicFinished (void);
	
	// Used for recording
	void COLORADO_DLLSPEC Colorado_Lua_Audio_recordChunk (void *, uint8_t *, int);
	void * COLORADO_DLLSPEC Colorado_Lua_Audio_startRecording (const char *);
	void COLORADO_DLLSPEC Colorado_Lua_Audio_stopRecording (void *);
}

#endif
