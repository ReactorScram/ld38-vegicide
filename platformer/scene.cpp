#include "animation.h"
#include "scene.h"

#include <glm/gtc/matrix_transform.hpp>

#include "colorado/camera.h"
#include "colorado/screen-options.h"

#include "level.h"
#include "prns.h"

using namespace Colorado;
using namespace glm;

ResourceTable make_resource_table () {
	ResourceTable rc;
	
	rc.shaders [(ShaderKey)EShader::Opaque] = ShaderFiles {"shaders/shader.vert", "shaders/shader.frag"};
	//rc.shaders [(ShaderKey)EShader::Tile] = ShaderFiles {"shaders/tile.vert", "shaders/shader.frag"};
	rc.shaders [(ShaderKey)EShader::Particle] = ShaderFiles {"shaders/particle.vert", "shaders/particle.frag"};
	rc.shaders [(ShaderKey)EShader::Shadow] = ShaderFiles {"shaders/shader.vert", "shaders/shadow.frag"};
	
	rc.textures [(TextureKey)ETexture::Blood] = "textures/blood.png";
	rc.textures [(TextureKey)ETexture::Carrot] = "textures/carrot.png";
	rc.textures [(TextureKey)ETexture::CarrotDead] = "textures/carrot-dead.png";
	rc.textures [(TextureKey)ETexture::Farm] = "textures/farm.png";
	rc.textures [(TextureKey)ETexture::Shadow] = "textures/shadow.png";
	// Meine tilen!
	rc.textures [(TextureKey)ETexture::Tiles] = "textures/tiles.png";
	rc.textures [(TextureKey)ETexture::Title] = "textures/title.png";
	rc.textures [(TextureKey)ETexture::White] = "textures/white.png";
	
	rc.meshes [(MeshKey)EMesh::DangerZone] = "meshes/danger-zone.iqm";
	rc.meshes [(MeshKey)EMesh::Square] = "meshes/square.iqm";
	rc.meshes [(MeshKey)EMesh::Venus] = "meshes/venus.iqm";
	
	return rc;
}

vec3 two2three (vec3 v) {
	return vec3 (v.x, v.y + v.z, 0.0f);
}

Entity add_sprite (GraphicsEcs & ecs, const vec3 & pos, const vec3 & size, const vec4 & color, ETexture texture) 
{
	auto e = ecs.add_entity ();
	
	ecs.rigid_mats [e] = scale (translate (mat4 (1.0f), two2three (pos)), size);
	ecs.diffuse_colors [e] = color;
	ecs.meshes [e] = (MeshKey)EMesh::Square;
	ecs.textures [e] = (TextureKey)texture;
	ecs.transparent_z [e] = pos.y;
	
	return e;
}

float shake (float rads, float hz, float strength) {
	return strength * sin (rads * hz);
}

// Curtain is normalized 0 to 1
void curtain (GraphicsEcs & ecs, float t) {
	float smooth_t = (1 - t) * (1 - t);
	
	GlState opaque_state;
	opaque_state.bools [GL_DEPTH_TEST] = false;
	opaque_state.bools [GL_CULL_FACE] = false;
	
	Pass curtain;
	curtain.shader = (ShaderKey)EShader::Opaque;
	curtain.gl_state = opaque_state;
	curtain.proj_view_mat = glm::ortho (0.0f, 800.0f, 0.0f, 480.0f);
	
	auto e = ecs.add_entity ();
	
	ecs.rigid_mats [e] = scale (translate (mat4 (1.0f), vec3 (400.0f, smooth_t * 480.0f + 240, 0.0f)), vec3 (400, 240, 0));
	ecs.diffuse_colors [e] = vec4 (34.0f / 256, 32.0f / 256, 52.0f / 256, 1.0f);
	ecs.meshes [e] = (MeshKey)EMesh::Square;
	ecs.textures [e] = (TextureKey)ETexture::White;
	ecs.transparent_z [e] = 0.0f;
	
	curtain.renderables [e];
	
	ecs.passes.push_back (curtain);
}

/*
"If a man will begin with curtain_t's, he shall end in doubts; but if he will be content to begin with doubts, he shall end in curtain_t's." -- France is Bacon 
 */
