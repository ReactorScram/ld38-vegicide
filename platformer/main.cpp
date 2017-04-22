#include <algorithm>
#include <chrono>
#include <unordered_set>
#include <stdint.h>
#include <sstream>

//#include "boost/date_time/posix_time/posix_time.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL/SDL.h>

#include "colorado/camera.h"
#include "colorado/game.h"
#include "colorado/fixed-timestep.h"
#include "terf/terf.h"
#include "video-encoder/video-encoder.h"

#include "graphics/graphics.h"

using glm::radians;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mod;
using namespace std;
using namespace std::chrono;

using namespace Colorado;

enum class ETexture {
	BenchAo,
	Carrot,
	Farm,
	Gear8,
	Gear32,
	Lenna,
	Noise,
	Shadow,
	White,
};

enum class EMesh {
	Bench,
	BenchUpper,
	Gear8,
	Gear32,
	Square,
	Venus,
};

enum class EShader {
	Opaque,
	Particle,
	Shadow,
};

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

Entity add_sprite (GraphicsEcs & ecs, const vec3 & pos, const vec3 & size, const vec3 & color, ETexture texture) 
{
	auto e = ecs.add_entity ();
	
	ecs.rigid_mats [e] = rotate (scale (translate (mat4 (1.0f), pos), size), radians (-90.0f), vec3 (1.0f, 0.0f, 0.0f));
	ecs.diffuse_colors [e] = color;
	ecs.meshes [e] = (MeshKey)EMesh::Square;
	ecs.textures [e] = (TextureKey)texture;
	
	return e;
}

GraphicsEcs animate_vegicide_demo (long frames, const ScreenOptions & screen_opts) 
{
	Camera camera;
	camera.fov = 0.25;
	//auto proj_mat = camera.generateProjectionMatrix (screen_opts.width, screen_opts.height);
	
	float aspect = (double)screen_opts.width / (double)screen_opts.height;
	
	auto proj_mat = scale (glm::ortho (-aspect, aspect, -1.0f, 1.0f), vec3 (1.0f / 8.0f));
	
	auto view_mat = mat4 (1.0f);
	view_mat = translate (view_mat, vec3 (0.0f, 0.0f, 0.0f));
	//auto camera_pos = inverse (view_mat) * vec4 (0.0, 0.0, 0.0, 1.0);
	//auto camera_forward = inverse (view_mat) * vec4 (0.0, 0.0, -1.0, 0.0);
	
	auto proj_view_mat = proj_mat * view_mat;
	
	GlState opaque_state;
	opaque_state.bools [GL_DEPTH_TEST] = false;
	opaque_state.bools [GL_CULL_FACE] = false;
	
	GlState transparent_state;
	transparent_state.bools [GL_DEPTH_TEST] = false;
	transparent_state.bools [GL_BLEND] = true;
	transparent_state.bools [GL_CULL_FACE] = false;
	transparent_state.blendFunc [0] = GL_SRC_ALPHA;
	transparent_state.blendFunc [1] = GL_ONE_MINUS_SRC_ALPHA;
	
	GlState shadow_state;
	shadow_state.bools [GL_DEPTH_TEST] = false;
	shadow_state.bools [GL_BLEND] = true;
	shadow_state.bools [GL_CULL_FACE] = false;
	shadow_state.blendFunc [0] = GL_DST_COLOR;
	shadow_state.blendFunc [1] = GL_ZERO;
	
	Pass opaque;
	opaque.shader = (ShaderKey)EShader::Opaque;
	opaque.gl_state = opaque_state;
	opaque.proj_view_mat = proj_view_mat;
	
	Pass shadows;
	shadows.shader = (ShaderKey)EShader::Shadow;
	shadows.gl_state = shadow_state;
	shadows.proj_view_mat = proj_view_mat;
	
	Pass transparent;
	transparent.shader = (ShaderKey)EShader::Opaque;
	transparent.gl_state = transparent_state;
	transparent.proj_view_mat = proj_view_mat;
	
	GraphicsEcs ecs;
	
	// Farm
	{
		auto e = ecs.add_entity ();
		
		ecs.rigid_mats [e] = rotate (scale (mat4 (1.0f), vec3 (-aspect * 8.0f, 8.0f, 1.0f)), radians (-90.0f), vec3 (1.0f, 0.0f, 0.0f));
		ecs.diffuse_colors [e] = vec3 (1.0f);
		ecs.meshes [e] = (MeshKey)EMesh::Square;
		ecs.textures [e] = (TextureKey)ETexture::Farm;
		
		opaque.renderables [e];
	}
	
	float t = frames * 2.0 * 3.1415926535 / 60.0f;
	
	vec3 shadow_color (0.5f);
	
	// Carrot
	{
		vec3 base_pos (0.0f, 0.0f, 0.0f);
		
		vec3 jump (0.0f, 1.0f + abs (sin (t)), 0.0f);
		vec3 size (1.0f);
		auto tex = ETexture::Carrot;
		vec3 color (1.0f);
		
		auto e = add_sprite (ecs, base_pos + jump, size, color, tex);
		
		transparent.renderables [e];
		
		{
			float shadow_scale = 0.25f;
			auto s = add_sprite (ecs, base_pos, vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale), shadow_color, ETexture::Shadow);
			
			shadows.renderables [s];
		}
	}
	
	// Venus
	{
		auto e = ecs.add_entity ();
		
		vec3 base_pos (0.0f, -4.0f, 0.0f);
		
		{
			float shadow_scale = 0.5f;
			auto s = add_sprite (ecs, base_pos, vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale), shadow_color, ETexture::Shadow);
			
			shadows.renderables [s];
		}
		
		vec3 size (1.0f - 0.125f * sin (t), 1.0f + 0.125f * sin (t), 1.0f);
		vec3 pos = base_pos + vec3 (0.0f, size.y, 0.0f);
		
		ecs.rigid_mats [e] = scale (translate (mat4 (1.0f), pos), size);
		ecs.diffuse_colors [e] = vec3 (0.005f, 0.228f, 0.047f);
		ecs.meshes [e] = (MeshKey)EMesh::Venus;
		ecs.textures [e] = (TextureKey)ETexture::White;
		
		transparent.renderables [e];
	}
	
	ecs.passes.push_back (opaque);
	ecs.passes.push_back (shadows);
	ecs.passes.push_back (transparent);
	
	return ecs;
}

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
			auto graphics_ecs = animate_vegicide_demo (frames, screen_opts);
			
			// Render
			graphics.render (graphics_ecs);
			
			// Video encoder isn't working - SAD!
			//ve.accumulateFrameFromGL ();
			//ve.encodeAccumulatedFrame (frames * 1000 / 60);
		}
	}
	
	return 0;
}
