#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL/SDL.h>

#include "colorado/game.h"
#include "colorado/fixed-timestep.h"
#include "colorado/gl.h"
#include "colorado/triangle-shader.h"
#include "terf/terf.h"

#include "graphics.h"

using namespace glm;
using namespace std;

using namespace Colorado;

enum class ETexture {
	Noise,
};

enum class EMesh {
	Gear8,
	Gear32,
	Square,
};

void load_graphics (Graphics & g, Terf::Archive & terf) {
	g.shaders.addShader ((ShaderKey)EShader::Debug, newShader (terf, "shader.vert", "shader.frag"));
	
	g.shaders.bind ((ShaderKey)EShader::Debug);
	
	g.attrib_set.insert (g.current_shader ()->vertPosAttribute);
	g.attrib_set.insert (g.current_shader ()->vertNormAttribute);
	
	g.textures.add ((TextureKey)ETexture::Noise, new Texture (terf, "hexture/noise.png"));
	
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	g.meshes.add_iqm ((MeshKey)EMesh::Square, terf.lookupFile ("meshes/square.iqm"));
	g.meshes.add_iqm ((MeshKey)EMesh::Gear8, terf.lookupFile ("meshes/gear8.iqm"));
	g.meshes.add_iqm ((MeshKey)EMesh::Gear32, terf.lookupFile ("meshes/gear32.iqm"));
}

Entity add_gear (GraphicsEcs & ecs, vec3 color, EMesh mesh) {
	Entity gear = ecs.add_entity ();
	
	ecs.opaque_pass [gear] = EcsTrue ();
	ecs.diffuse_colors [gear] = color;
	ecs.meshes [gear] = (MeshKey)mesh;
	
	return gear;
}

Entity add_gear_32 (GraphicsEcs & ecs, Entity gear, vec3 pos, double revolutions) {
	float radians = (mod (revolutions, 1.0)) * 2.0 * 3.1415926535;
	
	float gear_scale = 2.0 / ((2.097 + 2.0) * 0.5);
	
	ecs.rigid_mats [gear] = scale (rotate (translate (mat4 (1.0f), pos), radians, vec3 (0.0f, 0.0f, 1.0f)), vec3 (gear_scale));
	
	return gear;
}

Entity add_gear_8 (GraphicsEcs & ecs, Entity gear, vec3 pos, double revolutions) {
	float radians = (mod (revolutions, 1.0)) * 2.0 * 3.1415926535;
	
	float gear_scale = 0.5 / ((0.597 + 0.5) * 0.5);
	
	ecs.rigid_mats [gear] = scale (rotate (translate (mat4 (1.0f), pos), radians, vec3 (0.0f, 0.0f, 1.0f)), vec3 (gear_scale));
	
	return gear;
}

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
	//glEnable (GL_CULL_FACE);
	glEnable (GL_TEXTURE_2D);
	glFrontFace (GL_CW);
	
	bool running = true;
	
	Graphics graphics;
	// TODO: Learn move constructors
	load_graphics (graphics, terf);
	
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
		
		// Animate
		
		double revolutions = (double)frames / 720.0;
		
		GraphicsEcs graphics_ecs;
		
		add_gear_8 (graphics_ecs, add_gear (graphics_ecs, vec3 (0.5, 1.0, 1.0), EMesh::Gear8), vec3 (-0.25, 0.0, 0.0), -4 * revolutions + (1.5 / 8.0));
		
		add_gear_32 (graphics_ecs, add_gear (graphics_ecs, vec3 (1.0, 0.5, 0.5), EMesh::Gear32), vec3 (1.0, 0.0, 0.0), revolutions);
		
		// Render
		graphics.render (graphics_ecs, screen_opts);
	}
	
	return 0;
}
