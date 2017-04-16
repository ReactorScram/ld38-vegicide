#include <SDL/SDL.h>

#include "colorado/game.h"
#include "colorado/fixed-timestep.h"
#include "colorado/gl.h"
#include "colorado/screen-options.h"
#include "terf/terf.h"

using namespace std;
using namespace Colorado;

int main () {
	ScreenOptions screen_opts;
	screen_opts.fullscreen = false;
	screen_opts.width = 800;
	screen_opts.height = 480;
	
	Colorado::Game game (screen_opts);
	SDL_WM_SetCaption ("Colorado Hexture Map", nullptr);
	
	GLeeInit ();
	
	Terf::Archive terf ("rom.tar", "rom.tar.index");
	terf.enableTerfLookup = false;
	
	FixedTimestep timestep (60, 1);
	auto last_frame_time = SDL_GetTicks ();
	
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_CULL_FACE);
	glFrontFace (GL_CW);
	
	bool running = true;
	
	while (running) {
		SDL_Event ev;
		
		while (SDL_PollEvent (&ev)) {
			if (ev.type == SDL_QUIT) {
				running = false;
			}
		}
		
		auto frame_time = SDL_GetTicks ();
		auto numSteps = timestep.step (frame_time - last_frame_time);
		last_frame_time = frame_time;
		
		for (int i = 0; i < numSteps; i++) {
			// Step
		}
		
		// Render
		
		Colorado::swapBuffers ();
	}
	
	return 0;
}
