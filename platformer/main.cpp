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
	rc.textures [(TextureKey)ETexture::Lenna] = "textures/Lenna.png";
	rc.textures [(TextureKey)ETexture::Noise] = "hexture/noise.png";
	rc.textures [(TextureKey)ETexture::BenchAo] = "textures/bench-ao.png";
	rc.textures [(TextureKey)ETexture::Gear8] = "textures/gear8-ao.png";
	rc.textures [(TextureKey)ETexture::Gear32] = "textures/gear32-ao.png";
	rc.textures [(TextureKey)ETexture::White] = "textures/white.png";
	
	rc.meshes [(MeshKey)EMesh::Bench] = "meshes/bench.iqm";
	rc.meshes [(MeshKey)EMesh::BenchUpper] = "meshes/bench-upper.iqm";
	rc.meshes [(MeshKey)EMesh::Gear8] = "meshes/gear8.iqm";
	rc.meshes [(MeshKey)EMesh::Gear32] = "meshes/gear32.iqm";
	rc.meshes [(MeshKey)EMesh::Square] = "meshes/square.iqm";
	rc.meshes [(MeshKey)EMesh::Venus] = "meshes/venus.iqm";
	
	return rc;
}

Entity gear_32 (GraphicsEcs & ecs, vec3 pos, double revolutions, vec3 color) {
	auto gear = ecs.add_entity ();
	
	float radians = (mod (revolutions, 1.0)) * 2.0 * 3.1415926535;
	
	float gear_scale = 2.0 / ((2.097 + 2.0) * 0.5);
	
	ecs.rigid_mats [gear] = scale (rotate (translate (mat4 (1.0f), pos), radians, vec3 (0.0f, 0.0f, 1.0f)), vec3 (gear_scale, gear_scale, 0.5f));
	ecs.diffuse_colors [gear] = color;
	ecs.meshes [gear] = (MeshKey)EMesh::Gear32;
	ecs.textures [gear] = (TextureKey)ETexture::Gear32;
	
	return gear;
}

Entity gear_8 (GraphicsEcs & ecs, vec3 pos, double revolutions, vec3 color) {
	auto gear = ecs.add_entity ();
	
	float radians = (mod (revolutions, 1.0)) * 2.0 * 3.1415926535;
	
	float gear_scale = 0.5 / ((0.597 + 0.5) * 0.5);
	
	ecs.rigid_mats [gear] = scale (rotate (translate (mat4 (1.0f), pos), radians, vec3 (0.0f, 0.0f, 1.0f)), vec3 (gear_scale, gear_scale, 0.5f));
	ecs.diffuse_colors [gear] = color;
	ecs.meshes [gear] = (MeshKey)EMesh::Gear8;
	ecs.textures [gear] = (TextureKey)ETexture::Gear8;
	
	return gear;
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

GraphicsEcs animate_vegicide_demo (long /*frames*/, const ScreenOptions & screen_opts) 
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
	opaque_state.frontFace = GL_CW;
	
	GlState transparent_state;
	transparent_state.bools [GL_DEPTH_TEST] = true;
	transparent_state.bools [GL_BLEND] = true;
	transparent_state.bools [GL_CULL_FACE] = true;
	transparent_state.depthMask = false;
	transparent_state.blendFunc [0] = GL_SRC_ALPHA;
	transparent_state.blendFunc [1] = GL_ONE_MINUS_SRC_ALPHA;
	
	Pass opaque;
	opaque.shader = (ShaderKey)EShader::Opaque;
	opaque.gl_state = opaque_state;
	opaque.proj_view_mat = proj_view_mat;
	
	Pass transparent;
	transparent.shader = (ShaderKey)EShader::Particle;
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
	
	// Venus
	{
		auto e = ecs.add_entity ();
		
		ecs.rigid_mats [e] = mat4 (1.0f);
		ecs.diffuse_colors [e] = vec3 (0.005f, 0.228f, 0.047f);
		ecs.meshes [e] = (MeshKey)EMesh::Venus;
		ecs.textures [e] = (TextureKey)ETexture::White;
		
		opaque.renderables [e];
	}
	
	ecs.passes.push_back (opaque);
	ecs.passes.push_back (transparent);
	
	return ecs;
}

