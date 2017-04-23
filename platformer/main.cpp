#include <algorithm>
#include <chrono>
#include <iostream>
#include <unordered_set>
#include <stdint.h>
#include <sstream>

//#include "boost/date_time/posix_time/posix_time.hpp"
#include <glm/glm.hpp>
#include <SDL/SDL.h>

#include "colorado/game.h"
#include "colorado/fixed-timestep.h"
#include "terf/terf.h"
#include "video-encoder/video-encoder.h"

#include "animation.h"
#include "audio/audio.h"
#include "graphics/graphics.h"
#include "input.h"
#include "logic.h"
#include "scene.h"

using glm::radians;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mod;
using namespace std;
using namespace std::chrono;

using namespace Colorado;

uint64_t get_epoch () {
	auto now = std::chrono::system_clock::now().time_since_epoch ();
	return duration_cast <milliseconds> (now).count ();
}

enum class GameState {
	Title,
	Game,
};

int main () {
	string window_title = "ReactorScram LD38 warmup";
	Terf::Archive terf ("rom.tar", "rom.tar.index");
	terf.enableTerfLookup = false;
	ResourceTable rc = make_resource_table ();
	
	// End of game-specific bits
	
	ScreenOptions screen_opts;
	screen_opts.fullscreen = false;
	screen_opts.width = 800;
	screen_opts.height = 480;
	
	Colorado::Game game (screen_opts);
	
	FixedTimestep timestep (60, 1);
	auto last_frame_time = SDL_GetTicks ();
	
	bool running = true;
	
	// TODO: Learn move constructors
	Graphics graphics;
	graphics.load (terf, rc);
	
	Level level (terf.lookupFile ("maps/demo.bin"));
	level.load_sqlite_objects ("maps/demo.sqlite");
	
	auto level_iqm = level.to_iqm ();
	if (false) {
		cerr << "Level IQM size = " << level_iqm.size () << endl;
		ofstream iqm_out ("fuck.iqm");
		iqm_out.write ((const char *)&level_iqm [0], level_iqm.size ());
	}
	
	graphics.meshes.add_iqm ((MeshKey)EMesh::Level, level_iqm);
	
	SDL_WM_SetCaption (window_title.c_str (), nullptr);
	
	auto epoch = get_epoch ();
	stringstream ss;
	ss << "videos/" << epoch << ".mkv";
	//VideoEncoder ve (ss.str ().c_str (), screen_opts.width, screen_opts.height, 44100);
	
	long frames = 0;
	
	Input input;
	Logic logic (level);
	
	GameState game_state = GameState::Title;
	float curtain_t = 0.0f;
	
	Audio audio (terf);
	
	while (running) {
		SDL_Event ev;
		
		while (SDL_PollEvent (&ev)) {
			if (ev.type == SDL_QUIT) {
				running = false;
			}
			else {
				input.process (ev);
			}
		}
		
		auto frame_time = SDL_GetTicks ();
		auto numSteps = timestep.step (frame_time - last_frame_time);
		last_frame_time = frame_time;
		
		for (int i = 0; i < numSteps; i++) {
			// Step
			switch (game_state) {
				case GameState::Game:
					logic.step (input.frame, frames);
					break;
				case GameState::Title:
					float curtain_speed = 1.5f / 60.0f;
					curtain_t -= curtain_speed;
					
					for (int i = 0; i < (int)InputButton::NUM_BUTTONS; i++) {
						if (input.frame.taps [i]) {
							game_state = GameState::Game;
						}
						if (input.frame.buttons [i]) {
							curtain_t += curtain_speed * 2;
						}
					}
					
					curtain_t = glm::clamp (curtain_t, 0.0f, 1.0f);
					
					break;
			}
			input.clear_taps ();
			
			audio.update (logic.scene.audio_frame);
			
			frames++;
		}
		
		if (numSteps == 0) {
			SDL_Delay (2);
		}
		else {
			// Animate
			GraphicsEcs graphics_ecs;
			switch (game_state) {
				case GameState::Title:
					graphics_ecs = animate_title (frames, curtain_t, screen_opts);
					break;
				case GameState::Game:
					graphics_ecs = animate_vegicide (logic.scene, logic.level, frames, screen_opts);
					break;
			}
			
			// Render
			glViewport (0, 0, 800, 480);
			graphics.render (graphics_ecs);
			
			// Video encoder isn't working - SAD!
			//ve.accumulateFrameFromGL ();
			//ve.encodeAccumulatedFrame (frames * 1000 / 60);
		}
	}
	
	return 0;
}
