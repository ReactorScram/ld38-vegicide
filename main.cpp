#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL/SDL.h>

#include "colorado/attribute-enabler.h"
#include "colorado/camera.h"
#include "colorado/game.h"
#include "colorado/fixed-timestep.h"
#include "colorado/gl.h"
#include "colorado/screen-options.h"
#include "colorado/triangle-shader.h"
#include "terf/terf.h"

#include "ecs.h"
#include "mesh-binder.h"
#include "shader-binder.h"
#include "texture-binder.h"

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

enum class EShader {
	Debug,
};

struct Graphics {
	ShaderBinder shaders;
	AttributeEnabler ae;
	unordered_set <int> attrib_set;
	TextureBinder textures;
	MeshBinder meshes;
	
	const Colorado::TriangleShader * current_shader () const {
		return shaders.currentShader ();
	}
	
	void render (const GraphicsEcs & ecs, const ScreenOptions & screen_opts) {
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shaders.bind ((ShaderKey)EShader::Debug);
		
		auto uni_up = current_shader ()->uniformLocation ("up");
		auto uni_color = current_shader ()->uniformLocation ("diffuseColor");
		
		ae.enableAttributes (attrib_set);
		textures.bind ((TextureKey)ETexture::Noise);
		
		Camera camera;
		auto proj_mat = camera.generateProjectionMatrix (screen_opts.width, screen_opts.height);
		
		auto view_mat = mat4 (1.0f);
		view_mat = translate (view_mat, vec3 (0.0f, 0.0f, -3.0f));
		
		auto proj_view_mat = proj_mat * view_mat;
		
		for (const auto pair: ecs.opaque_pass) {
			auto e = pair.first;
			auto model_mat = ecs.rigid_mats.at (e);
			auto color = ecs.diffuse_colors.at (e);
			auto mesh = ecs.meshes.at (e);
			
			current_shader ()->setMvpMatrix (proj_view_mat * model_mat);
			
			auto light_mat = inverse (model_mat);
			auto object_up = light_mat * vec4 (0.0, 1.0, 0.0, 0.0);
			
			glUniform3f (uni_up, object_up.x, object_up.y, object_up.z);
			glUniform3f (uni_color, color.r, color.g, color.b);
			
			meshes.bind (mesh);
			
			const int floats_per_vert = 3 + 2 + 3 + 4;
			const int stride = sizeof (GLfloat) * floats_per_vert;
			
			glVertexAttribPointer (current_shader ()->vertPosAttribute, 3, GL_FLOAT, false, stride, (char *)nullptr + 0);
			glVertexAttribPointer (current_shader ()->vertNormAttribute, 3, GL_FLOAT, false, stride, (char *)nullptr + sizeof (GLfloat) * (3 + 2));
			
			meshes.currentMesh ()->renderPlacementIndexed (0);
		}
		
		Colorado::swapBuffers ();
	}
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