GraphicsEcs animate_gear_demo (long frames, const ScreenOptions & screen_opts) {
	double revolutions = (double)frames / 60.0;
	
	double axles [3];
	axles [0] = revolutions;
	axles [1] = -0.25 * axles [0] + (9.5 / 32.0);
	axles [2] = -0.25 * axles [1] + (5.5 / 32.0);
	
	vec3 red (1.0, 0.5, 0.5);
	vec3 cyan (0.5, 1.0, 1.0);
	
	Camera camera;
	camera.fov = 0.25;
	auto proj_mat = camera.generateProjectionMatrix (screen_opts.width, screen_opts.height);
	
	float phi = 0.0f; //20.0f;
	float theta = 0.0f; //30.0f;
	
	auto view_mat = mat4 (1.0f);
	view_mat = rotate (rotate (translate (view_mat, vec3 (-0.0f, 0.5f, -15.0f)), radians (phi), vec3 (1.0f, 0.0f, 0.0f)), radians (theta), vec3 (0.0f, 1.0f, 0.0f));
	auto camera_pos = inverse (view_mat) * vec4 (0.0, 0.0, 0.0, 1.0);
	auto camera_forward = inverse (view_mat) * vec4 (0.0, 0.0, -1.0, 0.0);
	
	auto proj_view_mat = proj_mat * view_mat;
	
	GraphicsEcs graphics_ecs;
	
	// TODO: Cache these!
	// GL states
	GlState opaque_state;
	opaque_state.bools [GL_DEPTH_TEST] = true;
	opaque_state.bools [GL_CULL_FACE] = true;
	opaque_state.frontFace = GL_CW;
	
	GlState opaque_backface_state;
	opaque_backface_state.bools [GL_DEPTH_TEST] = true;
	opaque_backface_state.bools [GL_CULL_FACE] = true;
	opaque_backface_state.frontFace = GL_CCW;
	
	// Pulls shadows from the stencil buffer
	GlState shadow_state;
	shadow_state.bools [GL_DEPTH_TEST] = true;
	shadow_state.bools [GL_CULL_FACE] = true;
	shadow_state.bools [GL_STENCIL_TEST] = true;
	shadow_state.depthFunc = GL_ALWAYS;
	shadow_state.stencilFunc [0] = GL_EQUAL;
	shadow_state.stencilFunc [1] = 0x01;
	shadow_state.stencilFunc [2] = 0x01;
	shadow_state.stencilOp [0] = GL_KEEP;
	shadow_state.stencilOp [1] = GL_KEEP;
	shadow_state.stencilOp [2] = GL_KEEP;
	
	// Renders shadows into the stencil for later
	GlState shadow_stencil_state;
	shadow_stencil_state.bools [GL_DEPTH_TEST] = true;
	shadow_stencil_state.bools [GL_CULL_FACE] = true;
	shadow_stencil_state.bools [GL_STENCIL_TEST] = true;
	shadow_stencil_state.frontFace = GL_CCW;
	shadow_stencil_state.stencilFunc [0] = GL_ALWAYS;
	shadow_stencil_state.stencilFunc [1] = 0x01;
	shadow_stencil_state.stencilFunc [2] = 0x01;
	shadow_stencil_state.stencilOp [0] = GL_KEEP;
	shadow_stencil_state.stencilOp [1] = GL_KEEP;
	shadow_stencil_state.stencilOp [2] = GL_REPLACE;
	
	GlState transparent_state;
	transparent_state.bools [GL_DEPTH_TEST] = true;
	transparent_state.bools [GL_BLEND] = true;
	transparent_state.bools [GL_CULL_FACE] = true;
	transparent_state.depthMask = false;
	transparent_state.blendFunc [0] = GL_SRC_ALPHA;
	transparent_state.blendFunc [1] = GL_ONE_MINUS_SRC_ALPHA;
	
	// Passes
	Pass casters;
	casters.shader = (ShaderKey)EShader::Opaque;
	casters.gl_state = opaque_state;
	casters.proj_view_mat = proj_view_mat;
	
	Pass receivers_backface;
	receivers_backface.shader = (ShaderKey)EShader::Shadow;
	receivers_backface.gl_state = opaque_backface_state;
	receivers_backface.proj_view_mat = proj_view_mat;
	
	Pass shadow_stencil;
	shadow_stencil.shader = (ShaderKey)EShader::Shadow;
	shadow_stencil.gl_state = shadow_stencil_state;
	shadow_stencil.proj_view_mat = proj_view_mat;
	
	Pass receivers;
	receivers.shader = (ShaderKey)EShader::Opaque;
	receivers.gl_state = opaque_state;
	receivers.proj_view_mat = proj_view_mat;
	
	Pass shadow;
	shadow.shader = (ShaderKey)EShader::Shadow;
	shadow.gl_state = shadow_state;
	shadow.proj_view_mat = proj_view_mat;
	
	Pass transparent;
	transparent.shader = (ShaderKey)EShader::Particle;
	transparent.gl_state = transparent_state;
	transparent.proj_view_mat = proj_view_mat;
	
	float gear_y = 0.0f;
	
	casters.renderables [gear_8 (graphics_ecs, vec3 (-1.25, gear_y, 0.0), axles [0], cyan)];
	
	casters.renderables [gear_32 (graphics_ecs, vec3 (0.0, gear_y, 0.0), axles [1], cyan)];
	casters.renderables [gear_8 (graphics_ecs, vec3 (0.0, gear_y, 0.25), axles [1], red)];
	
	casters.renderables [gear_32 (graphics_ecs, vec3 (1.25, gear_y, 0.25), axles [2], red)];
	
	vec3 bench_color (1.0f);
	auto bench_mat = rotate (translate (mat4 (1.0f), vec3 (0.0f, -1.5f, 0.125f)), radians (-90.0f), vec3 (1.0f, 0.0f, 0.0f));
	
	{
		auto e = graphics_ecs.add_entity ();
		
		graphics_ecs.rigid_mats [e] = bench_mat;
		graphics_ecs.diffuse_colors [e] = bench_color;
		graphics_ecs.meshes [e] = (MeshKey)EMesh::BenchUpper;
		graphics_ecs.textures [e] = (TextureKey)ETexture::BenchAo;
		casters.renderables [e];
	}
	
	{
		Entity e = graphics_ecs.add_entity ();
		graphics_ecs.rigid_mats [e] = bench_mat;
		graphics_ecs.diffuse_colors [e] = bench_color;
		graphics_ecs.meshes [e] = (MeshKey)EMesh::Bench;
		graphics_ecs.textures [e] = (TextureKey)ETexture::BenchAo;
		receivers.renderables [e];
	}
	
	mat4 shadow_mat;
	shadow_mat [1][1] = 0.0f;
	shadow_mat [1][0] = -0.25f;
	shadow_mat [1][2] = 0.25f;
	
	shadow_mat = translate (translate (mat4 (1.0f), vec3 (0.0, -1.5, 0.0)) * shadow_mat, vec3 (0.0, 1.5, 0.0));
	
	auto shadow_color = vec3 (0.125f, 0.0f, 0.125f);
	
	// Add all the gears to the shadow pass
	for (auto pair: casters.renderables) {
		auto old_e = pair.first;
		
		auto e = graphics_ecs.add_entity ();
		
		auto old_model_mat = graphics_ecs.rigid_mats [old_e];
		
		graphics_ecs.rigid_mats [e] = shadow_mat * old_model_mat;
		graphics_ecs.diffuse_colors [e] = shadow_color;
		graphics_ecs.meshes [e] = graphics_ecs.meshes [old_e];
		graphics_ecs.textures [e] = (TextureKey)ETexture::White;
		
		shadow.renderables [e];
	}
	
	receivers_backface.renderables = receivers.renderables;
	shadow_stencil.renderables = shadow.renderables;
	
	// Add particles I guess
	{
		auto e = graphics_ecs.add_entity ();
		ParticleArray lennas;
		
		lennas.mesh = (MeshKey)EMesh::Square;
		lennas.texture = (TextureKey)ETexture::Lenna;
		
		vector <ParticlePos> particles;
		
		mat4 particle_mat = get_billboard_mat_2 (vec3 (0.0f), camera_forward);
		
		const int n = 10;
		for (int i = 0; i < n; i++) {
			const float t = (float)i / n;
			
			ParticlePos p;
			p.color = vec4 (1.0f, 1.0f - t, 1.0f, 0.5f);
			
			vec3 base (-1.5f + 2.0f * (i - n / 2), 0.0f, 1.0f);
			
			float radius = 0.5f;
			
			float theta = (mod (revolutions / 4.0, 1.0) + t / 4.0) * 2 * 3.1415926535;
			vec3 offset (0.0, 0.0, sin (theta));
			
			p.pos = vec4 (base + radius * offset, 1.0f);
			
			particles.push_back (p);
		}
		
		SpriteSorter sorter (camera_pos);
		sort (particles.begin (), particles.end (), sorter);
		
		for (const ParticlePos & p: particles) {
			lennas.particles.push_back (Particle {
				translate (particle_mat, p.pos),
				p.color
			});
		}
		
		transparent.particle_arrays.push_back (e);
		graphics_ecs.particle_arrays [e] = lennas;
	}
	
	graphics_ecs.passes.push_back (casters);
	graphics_ecs.passes.push_back (receivers_backface);
	graphics_ecs.passes.push_back (shadow_stencil);
	graphics_ecs.passes.push_back (receivers);
	graphics_ecs.passes.push_back (shadow);
	graphics_ecs.passes.push_back (transparent);
	
	return graphics_ecs;
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