#include "animation.h"
#include "scene.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "gl-consts.h"

#include "level.h"

using namespace Colorado;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

using glm::abs;
using glm::mix;
using glm::mod;
using glm::normalize;
using glm::translate;
using glm::rotate;
using glm::scale;
using glm::sin;
using std::cerr;
using std::endl;

vec3 two2three (vec3 v) {
	return vec3 (v.x, v.y - v.z, 0.0f);
}

void SceneEcs::play_sound (ESound s) {
	audio_frame.sounds [(int)s] = true;
}

Entity add_sprite (GraphicsEcs & ecs, const vec3 & pos, const vec3 & size, const vec4 & color, ETexture texture) 
{
	auto e = ecs.add_entity ();
	
	ecs.rigid_mats [e] = scale (translate (mat4 (1.0f), two2three (pos)), size * vec3 (1.0f, -1.0f, 1.0f));
	ecs.diffuse_colors [e] = color;
	ecs.meshes [e] = (MeshKey)EMesh::Square;
	ecs.textures [e] = (TextureKey)texture;
	ecs.transparent_z [e] = -pos.y;
	
	return e;
}

float shake (float rads, float hz, float strength) {
	return strength * sin (rads * hz);
}

// Curtain is normalized 0 to 1
void curtain (GraphicsEcs & ecs, float t) {
	float smooth_t = (1 - t) * (1 - t);
	
	GlState opaque_state;
	opaque_state.bools [VEGL_DEPTH_TEST] = false;
	opaque_state.bools [VEGL_CULL_FACE] = false;
	
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
GraphicsEcs animate_title (long frames, float curtain_t, float /*aspect*/)
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
	opaque_state.bools [VEGL_DEPTH_TEST] = false;
	opaque_state.bools [VEGL_CULL_FACE] = false;
	
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

struct EnemyBase {
	Entity sprite;
	Entity shadow;
};

EnemyBase animate_carrot (GraphicsEcs & ecs, const SceneEcs & scene, Entity old_e, long frames) 
{
	const float t = frames * 2.0 * 3.1415926535 / 60.0f;
	vec3 base_pos = scene.positions.at (old_e);
	
	vec4 shadow_color (vec3 (0.5f), 1.0f);
	
	bool dead = false;
	{
		auto dead_it = scene.dead.find (old_e);
		dead = dead_it != scene.dead.end () && (*dead_it).second;
	}
	
	float jump_ofs = abs (sin (t + old_e));
	if (dead) {
		jump_ofs = 0.0f;
	}
	vec3 jump (0.0f, 0.0f, 1.0f + jump_ofs);
	vec3 size (1.0f);
	auto tex = ETexture::Carrot;
	vec4 base_color (1.0f);
	
	vec4 blood_color = shadow_color;
	float shadow_scale = glm::max (0.0f, 0.25f / (jump_ofs + 1.0f));
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
	
	EnemyBase eb;
	
	eb.sprite = e;
	
	{
		auto s = add_sprite (ecs, base_pos, vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale), blood_color, shadow_tex);
		
		if (dead) {
			ecs.rigid_mats [s] = rotate (scale (translate (mat4 (1.0f), two2three (base_pos)), vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale)), (float)old_e, vec3 (0.0f, 0.0f, 1.0f));
		}
		
		eb.shadow = s;
	}
	
	return eb;
}

EnemyBase animate_crabapple (GraphicsEcs & ecs, const SceneEcs & scene, Entity old_e, long frames) 
{
	const float t = frames * 2.0 * 3.1415926535 / 60.0f;
	vec3 base_pos = scene.positions.at (old_e);
	
	vec4 shadow_color (vec3 (0.5f), 1.0f);
	
	bool dead = false;
	{
		auto dead_it = scene.dead.find (old_e);
		dead = dead_it != scene.dead.end () && (*dead_it).second;
	}
	
	float jump_ofs = abs (sin (t));
	if (dead || ! get_component (scene.ai_active, old_e, false)) {
		jump_ofs = 0.0f;
	}
	vec3 jump (0.0f, 0.0f, 0.5f + jump_ofs);
	vec3 size (1.0f);
	auto tex = ETexture::CrabApple;
	vec4 base_color (1.0f);
	
	vec4 blood_color = shadow_color;
	float shadow_scale = glm::max (0.0f, 0.25f / (jump_ofs + 1.0f));
	auto shadow_tex = ETexture::Shadow;
	
	if (dead) {
		tex = ETexture::CrabAppleDead;
		//jump = vec3 (0.0f);
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
	
	EnemyBase eb;
	
	eb.sprite = e;
	
	{
		auto s = add_sprite (ecs, base_pos, vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale), blood_color, shadow_tex);
		
		if (dead) {
			ecs.rigid_mats [s] = rotate (scale (translate (mat4 (1.0f), two2three (base_pos)), vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale)), (float)old_e, vec3 (0.0f, 0.0f, 1.0f));
		}
		
		eb.shadow = s;
	}
	
	return eb;
}

