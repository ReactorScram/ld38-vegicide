#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL/SDL.h>

#include "colorado/game.h"
#include "colorado/fixed-timestep.h"
#include "colorado/gl.h"
#include "colorado/triangle-shader.h"
#include "terf/terf.h"

#include "graphics/graphics.h"

using namespace glm;
using namespace std;

using namespace Colorado;

enum class ETexture {
	BenchAo,
	Gear8,
	Gear32,
	Noise,
	White,
};

enum class EMesh {
	Bench,
	BenchUpper,
	Gear8,
	Gear32,
	Square,
};

enum class EShader {
	Debug,
	Shadow,
};

ResourceTable make_resource_table () {
	ResourceTable rc;
	
	rc.shaders [(ShaderKey)EShader::Debug] = ShaderFiles {"shader.vert", "shader.frag"};
	rc.shaders [(ShaderKey)EShader::Shadow] = ShaderFiles {"shader.vert", "shadow.frag"};
	
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

GraphicsEcs animate (long frames) {
	double revolutions = (double)frames / 60.0;
	
	double axles [3];
	axles [0] = revolutions;
	axles [1] = -0.25 * axles [0] + (9.5 / 32.0);
	axles [2] = -0.25 * axles [1] + (5.5 / 32.0);
	
	vec3 red (1.0, 0.5, 0.5);
	vec3 cyan (0.5, 1.0, 1.0);
	
	GraphicsEcs graphics_ecs;
	
	// Add everything to a normal opaque pass
	Pass opaque;
	opaque.shader = (ShaderKey)EShader::Debug;
	
	Pass shadow;
	shadow.shader = (ShaderKey)EShader::Shadow;
	
	opaque.renderables [gear_8 (graphics_ecs, vec3 (-1.25, 0.0, 0.0), axles [0], cyan)];
	
	opaque.renderables [gear_32 (graphics_ecs, vec3 (0.0, 0.0, 0.0), axles [1], cyan)];
	opaque.renderables [gear_8 (graphics_ecs, vec3 (0.0, 0.0, 0.25), axles [1], red)];
	
	opaque.renderables [gear_32 (graphics_ecs, vec3 (1.25, 0.0, 0.25), axles [2], red)];
	
	mat4 shadow_mat;
	shadow_mat [1][1] = 0.0f;
	shadow_mat [1][0] = -0.25f;
	shadow_mat [1][2] = 0.25f;
	
	shadow_mat = translate (translate (mat4 (1.0f), vec3 (0.0, -1.49, 0.0)) * shadow_mat, vec3 (0.0, 1.49, 0.0));
	
	auto shadow_color = vec3 (0.125f);
	
	// Add all the gears to the shadow pass
	for (auto pair: opaque.renderables) {
		auto old_e = pair.first;
		
		auto e = graphics_ecs.add_entity ();
		
		auto old_model_mat = graphics_ecs.rigid_mats [old_e];
		
		graphics_ecs.rigid_mats [e] = shadow_mat * old_model_mat;
		graphics_ecs.diffuse_colors [e] = shadow_color;
		graphics_ecs.meshes [e] = graphics_ecs.meshes [old_e];
		graphics_ecs.textures [e] = (TextureKey)ETexture::White;
		
		shadow.renderables [e];
	}
	
	auto bench_mat = rotate (translate (mat4 (1.0f), vec3 (0.0f, -1.5f, 0.125f)), radians (-90.0f), vec3 (1.0f, 0.0f, 0.0f));
	
	// Also add this bogus shadow caster
	{
		auto e = graphics_ecs.add_entity ();
		
		graphics_ecs.rigid_mats [e] = shadow_mat * bench_mat;
		graphics_ecs.diffuse_colors [e] = shadow_color;
		graphics_ecs.meshes [e] = (MeshKey)EMesh::BenchUpper;
		shadow.renderables [e];
	}
	
	{
		Entity e = graphics_ecs.add_entity ();
		graphics_ecs.rigid_mats [e] = bench_mat;
		graphics_ecs.diffuse_colors [e] = vec3 (0.5f);
		graphics_ecs.meshes [e] = (MeshKey)EMesh::Bench;
		graphics_ecs.textures [e] = (TextureKey)ETexture::BenchAo;
		opaque.renderables [e];
	}
	
	graphics_ecs.passes [graphics_ecs.add_entity ()] = opaque;
	graphics_ecs.passes [graphics_ecs.add_entity ()] = shadow;
	
	return graphics_ecs;
}

int main () {
	ScreenOptions screen_opts;
	screen_opts.fullscreen = false;
	screen_opts.width = 800;
	screen_opts.height = 480;
	
	Colorado::Game game (screen_opts);
	SDL_WM_SetCaption ("ReactorScram LD38 warmup", nullptr);
	
	GLeeInit ();
	
	Terf::Archive terf ("rom.tar", "rom.tar.index");
	terf.enableTerfLookup = false;
	
	FixedTimestep timestep (60, 1);
	auto last_frame_time = SDL_GetTicks ();
	
	glEnable (GL_DEPTH_TEST);
	//glEnable (GL_CULL_FACE);
	glEnable (GL_TEXTURE_2D);
	glFrontFace (GL_CW);
	
	bool running = true;
	
	Graphics graphics;
	// TODO: Learn move constructors
	ResourceTable rc = make_resource_table ();
	graphics.load (terf, rc);
	
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
			auto graphics_ecs = animate (frames);
			
			// Render
			graphics.render (graphics_ecs, screen_opts);
		}
	}
	
	return 0;
}
