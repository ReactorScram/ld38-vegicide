#include <algorithm>
#include <chrono>
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

#include "graphics/graphics.h"
#include "scene.h"

using glm::radians;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mod;
using namespace std;
using namespace std::chrono;

using namespace Colorado;

ResourceTable make_resource_table () {
	ResourceTable rc;
	
	rc.shaders [(ShaderKey)EShader::Opaque] = ShaderFiles {"shaders/shader.vert", "shaders/shader.frag"};
	rc.shaders [(ShaderKey)EShader::Particle] = ShaderFiles {"shaders/particle.vert", "shaders/particle.frag"};
	rc.shaders [(ShaderKey)EShader::Shadow] = ShaderFiles {"shaders/shader.vert", "shaders/shadow.frag"};
	
	rc.textures [(TextureKey)ETexture::Carrot] = "textures/carrot.png";
	rc.textures [(TextureKey)ETexture::Farm] = "textures/farm.png";
	rc.textures [(TextureKey)ETexture::Shadow] = "textures/shadow.png";
	rc.textures [(TextureKey)ETexture::White] = "textures/white.png";
	
	rc.meshes [(MeshKey)EMesh::Square] = "meshes/square.iqm";
	rc.meshes [(MeshKey)EMesh::Venus] = "meshes/venus.iqm";
	
	return rc;
}

mat4 get_billboard_mat_2 (const vec3 & pos, const vec3 & camera_forward) {
	vec3 towardsCamera = -camera_forward;
	towardsCamera.y = 0.0f;
	towardsCamera = normalize (towardsCamera);
	vec3 up (0.0f, 1.0f, 0.0f);
	vec3 sideways = cross (towardsCamera, up);
	
	float scale = 0.5f;
	
	return mat4 (
		vec4 (-scale * sideways, 0.0f), 
		vec4 (-scale * towardsCamera, 0.0f),
		vec4 (scale * up, 0.0f),
		vec4 (pos, 1.0f));
}

mat4 get_billboard_mat (const vec3 & pos, const vec3 & camera_pos)
{
	return get_billboard_mat_2 (pos, pos - camera_pos);
}

struct ParticlePos {
	vec4 color;
	vec3 pos;
};

struct SpriteSorter {
	vec3 cameraPos;
	
	SpriteSorter (vec3 p) : cameraPos (p) {}
	
	// Voodoo!
	bool operator () (const ParticlePos & a, const ParticlePos & b) const {
		auto whateverA = a.pos - cameraPos;
		auto whateverB = b.pos - cameraPos;
		return dot (whateverA, whateverA) > dot (whateverB, whateverB);
	}
};

uint64_t get_epoch () {
	auto now = std::chrono::system_clock::now().time_since_epoch ();
	return duration_cast <milliseconds> (now).count ();
}

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
	
	SDL_WM_SetCaption (window_title.c_str (), nullptr);
	
	auto epoch = get_epoch ();
	stringstream ss;
	ss << "videos/" << epoch << ".mkv";
	//VideoEncoder ve (ss.str ().c_str (), screen_opts.width, screen_opts.height, 44100);
	
	long frames = 0;
	
	SceneEcs ecs;
	
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
			frames++;
		}
		
		if (numSteps == 0) {
			SDL_Delay (2);
		}
		else {
			// Animate
			auto graphics_ecs = animate_vegicide (ecs, frames, screen_opts);
			
			// Render
			graphics.render (graphics_ecs);
			
			// Video encoder isn't working - SAD!
			//ve.accumulateFrameFromGL ();
			//ve.encodeAccumulatedFrame (frames * 1000 / 60);
		}
	}
	
	return 0;
}