EnemyBase animate_beetnik (GraphicsEcs & ecs, const SceneEcs & scene, Entity old_e, long frames) 
{
	const float t = frames * 2.0 * 3.1415926535 / 60.0f;
	vec3 base_pos = scene.positions.at (old_e);
	
	vec4 shadow_color (vec3 (0.5f), 1.0f);
	
	bool dead = false;
	{
		auto dead_it = scene.dead.find (old_e);
		dead = dead_it != scene.dead.end () && (*dead_it).second;
	}
	
	float jump_ofs = abs (sin (t));
	if (dead || ! get_component (scene.ai_active, old_e, false)) {
		jump_ofs = 0.0f;
	}
	vec3 jump (0.0f, 0.0f, 0.5f + jump_ofs);
	vec3 size (1.0f);
	auto tex = ETexture::Beet;
	vec4 base_color (1.0f);
	
	vec4 blood_color = shadow_color;
	float shadow_scale = glm::max (0.0f, 0.25f / (jump_ofs + 1.0f));
	auto shadow_tex = ETexture::Shadow;
	
	if (dead) {
		tex = ETexture::BeetDead;
		//jump = vec3 (0.0f);
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
	
	EnemyBase eb;
	
	eb.sprite = e;
	
	{
		auto s = add_sprite (ecs, base_pos, vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale), blood_color, shadow_tex);
		
		if (dead) {
			ecs.rigid_mats [s] = rotate (scale (translate (mat4 (1.0f), two2three (base_pos)), vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale)), (float)old_e, vec3 (0.0f, 0.0f, 1.0f));
		}
		
		eb.shadow = s;
	}
	
	return eb;
}

