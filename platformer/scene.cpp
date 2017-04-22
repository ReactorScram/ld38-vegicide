#include "animation.h"
#include "scene.h"

#include <glm/gtc/matrix_transform.hpp>

#include "colorado/camera.h"
#include "colorado/screen-options.h"

using namespace Colorado;
using namespace glm;

ResourceTable make_resource_table () {
	ResourceTable rc;
	
	rc.shaders [(ShaderKey)EShader::Opaque] = ShaderFiles {"shaders/shader.vert", "shaders/shader.frag"};
	rc.shaders [(ShaderKey)EShader::Particle] = ShaderFiles {"shaders/particle.vert", "shaders/particle.frag"};
	rc.shaders [(ShaderKey)EShader::Shadow] = ShaderFiles {"shaders/shader.vert", "shaders/shadow.frag"};
	
	rc.textures [(TextureKey)ETexture::Carrot] = "textures/carrot.png";
	rc.textures [(TextureKey)ETexture::CarrotDead] = "textures/carrot-dead.png";
	rc.textures [(TextureKey)ETexture::Farm] = "textures/farm.png";
	rc.textures [(TextureKey)ETexture::Shadow] = "textures/shadow.png";
	rc.textures [(TextureKey)ETexture::White] = "textures/white.png";
	
	rc.meshes [(MeshKey)EMesh::Square] = "meshes/square.iqm";
	rc.meshes [(MeshKey)EMesh::Venus] = "meshes/venus.iqm";
	
	return rc;
}

vec3 two2three (vec3 v) {
	return vec3 (v.x, v.y + v.z, 0.0f);
}

Entity add_sprite (GraphicsEcs & ecs, const vec3 & pos, const vec3 & size, const vec3 & color, ETexture texture) 
{
	auto e = ecs.add_entity ();
	
	ecs.rigid_mats [e] = rotate (scale (translate (mat4 (1.0f), two2three (pos)), size), radians (-90.0f), vec3 (1.0f, 0.0f, 0.0f));
	ecs.diffuse_colors [e] = color;
	ecs.meshes [e] = (MeshKey)EMesh::Square;
	ecs.textures [e] = (TextureKey)texture;
	ecs.transparent_z [e] = pos.y;
	
	return e;
}

GraphicsEcs animate_vegicide (const SceneEcs & scene, long frames, const ScreenOptions & screen_opts) 
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
		ecs.transparent_z [e] = 0.0f;
		
		opaque.renderables [e];
	}
	
	float t = frames * 2.0 * 3.1415926535 / 60.0f;
	
	vec3 shadow_color (0.5f);
	
	// Carrot
	for (auto pair : scene.carrots) {
		auto old_e = pair.first;
		
		vec3 base_pos = two2three (scene.positions.at (old_e));
		
		vec3 jump (0.0f, 0.0f, 1.0f + abs (sin (t)));
		vec3 size (1.0f);
		auto tex = ETexture::Carrot;
		vec3 color (1.0f);
		
		{
			auto dead_it = scene.dead.find (old_e);
			if (dead_it != scene.dead.end () && (*dead_it).second) {
				tex = ETexture::CarrotDead;
				jump = vec3 (0.0f);
			}
		}
		
		{
			auto targeted_it = scene.targeted.find (old_e);
			if ((frames % 16) < 8 && targeted_it != scene.targeted.end () && (*targeted_it).second) 
			{
				color = vec3 (1.0f, 0.0f, 0.0f);
			}
		}
		
		auto e = add_sprite (ecs, base_pos + jump, size, color, tex);
		
		transparent.renderables [e];
		
		{
			float shadow_scale = 0.25f;
			auto s = add_sprite (ecs, base_pos, vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale), shadow_color, ETexture::Shadow);
			
			shadows.renderables [s];
		}
	}
	
	// Venus
	for (auto pair : scene.venuses) {
		auto old_e = pair.first;
		auto venus = pair.second;
		auto e = ecs.add_entity ();
		
		auto pos = scene.positions.at (old_e);
		
		vec3 base_pos = two2three (pos);
		
		{
			float shadow_scale = 0.5f;
			auto s = add_sprite (ecs, two2three (pos * vec3 (1.0f, 1.0f, 0.0f)), vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale), shadow_color, ETexture::Shadow);
			
			shadows.renderables [s];
		}
		
		vec3 breathe_size (1.0f - 0.125f * sin (t), 1.0f + 0.125f * sin (t), 1.0f);
		
		vec3 tense_size (1.0f + 0.125f, 0.75f, 1.0f);
		
		vec3 size = mix (breathe_size, tense_size, venus.pounce_anim);
		
		ecs.rigid_mats [e] = scale (translate (mat4 (1.0f), base_pos + vec3 (0.0f, size.y, 0.0f)), size);
		
		if (venus.pounce_anim == 1.0f && (frames % 16) < 8) {
			ecs.diffuse_colors [e] = vec3 (1.0f, 0.1f, 0.1f);
		}
		else {
			ecs.diffuse_colors [e] = vec3 (0.005f, 0.228f, 0.047f);
		}
		ecs.meshes [e] = (MeshKey)EMesh::Venus;
		ecs.textures [e] = (TextureKey)ETexture::White;
		ecs.transparent_z [e] = base_pos.y;
		
		transparent.renderables [e];
	}
	
	ecs.passes.push_back (opaque);
	ecs.passes.push_back (shadows);
	ecs.passes.push_back (transparent);
	
	return ecs;
}
