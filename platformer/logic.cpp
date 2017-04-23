#include "logic.h"

#include <iostream>

#include <glm/glm.hpp>

using namespace glm;
using namespace std;

void place_carrot (SceneEcs & scene, const vec3 & pos) {
	auto e = scene.add_entity ();
	scene.positions [e] = pos;
	scene.anim_t [e] = 0.0f;
	scene.carrots [e] = EcsTrue ();
	scene.pouncables [e] = true;
}

SceneEcs reset_scene () {
	SceneEcs scene;
	
	scene.screenshake_t = 0.0f;
	
	place_carrot (scene, vec3 (3.0f, -3.0f, 0.0f));
	place_carrot (scene, vec3 (3.0f, -5.0f, 0.0f));
	place_carrot (scene, vec3 (3.0f, -4.0f, 0.0f));
	
	place_carrot (scene, vec3 (7.0f, -4.0f, 0.0f));
	
	place_carrot (scene, vec3 (0.0f, 1.0f, 0.0f));
	
	{
		auto e = scene.add_entity ();
		scene.positions [e] = vec3 (-7.0f, -4.0f, 0.0f);
		scene.velocities [e] = vec3 (0.0f);
		scene.anim_t [e] = 0.0f;
		scene.venuses [e] = Venus ();
		scene.player_input [e] = EcsTrue ();
	}
	
	return scene;
}

void shake_screen (SceneEcs & scene, float time) {
	scene.screenshake_t = glm::max (scene.screenshake_t, time);
}

Logic::Logic (const Level & l) : level (l) {
	scene = reset_scene ();
}

vec2 get_pounce_vec (const InputFrame & input) {
	vec2 pounce_vec (0.0f, 0.0f);
	
	if (input.buttons [(int)InputButton::Left]) {
		pounce_vec.x = -1.0f;
	}
	else if (input.buttons [(int)InputButton::Right]) {
		pounce_vec.x = 1.0f;
	}
	
	if (input.buttons [(int)InputButton::Up]) {
		pounce_vec.y = 1.0f;
	}
	else if (input.buttons [(int)InputButton::Down]) {
		pounce_vec.y = -1.0f;
	}
	
	if (length (pounce_vec) > 1.0f) {
		pounce_vec = normalize (pounce_vec);
	}
	
	return pounce_vec;
}

// Everything from here down is a system I guess yay

vec3 get_walk_pos (const SceneEcs & scene, Entity e, const InputFrame & input) 
{
	auto pos = scene.positions.at (e);
	vec3 target_pos = pos;
	
	if (input.taps [(int)InputButton::Left]) {
		target_pos = pos + vec3 (-1.0f, 0.0f, 0.0f);
	}
	if (input.taps [(int)InputButton::Right]) {
		target_pos = pos + vec3 (1.0f, 0.0f, 0.0f);
	}
	if (input.taps [(int)InputButton::Up]) {
		target_pos = pos + vec3 (0.0f, 1.0f, 0.0f);
	}
	if (input.taps [(int)InputButton::Down]) {
		target_pos = pos + vec3 (0.0f, -1.0f, 0.0f);
	}
	
	bool can_move_there = true;
	
	for (auto pair : scene.pouncables) {
		auto victim_e = pair.first;
		
		if (! pair.second) {
			continue;
		}
		
		auto victim_pos = scene.positions.at (victim_e);
		
		const auto diff = vec2 (victim_pos - target_pos);
		if (length (diff) <= 0.5f) {
			can_move_there = false;
			break;
		}
	}
	
	if (can_move_there) {
		return target_pos;
	}
	else {
		return pos;
	}
}

void player_walk (SceneEcs & scene, Entity e, const vec3 & pos) {
	scene.positions [e] = pos;
}

// Yeah sure why not?
vector <Entity> get_pounce_victims (const SceneEcs & scene, const vec3 & venus_pos) 
{
	vector <Entity> result;
	
	// Pounced onto target?
	for (auto pair : scene.pounce_target) {
		Entity pouncee_e = pair.first;
		
		auto pos = scene.positions.at (pouncee_e);
		
		if (length (pos - venus_pos) < 0.5f) {
			// Yes
			result.push_back (pouncee_e);
		}
	}
	
	return result;
}

void kill_enemies (SceneEcs & scene, const vector <Entity> & victims) 
{
	for (Entity pouncee_e : victims) {
		scene.dead [pouncee_e] = true;
		scene.pouncables [pouncee_e] = false;
		shake_screen (scene, 0.375f);
	}
}

vec3 get_pounce_velocity (const vec2 & pounce_vec, float pounce_range) {
	// Pounce!
	// Default vertical jump
	float jump_power = 1.0f;
	vec2 pounce_xy (0.0f);
	
	if (length (pounce_vec) > 0.0f) {
		jump_power = length (pounce_vec) / pounce_range;
		pounce_xy = normalize (pounce_vec) * 40.0f / 60.0f;
	}
	
	jump_power = glm::max (0.25f / pounce_range, jump_power);
	
	return vec3 (pounce_xy.x, pounce_xy.y, 1.0f * jump_power);
}