GraphicsEcs animate_title (long frames, float curtain_t, const ScreenOptions & /*screen_opts*/)
{
	const float t = frames / 60.0f;
	float phase = t * 2.0 * 3.1415926535;
	float screen_shake = mod (t, 4.0f) > 3.25f ? 1.0f : 0.0f;
	
	float noise = 0;
	if (screen_shake > 0.0f) {
		
		noise = shake (phase, 27, 2) + shake (phase, 20, 2) + shake (phase, 14, 3);
	}
	
	vec3 camera (floor (-56 + noise), floor (0), 0);
	
	GlState opaque_state;
	opaque_state.bools [GL_DEPTH_TEST] = false;
	opaque_state.bools [GL_CULL_FACE] = false;
	
	Pass opaque;
	opaque.shader = (ShaderKey)EShader::Opaque;
	opaque.gl_state = opaque_state;
	opaque.proj_view_mat = translate (glm::ortho (0.0f, 800.0f, 0.0f, 480.0f), -camera);
	
	GraphicsEcs ecs;
	
	if (true) {
		auto e = ecs.add_entity ();
		
		ecs.rigid_mats [e] = scale (translate (mat4 (1.0f), vec3 (400.0f, 240.0f, 0.0f)), vec3 (512.0f, 256.0f, 0.0f));
		ecs.diffuse_colors [e] = vec4 (1.0f);
		ecs.meshes [e] = (MeshKey)EMesh::Square;
		ecs.textures [e] = (TextureKey)ETexture::Title;
		ecs.transparent_z [e] = 0.0f;
		
		opaque.renderables [e];
	}
	
	ecs.passes.push_back (opaque);
	curtain (ecs, curtain_t);
	
	return ecs;
}

