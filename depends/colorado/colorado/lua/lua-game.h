#ifndef COLORADO_LUA_GAME_H
#define COLORADO_LUA_GAME_H

#include "colorado/dllspec.h"

extern "C" {
	namespace Colorado {
	struct ScreenOptions;
	}
	typedef struct Colorado::ScreenOptions Colorado_ScreenOptions;
	
	typedef struct SDL_Surface SDL_Surface;
	SDL_Surface * COLORADO_DLLSPEC Colorado_Lua_Game_startSdl (const Colorado_ScreenOptions *);
	void COLORADO_DLLSPEC Colorado_Lua_Game_stopSdl ();
}

#endif // LUAGAME_H