void start_pounce (SceneEcs & scene, Entity e, const vec3 & velocity) 
{
	scene.velocities [e] = velocity;
}

Entity get_closest_pouncable (const SceneEcs & scene, const vec3 & pos, float range, const vec2 & pounce_vec) {
	Entity closest_victim = -1;
	vec2 closest_vec = pounce_vec;
	
	// Find closest pouncable
	for (auto pair : scene.pouncables) {
		auto victim_e = pair.first;
		
		if (! pair.second) {
			continue;
		}
		
		auto victim_pos = scene.positions.at (victim_e);
		
		const auto diff = vec2 (victim_pos - pos);
		bool can_pounce_enemy = length (diff) <= range && dot (normalize (diff), pounce_vec) >= 0.707f;
		
		bool already_on_enemy = length (diff) == 0.0f;
		
		if (already_on_enemy || can_pounce_enemy) {
			if (closest_victim == -1 || length (diff) < length (closest_vec)) {
				closest_victim = victim_e;
				closest_vec = diff;
			}
		}
	}
	
	return closest_victim;
}

void apply_venus_input (SceneEcs & scene, Entity e, Venus & venus, const InputFrame & input) 
{
	auto pos = scene.positions.at (e);
	
	bool pounce_button_pressed = input.buttons [(int)InputButton::Pounce];
	bool on_ground = pos.z == 0.0f;
	bool pounce_charged = venus.pounce_anim >= 4.0f / 60.0f;
	
	bool could_pounce = on_ground && pounce_charged;
	const vec2 input_pounce_vec = get_pounce_vec (input);
	
	scene.pounce_vec [e] = input_pounce_vec;
	
	if (on_ground) {
		const auto pounce_range = 10.0f;
		vec2 pounce_vec = input_pounce_vec;
		
		if (could_pounce) {
			// Search for pouncables
			
			const float range = venus.pounce_anim * pounce_range;
			const Entity closest_victim = get_closest_pouncable (scene, pos, range, input_pounce_vec);
			
			scene.pounce_target.clear ();
			
			if (closest_victim >= 0) {
				scene.targeted [closest_victim] = true;
				scene.pounce_target [closest_victim] = EcsTrue ();
				pounce_vec = scene.positions.at (closest_victim) - pos;
			}
			else {
				// The pounce is charged but no enemies are
				// available - Just pounce at the ground
				pounce_vec *= pounce_range * venus.pounce_anim;
			}
		}
		{
			const vec2 pounce_vec_2 = pounce_vec;
			
			if (pounce_button_pressed) 
			{
				venus.pounce_anim += 2.0f / 60.0f;
			}
			else if (pounce_charged) {
				start_pounce (scene, e, get_pounce_velocity (pounce_vec_2, pounce_range));
				venus.pounce_anim = 0.0f;
			}
			else {
				venus.pounce_anim -= 10.0f / 60.0f;
			}
		}
	}
	else {
		venus.pounce_anim = 0.0f;
	}
	
	venus.pounce_anim = clamp (venus.pounce_anim, 0.0f, 1.0f);
}

void apply_player_input (SceneEcs & scene, Entity e, const InputFrame & input) 
{
	auto pos = scene.positions.at (e);
	
	bool can_move = true;
	
	if (pos.z > 0.0f) {
		can_move = false;
	}
	else {
		scene.velocities [e] = vec3 (0.0f);
		pos.x = floor (pos.x + 0.5f);
		pos.y = floor (pos.y + 0.5f);
		pos.z = 0.0f;
		scene.positions [e] = pos;
	}
	
	if (input.buttons [(int)InputButton::Pounce]) {
		can_move = false;
	}
	
	auto venus_it = scene.venuses.find (e);
	if (venus_it != scene.venuses.end ()) {
		auto & venus = (*venus_it).second;
		
		apply_venus_input (scene, e, venus, input);
	}
	
	if (can_move) {
		pos = get_walk_pos (scene, e, input);
		player_walk (scene, e, pos);
	}
	
	const auto vel = scene.velocities.at (e);
	if (pos.z > 0.0f || vel.z > 0.0f) {
		// Mid-pounce
		scene.velocities [e] = vel + vec3 (0.0f, 0.0f, -0.125f);
		pos += scene.velocities.at (e);
		scene.positions [e] = pos;
		
		kill_enemies (scene, get_pounce_victims (scene, pos));
	}
	
	bool debug = false;
	if (debug) {
		cerr << "X = " << scene.positions [e].x << endl;
	}
}

void Logic::step (const InputFrame & input) {
	scene.targeted.clear ();
	
	if (input.taps [(int)InputButton::Reset]) {
		scene = reset_scene ();
	}
	
	for (auto pair : scene.player_input) {
		auto e = pair.first;
		
		apply_player_input (scene, e, input);
	}
	
	scene.screenshake_t = glm::max (0.0f, scene.screenshake_t - 1.0f / 60.0f);
}