GraphicsEcs animate_vegicide (const SceneEcs & scene, const Level &, long frames, const ScreenOptions & screen_opts) 
{
	const float t = frames * 2.0 * 3.1415926535 / 60.0f;
	float screen_shake = scene.screenshake_t;
	
	float noise = 0;
	if (screen_shake > 0.0f) {
		noise = shake (t, 20, 2) + shake (t, 14, 3) + shake (t, 8, 5);
	}
	
	vec3 camera (floor (10 + noise), floor (0), 0);
	
	const float aspect = (double)screen_opts.width / (double)screen_opts.height;
	
	const float mob_scale = 1.0f / 15.0f;
	
	const auto proj_mat = glm::ortho (0.0f, aspect, 0.0f, 1.0f);
	
	const auto view_mat = translate (scale (mat4 (1.0f), vec3 (mob_scale)), -camera / 32.0f);
	//auto camera_pos = inverse (view_mat) * vec4 (0.0, 0.0, 0.0, 1.0);
	//auto camera_forward = inverse (view_mat) * vec4 (0.0, 0.0, -1.0, 0.0);
	
	const auto proj_view_mat = proj_mat * view_mat;
	
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
	opaque.proj_view_mat = glm::ortho (0.0f, 800.0f, 0.0f, 480.0f);
	
	Pass shadows;
	shadows.shader = (ShaderKey)EShader::Shadow;
	shadows.gl_state = shadow_state;
	shadows.proj_view_mat = proj_view_mat;
	
	Pass transparent;
	transparent.shader = (ShaderKey)EShader::Opaque;
	transparent.gl_state = transparent_state;
	transparent.proj_view_mat = proj_view_mat;
	
	GraphicsEcs ecs;
	
	
	
	// Level 
	if (true) {
		auto e = ecs.add_entity ();
		
		ecs.rigid_mats [e] = scale (translate (mat4 (1.0f), vec3 (0.0f - camera.x, 480.0f - camera.y, 0.0f)), vec3 (32.0f, -32.0f, 0.0f));
		ecs.diffuse_colors [e] = vec4 (1.0f);
		ecs.meshes [e] = (MeshKey)EMesh::Level;
		ecs.textures [e] = (TextureKey)ETexture::Tiles;
		ecs.transparent_z [e] = 0.0f;
		
		opaque.renderables [e];
	}
	
	vec4 shadow_color (vec3 (0.5f), 1.0f);
	
	// Carrot
	for (auto pair : scene.carrots) {
		auto old_e = pair.first;
		
		vec3 base_pos = two2three (scene.positions.at (old_e));
		
		bool dead = false;
		{
			auto dead_it = scene.dead.find (old_e);
			dead = dead_it != scene.dead.end () && (*dead_it).second;
		}
		
		float jump_ofs = abs (sin (t));
		if (dead) {
			jump_ofs = 0.0f;
		}
		vec3 jump (0.0f, 0.0f, 1.0f + jump_ofs);
		vec3 size (1.0f);
		auto tex = ETexture::Carrot;
		vec4 base_color (1.0f);
		
		
		
		vec4 blood_color = shadow_color;
		float shadow_scale = max (0.0f, 0.25f / (jump_ofs + 1.0f));
		auto shadow_tex = ETexture::Shadow;
		
		if (dead) {
			tex = ETexture::CarrotDead;
			jump = vec3 (0.0f);
			blood_color = vec4 (117.0f / 256, 28.0f / 256, 0.0f / 256, 1.0f);
			shadow_scale *= 4.0f;
			shadow_tex = ETexture::Blood;
		}
		
		{
			auto targeted_it = scene.targeted.find (old_e);
			if ((frames % 16) < 8 && targeted_it != scene.targeted.end () && (*targeted_it).second) 
			{
				base_color = vec4 (1.0f, 0.0f, 0.0f, 1.0f);
			}
		}
		
		auto e = add_sprite (ecs, base_pos + jump, size, base_color, tex);
		
		transparent.renderables [e];
		
		{
			auto s = add_sprite (ecs, base_pos, vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale), blood_color, shadow_tex);
			
			if (dead) {
				ecs.rigid_mats [s] = rotate (scale (translate (mat4 (1.0f), two2three (base_pos)), vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale)), (float)old_e, vec3 (0.0f, 0.0f, 1.0f));
			}
			
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
			float shadow_scale = max (0.0f, 0.5f / (pos.z + 1.0f));
			auto s = add_sprite (ecs, two2three (pos * vec3 (1.0f, 1.0f, 0.0f)), vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale), shadow_color, ETexture::Shadow);
			
			shadows.renderables [s];
		}
		
		vec3 breathe_size (1.0f - 0.125f * sin (t), 1.0f + 0.125f * sin (t), 1.0f);
		
		vec3 tense_size (1.0f + 0.125f, 0.75f, 1.0f);
		
		vec3 size = mix (breathe_size, tense_size, venus.pounce_anim);
		
		bool add_danger_zone = venus.pounce_anim > 1.0f / 10.0f;
		
		if (venus.pounce_anim == 1.0f) {
			if ((frames % 16) < 8) {
				ecs.diffuse_colors [e] = vec4 (1.0f, 0.1f, 0.1f, 1.0f);
				add_danger_zone = false;
			}
			else {
				ecs.diffuse_colors [e] = vec4 (0.005f, 0.228f, 0.047f, 1.0f);
				add_danger_zone = true;
			}
		}
		else {
			ecs.diffuse_colors [e] = vec4 (0.005f, 0.228f, 0.047f, 1.0f);
		}
		
		if (add_danger_zone) {
			auto zone_e = ecs.add_entity ();
			vec2 pounce_vec = scene.pounce_vec.at (old_e);
			
			mat4 pounce_mat (1.0);
			pounce_mat [0][0] = pounce_vec.x;
			pounce_mat [0][1] = pounce_vec.y;
			
			pounce_mat [1][0] = pounce_vec.y;
			pounce_mat [1][1] = -pounce_vec.x;
			
			ecs.rigid_mats [zone_e] = scale (translate (mat4 (1.0f), base_pos), vec3 (10.0f * venus.pounce_anim)) * pounce_mat;
			ecs.diffuse_colors [zone_e] = vec4 (1.0f, 0.1f, 0.1f, 0.25f);
			ecs.meshes [zone_e] = (MeshKey)EMesh::DangerZone;
			ecs.textures [zone_e] = (TextureKey)ETexture::White;
			ecs.transparent_z [zone_e] = -100.0f;
			
			transparent.renderables [zone_e];
		}
		
		ecs.rigid_mats [e] = scale (translate (mat4 (1.0f), base_pos + vec3 (0.0f, size.y, 0.0f)), size);
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
