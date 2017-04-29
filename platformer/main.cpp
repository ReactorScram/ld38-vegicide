#include <SDL/SDL.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <unordered_set>
#include <stdint.h>
#include <sstream>
#include <string>

//#include "boost/date_time/posix_time/posix_time.hpp"
#include <glm/glm.hpp>

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

ResourceTable make_resource_table () {
	ResourceTable rc;
	
	rc.shaders [(ShaderKey)EShader::Opaque] = ShaderFiles {"shaders/shader.vert", "shaders/shader.frag"};
	//rc.shaders [(ShaderKey)EShader::Tile] = ShaderFiles {"shaders/tile.vert", "shaders/shader.frag"};
	//rc.shaders [(ShaderKey)EShader::Particle] = ShaderFiles {"shaders/particle.vert", "shaders/particle.frag"};
	rc.shaders [(ShaderKey)EShader::Shadow] = ShaderFiles {"shaders/shader.vert", "shaders/shadow.frag"};
	
	rc.textures [(TextureKey)ETexture::Beet] = "textures/beet.png";
	rc.textures [(TextureKey)ETexture::BeetDead] = "textures/beet-dead.png";
	rc.textures [(TextureKey)ETexture::Blood] = "textures/blood.png";
	rc.textures [(TextureKey)ETexture::Carrot] = "textures/carrot.png";
	rc.textures [(TextureKey)ETexture::CarrotDead] = "textures/carrot-dead.png";
	rc.textures [(TextureKey)ETexture::CrabApple] = "textures/crab-apple.png";
	rc.textures [(TextureKey)ETexture::CrabAppleDead] = "textures/crab-apple-dead.png";
	rc.textures [(TextureKey)ETexture::Egg] = "textures/egg.png";
	rc.textures [(TextureKey)ETexture::Farm] = "textures/farm.png";
	rc.textures [(TextureKey)ETexture::Pumpking] = "textures/pumpking.png";
	rc.textures [(TextureKey)ETexture::PumpkingDead] = "textures/pumpking-dead.png";
	rc.textures [(TextureKey)ETexture::Shadow] = "textures/shadow.png";
	// Meine tilen!
	rc.textures [(TextureKey)ETexture::Tiles] = "textures/tiles.png";
	rc.textures [(TextureKey)ETexture::Title] = "textures/title.png";
	rc.textures [(TextureKey)ETexture::Venus] = "textures/venus.png";
	rc.textures [(TextureKey)ETexture::VenusDead] = "textures/venus-dead.png";
	rc.textures [(TextureKey)ETexture::Victory] = "textures/victory.png";
	rc.textures [(TextureKey)ETexture::Vignette] = "textures/vignette.png";
	rc.textures [(TextureKey)ETexture::White] = "textures/white.png";
	
	rc.meshes [(MeshKey)EMesh::DangerZone] = "meshes/danger-zone.iqm";
	rc.meshes [(MeshKey)EMesh::Square] = "meshes/square.iqm";
	rc.meshes [(MeshKey)EMesh::Venus] = "meshes/venus.iqm";
	
	return rc;
}

void to_json (stringstream & s, const float f) {
	s << f;
}

void to_json (stringstream & s, const int32_t i) {
	s << i;
}

void to_json (stringstream & s, const vec4 & v) {
	s << "[ ";
	
	for (int i = 0; i < 4; i++) {
		if (i > 0) {
			s << ", ";
		}
		s << v [i];
	}
	
	s << " ]";
}

void to_json (stringstream & s, const mat4 & m) {
	s << "[ ";
	
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			if (x > 0 || y > 0) {
				s << ", ";
			}
			s << m [y][x];
		}
	}
	
	s << " ]";
}

void to_json (stringstream & s, EcsTrue) {
	s << "true";
}

template <typename T>
void to_json (stringstream & s, const Components <T> & comps) {
	s << "{ ";
	
	bool first = true;
	
	for (auto pair : comps) {
		Entity e = pair.first;
		const T & v = pair.second;
		
		if (first) {
			first = false;
		}
		else {
			s << ",\n";
		}
		
		s << "\"" << e << "\": ";
		to_json (s, v);
	}
	
	s << " }";
}

