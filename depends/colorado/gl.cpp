#include "colorado/gl.h"

#include <SDL/SDL.h>

namespace Colorado {
void swapBuffers () {
#ifdef COLORADO_PANDORA
	EGL_SwapBuffers ();
#else
	SDL_GL_SwapBuffers ();
#endif
}
}
