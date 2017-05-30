#ifndef COLORADOGL_H
#define COLORADOGL_H

#ifdef COLORADO_PANDORA
	#include <GLES2/gl2.h>
	#include <eglport/eglport.h>
	
	#define GL_CLAMP GL_CLAMP_TO_EDGE
#else
	#include <GLee.h>
#endif

#include "colorado/dllspec.h"

namespace Colorado {
	COLORADO_DLLSPEC void swapBuffers ();
}

#endif // COLORADOGL_H
