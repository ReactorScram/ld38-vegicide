#include "colorado/game.h"

#include <iostream>
using std::cerr;
using std::endl;

#include <SDL/SDL.h>

#include "colorado/gl.h"
#include "colorado/screen-options.h"
#include "terf/terf.h"

namespace Colorado {

Game::Game (const ScreenOptions & screenOpts) {
	surface = startSdl (screenOpts);
}

Game::~Game () {
	stopSdl ();
}

SDL_Surface * Game::startSdl (const ScreenOptions & screenOpts) {
	if (0 != SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
		cerr << SDL_GetError () << endl;
	}
	
	SDL_GL_SetAttribute (SDL_GL_STENCIL_SIZE, 8);
	
	SDL_Surface * screen = NULL;
	
	#ifdef COLORADO_HAVE_GLES
		// OpenPandora
		screen = SDL_SetVideoMode (screenOpts.width, screenOpts.height, 0, SDL_SWSURFACE | SDL_FULLSCREEN);
	#else
		int flags = SDL_OPENGL;
		
		if (screenOpts.fullscreen) {
			flags |= SDL_FULLSCREEN;
		}
		
		#ifndef COLORADO_PANDORA
			SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, 4);
		#endif
		
		screen = SDL_SetVideoMode (screenOpts.width, screenOpts.height, 32, flags);
	#endif
	
	#ifdef USE_GLEW
		/*
		GLenum err = glewInit ();
		if (err != GLEW_OK) {
			cerr << "Serious GLEW error" << endl;
		}
		
		if (GLEW_VERSION_2_1) {
			//cerr << "OpenGL 2.1" << endl;
		}
		else if (GLEW_VERSION_2_0) {
			//cerr << "OpenGL 2.0" << endl;
		}
		else {
			//cerr << "glew version: " << GLEW_VERSION << endl;
		}
		*/
		
		GLeeInit ();
	#endif
	
	#ifdef COLORADO_HAVE_GLES
		if (EGL_Open (screenOpts.width, screenOpts.height)) {
			// Error
		}
	#endif
	
	return screen;
}

void Game::stopSdl () {
#ifdef HAVE_GLES
	EGL_Close ();
#endif
	
	SDL_Quit ();
}
}