GraphicsEcs animate_vegicide (const SceneEcs & scene, const Level &, long frames, float aspect) 
{
	const float t = frames * 2.0 * 3.1415926535 / 60.0f;
	float screen_shake = scene.screenshake_t;
	bool flash = (frames % 16) < 8;
	
	float noise = 0;
	if (screen_shake > 0.0f) {
		noise = shake (t, 20, 2) + shake (t, 14, 3) + shake (t, 8, 5);
	}
	
	vec3 camera (floor (scene.camera.x + noise), floor (scene.camera.y), 0);
	
	//vec3 camera (0.0f);
	
	const float mob_scale = 1.0f / 15.0f;
	
	const auto proj_mat = glm::ortho (0.0f, aspect, 1.0f, 0.0f);
	
	const auto view_mat = translate (scale (mat4 (1.0f), vec3 (mob_scale)), -camera / 32.0f);
	//auto camera_pos = inverse (view_mat) * vec4 (0.0, 0.0, 0.0, 1.0);
	//auto camera_forward = inverse (view_mat) * vec4 (0.0, 0.0, -1.0, 0.0);
	
	const auto proj_view_mat = proj_mat * view_mat;
	
	GlState opaque_state;
	opaque_state.bools [VEGL_DEPTH_TEST] = false;
	opaque_state.bools [VEGL_CULL_FACE] = false;
	
	GlState transparent_state;
	transparent_state.bools [VEGL_DEPTH_TEST] = false;
	transparent_state.bools [VEGL_BLEND] = true;
	transparent_state.bools [VEGL_CULL_FACE] = false;
	transparent_state.blendFunc [0] = VEGL_SRC_ALPHA;
	transparent_state.blendFunc [1] = VEGL_ONE_MINUS_SRC_ALPHA;
	
	GlState shadow_state;
	shadow_state.bools [VEGL_DEPTH_TEST] = false;
	shadow_state.bools [VEGL_BLEND] = true;
	shadow_state.bools [VEGL_CULL_FACE] = false;
	shadow_state.blendFunc [0] = VEGL_DST_COLOR;
	shadow_state.blendFunc [1] = VEGL_ZERO;
	
	Pass opaque;
	opaque.shader = (ShaderKey)EShader::Opaque;
	opaque.gl_state = opaque_state;
	opaque.proj_view_mat = glm::ortho (0.0f, 800.0f, 480.0f, 0.0f) * translate (mat4 (1.0f), -camera);
	
	Pass shadows;
	shadows.shader = (ShaderKey)EShader::Shadow;
	shadows.gl_state = shadow_state;
	shadows.proj_view_mat = proj_view_mat;
	
	Pass transparent;
	transparent.shader = (ShaderKey)EShader::Opaque;
	transparent.gl_state = transparent_state;
	transparent.proj_view_mat = proj_view_mat;
	
	Pass vignette;
	vignette.shader = (ShaderKey)EShader::Opaque;
	vignette.gl_state = transparent_state;
	vignette.proj_view_mat = scale (mat4 (1.0f), vec3 (1.0f, -1.0f, 0.0f));
	
	GraphicsEcs ecs;
	
	// Level 
	if (true) {
		auto e = ecs.add_entity ();
		
		ecs.rigid_mats [e] = scale (mat4 (1.0f), vec3 (32.0f, 32.0f, 0.0f));
		ecs.diffuse_colors [e] = vec4 (1.0f);
		ecs.meshes [e] = (MeshKey)EMesh::Level;
		ecs.textures [e] = (TextureKey)ETexture::Tiles;
		ecs.transparent_z [e] = 0.0f;
		
		opaque.renderables [e];
	}
	
	vec4 shadow_color (vec3 (0.5f), 1.0f);
	
	// Pumpking
	for (auto pair : scene.pumpkings) {
		auto old_e = pair.first;
		
		vec3 base_pos = scene.positions.at (old_e);
		
		bool dead = false;
		{
			auto dead_it = scene.dead.find (old_e);
			dead = dead_it != scene.dead.end () && (*dead_it).second;
		}
		
		float breathe = 0.2f * sin (t * 0.25f);
		auto tex = ETexture::Pumpking;
		vec4 base_color (1.0f);
		
		{
			auto it = scene.targeted.find (old_e);
			if (it != scene.targeted.end () && (*it).second) 
			{
				base_color = vec4 (1.0f, 0.0f, 0.0f, 1.0f);
			}
		}
		{
			auto it = scene.damage_flash.find (old_e);
			if (flash && it != scene.damage_flash.end () && (*it).second > frames) 
			{
				
				base_color = vec4 (1.0f, 0.0f, 0.0f, 1.0f);
				//breathe = 0.3f * sin (t * 0.5f);
			}
		}
		
		vec4 blood_color = shadow_color;
		float shadow_scale = glm::max (0.0f, 2.0f / (base_pos.z + 1.0f));
		auto shadow_tex = ETexture::Shadow;
		
		if (dead) {
			tex = ETexture::PumpkingDead;
			blood_color = vec4 (96.0f / 256, 13.0f / 256, 70.0f / 256, 1.0f);
			shadow_scale *= 2.0f;
			shadow_tex = ETexture::Blood;
			breathe = 0.0f;
		}
		
		vec3 size (2.0f + breathe, 2.0f - breathe, 0.0f);
		
		auto e = add_sprite (ecs, base_pos + vec3 (0.0f, 0.0f, -0.5f + size.y), size, base_color, tex);
		
		transparent.renderables [e];
		
		{
			auto s = add_sprite (ecs, base_pos, vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale), blood_color, shadow_tex);
			
			if (dead) {
				ecs.rigid_mats [s] = rotate (scale (translate (mat4 (1.0f), two2three (base_pos)), vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale)), (float)old_e, vec3 (0.0f, 0.0f, 1.0f));
			}
			
			shadows.renderables [s];
		}
	}
	
	// Carrot
	for (auto pair : scene.carrots) {
		auto old_e = pair.first;
		
		EnemyBase eb = animate_carrot (ecs, scene, old_e, frames);
		
		transparent.renderables [eb.sprite];
		shadows.renderables [eb.shadow];
	}
	
	// Beetniks
	for (auto pair : scene.beetniks) {
		auto old_e = pair.first;
		
		EnemyBase eb = animate_beetnik (ecs, scene, old_e, frames);
		
		transparent.renderables [eb.sprite];
		shadows.renderables [eb.shadow];
	}
	
	// Crab-apples
	for (auto pair : scene.crabapples) {
		auto old_e = pair.first;
		
		EnemyBase eb = animate_crabapple (ecs, scene, old_e, frames);
		
		transparent.renderables [eb.sprite];
		shadows.renderables [eb.shadow];
	}
	
	for (auto pair : scene.eggs) {
		auto old_e = pair.first;
		
		vec3 base_pos = scene.positions.at (old_e);
		
		vec3 size (1.0f);
		auto tex = ETexture::Egg;
		vec4 base_color (1.0f);
		
		auto e = add_sprite (ecs, base_pos, size, base_color, tex);
		transparent.renderables [e];
	}
	
	// Venus
	int venus_health = 0;
	for (auto pair : scene.venuses) {
		auto old_e = pair.first;
		auto venus = pair.second;
		venus_health = scene.hp.at (old_e);
		auto e = ecs.add_entity ();
		
		auto pos = scene.positions.at (old_e);
		
		vec3 base_pos = pos;
		//cerr << "venus: (" << base_pos.x << ", " << base_pos.y << ")" << endl;
		
		{
			float shadow_scale = glm::max (0.0f, 0.5f / (pos.z + 1.0f));
			auto s = add_sprite (ecs, pos * vec3 (1.0f, 1.0f, 0.0f), vec3 (shadow_scale, 0.5f * shadow_scale, shadow_scale), shadow_color, ETexture::Shadow);
			
			shadows.renderables [s];
		}
		
		vec3 breathe_size (1.0f - 0.125f * sin (t), 1.0f + 0.125f * sin (t), 1.0f);
		
		vec3 tense_size (1.0f + 0.125f, 0.75f, 1.0f);
		
		vec3 size = mix (breathe_size, tense_size, venus.pounce_anim);
		
		bool add_danger_zone = venus.pounce_anim > 1.0f / 10.0f;
		
		ecs.diffuse_colors [e] = vec4 (1.0f);
		
		if (flash && get_component (scene.damage_flash, old_e, (int64_t)0) >= frames) {
			ecs.diffuse_colors [e] = vec4 (1.0f, 0.1f, 0.1f, 1.0f);
		}
		
		if (venus.pounce_anim == 1.0f) {
			add_danger_zone = flash;
		}
		else {
			//ecs.diffuse_colors [e] = vec4 (1.0f);
		}
		
		if (add_danger_zone) {
			auto zone_e = ecs.add_entity ();
			vec2 pounce_vec = scene.pounce_vec.at (old_e);
			
			mat4 pounce_mat (1.0);
			pounce_mat [0][0] = pounce_vec.x;
			pounce_mat [0][1] = pounce_vec.y;
			
			pounce_mat [1][0] = pounce_vec.y;
			pounce_mat [1][1] = -pounce_vec.x;
			
			ecs.rigid_mats [zone_e] = scale (translate (mat4 (1.0f), two2three (base_pos)), vec3 (venus.pounce_range * venus.pounce_anim)) * pounce_mat;
			ecs.diffuse_colors [zone_e] = vec4 (1.0f, 0.1f, 0.1f, 0.25f);
			ecs.meshes [zone_e] = (MeshKey)EMesh::DangerZone;
			ecs.textures [zone_e] = (TextureKey)ETexture::White;
			ecs.transparent_z [zone_e] = -100.0f;
			
			transparent.renderables [zone_e];
		}
		
		auto tex = ETexture::Venus;
		
		if (get_component (scene.dead, old_e, false)) {
			tex = ETexture::VenusDead;
			size = tense_size;
		}
		
		ecs.rigid_mats [e] = scale (translate (mat4 (1.0f), two2three (base_pos) + vec3 (0.0f, -size.y, 0.0f)), size * vec3 (1.0f, -1.0f, 1.0f));
		ecs.meshes [e] = (MeshKey)EMesh::Square;
		ecs.textures [e] = (TextureKey)tex;
		ecs.transparent_z [e] = -base_pos.y;
		
		transparent.renderables [e];
	}
	
	bool player_victory = true;
	// If all pumpkings are dead you win
	for (auto pair : scene.pumpkings) {
		if (! get_component (scene.dead, pair.first, false)) {
			player_victory = false;
		}
	}
	
	if (player_victory) {
		auto tex = ETexture::Victory;
		vec4 base_color (1.0f);
		
		auto e = add_sprite (ecs, vec3 (0.0f), vec3 (1.0f), base_color, tex);
		vignette.renderables [e];
	}
	else if (frames % 60 >= 30 || venus_health == 0) {
		auto tex = ETexture::Vignette;
		vec4 base_color (1.0f, 0.1f, 0.1f, 1.0f - (venus_health / 4.0f));
		
		auto e = add_sprite (ecs, vec3 (0.0f), vec3 (1.0f), base_color, tex);
		vignette.renderables [e];
	}
	
	ecs.passes.push_back (opaque);
	ecs.passes.push_back (shadows);
	ecs.passes.push_back (transparent);
	ecs.passes.push_back (vignette);
	
	return ecs;
}