template <typename T>
void to_json (stringstream & s, const vector <T> & v) {
	s << "[ ";
	
	bool first = true;
	
	for (auto e : v) {
		if (first) {
			first = false;
		}
		else {
			s << ",\n";
		}
		
		to_json (s, e);
	}
	
	s << " ]";
}

void to_json (stringstream & s, const Pass & p) {
	s << "{ ";
	
	s << "\"renderables\": ";
	to_json (s, p.renderables);
	s << ", " << endl;
	
	s << "\"proj_view_mat\": ";
	to_json (s, p.proj_view_mat);
	
	s << " }";
}

void to_json (stringstream & s, const GraphicsEcs & ecs) {
	s << "{ ";
	
	s << "\"passes\": ";
	to_json (s, ecs.passes);
	s << ", " << endl;
	
	s << "\"transparent_z\": ";
	to_json (s, ecs.transparent_z);
	s << ", " << endl;
	
	s << "\"rigid_mats\": ";
	to_json (s, ecs.rigid_mats);
	s << ", " << endl;
	
	s << "\"diffuse_colors\": ";
	to_json (s, ecs.diffuse_colors);
	s << ", " << endl;
	
	s << "\"meshes\": ";
	to_json (s, ecs.meshes);
	s << ", " << endl;
	
	s << "\"textures\": ";
	to_json (s, ecs.textures);
	
	s << " }";
}

int main (int /* argc */, char * /* argv */ []) {
	string window_title = "ReactorScram LD38 Vegicide";
	Terf::Archive terf ("rom.tar", "rom.tar.index");
	terf.enableTerfLookup = false;
	terf.enableFsLookup = true;
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
	
	bool play_demo = false;
	map <long, vector <KeyEvent> > input_key_log;
	if (play_demo) {
		auto buffer = terf.lookupFile ("key_log.bin");
		//cerr << "Loaded demo buffer of " << buffer.size () << " bytes" << endl;
		
		const int stride = 8 + 1 + 1;
		
		for (int i = 0; i < (int)buffer.size (); i += stride) {
			long f = *(long *)&buffer [i];
			
			KeyEvent ke;
			ke.down = *(bool *)&buffer [i + 8];
			ke.code = *(uint8_t *)&buffer [i + 9];
			
			//cerr << f << ", " << ke.down << ", " << (int)ke.code << endl;
			
			if (input_key_log.find (f) == input_key_log.end ()) {
				input_key_log [f] = vector <KeyEvent> ();
			}
			input_key_log [f].push_back (ke);
		}
	}
	
	ofstream key_log ("key_log.txt", ios_base::app);
	
	bool record_demo = true;
	if (record_demo) {
		key_log << "# Vegicide writes this file for debugging purposes but never reads it back" << endl;
		key_log << "# Frame count, key down, key code" << endl;
	}
	
	bool dumped_ecs = false;
	
	while (running) {
		auto frame_time = SDL_GetTicks ();
		auto numSteps = timestep.step (frame_time - last_frame_time);
		last_frame_time = frame_time;
		
		for (int i = 0; i < numSteps; i++) {
			SDL_Event ev;
			vector <KeyEvent> evs;
			
			while (SDL_PollEvent (&ev)) {
				if (ev.type == SDL_QUIT) {
					running = false;
				}
				else {
					evs.push_back (Input::encode (ev));
				}
			}
			
			if (play_demo) {
				//cerr << frames << endl;
				auto key_it = input_key_log.find (frames);
				if (key_it != input_key_log.end ()) {
					for (auto ev : key_it->second) {
						input.process (ev);
					}
				}
			}
			else {
				for (auto ev : evs) {
					if (record_demo) {
						if (ev.code != 255) {
						key_log << frames << ", " << (int)ev.down << ", " << (int)ev.code << endl;
						}
					}
					input.process (ev);
				}
			}
			
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
					
					if (dumped_ecs) {
						// Pass
					}
					else {
						stringstream s;
						to_json (s, graphics_ecs);
						
						ofstream of ("graphics_ecs.json");
						of << s.str () << endl;
						
						dumped_ecs = true;
					}
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
