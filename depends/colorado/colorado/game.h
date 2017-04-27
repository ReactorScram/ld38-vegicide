#ifndef COLORADOGAME_H
#define COLORADOGAME_H

#include "colorado/dllspec.h"

typedef struct SDL_Surface SDL_Surface;

namespace Colorado {
struct ScreenOptions;

// Note: Don't make two of these. All the SDL / GL stuff is global
// and so it would not work right
class COLORADO_DLLSPEC Game {
public:
	SDL_Surface * surface;
	
	// For the glory of RAII (just calls startSdl and stopSdl)
	Game (const ScreenOptions &);
	~Game ();
	
	static SDL_Surface * COLORADO_DLLSPEC startSdl (const ScreenOptions &);
	static void COLORADO_DLLSPEC stopSdl ();
};
}

#endif // COLORADOGAME_H
