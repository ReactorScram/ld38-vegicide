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

Logic::Logic () {
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
void player_walk (SceneEcs & scene, Entity e, const InputFrame & input) {
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
		scene.positions [e] = target_pos;
	}
}

void kill_pounce_victims (SceneEcs & scene, const vec3 & venus_pos) {
	// Pounced onto target?
	for (auto pair : scene.pounce_target) {
		Entity pouncee_e = pair.first;
		
		auto pos = scene.positions.at (pouncee_e);
		
		if (length (pos - venus_pos) < 0.5f) {
			// Yes
			scene.dead [pouncee_e] = true;
			scene.pouncables [pouncee_e] = false;
		}
	}
}

void apply_player_input (SceneEcs & scene, Entity e, const InputFrame & input) 
{
	auto pos = scene.positions.at (e);
	
	bool can_move = true;
	bool can_pounce = false;
	
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
	
	auto venus_it = scene.venuses.find (e);
	if (venus_it != scene.venuses.end ()) {
		auto & venus = (*venus_it).second;
		
		vec2 pounce_vec = get_pounce_vec (input);
		
		scene.pounce_vec [e] = pounce_vec;
		
		const auto pounce_range = 10.0f;
		
		if (venus.pounce_anim >= 4.0f / 60.0f) {
			can_pounce = true;
			// Search right for pouncables
			
			auto closest_victim = -1;
			vec2 closest_vec = pounce_vec;
			
			// Find closest pouncable
			for (auto pair : scene.pouncables) {
				auto victim_e = pair.first;
				
				if (! pair.second) {
					continue;
				}
				
				auto victim_pos = scene.positions.at (victim_e);
				
				const auto diff = vec2 (victim_pos - pos);
				bool can_pounce_enemy = length (diff) <= venus.pounce_anim * pounce_range && dot (normalize (diff), pounce_vec) >= 0.707f;
				
				bool already_on_enemy = length (diff) == 0.0f;
				
				if (already_on_enemy || can_pounce_enemy) {
					if (closest_victim == -1 || length (diff) < length (closest_vec)) {
						closest_victim = victim_e;
						closest_vec = diff;
					}
				}
			}
			
			scene.pounce_target.clear ();
			
			if (closest_victim >= 0) {
				scene.targeted [closest_victim] = true;
				scene.pounce_target [closest_victim] = EcsTrue ();
				pounce_vec = closest_vec;
			}
			else {
				pounce_vec *= pounce_range * venus.pounce_anim;
			}
		}
		
		if (input.buttons [(int)InputButton::Pounce] && pos.z == 
		0.0f) 
		{
			venus.pounce_anim += 2.0f / 60.0f;
			can_move = false;
		}
		else {
			if (can_pounce) {
				// Pounce!
				// Default vertical jump
				float jump_power = 1.0f;
				vec2 pounce_xy (0.0f);
				
				if (length (pounce_vec) > 0.0f) {
					jump_power = length (pounce_vec) / pounce_range;
					pounce_xy = normalize (pounce_vec) * 40.0f / 60.0f;
				}
				
				jump_power = glm::max (0.25f / pounce_range, jump_power);
				
				scene.velocities [e] = vec3 (pounce_xy.x, pounce_xy.y, 1.0f * jump_power);
				venus.pounce_anim = 0.0f;
			}
			else {
				venus.pounce_anim -= 10.0f / 60.0f;
			}
		}
		
		venus.pounce_anim = clamp (venus.pounce_anim, 0.0f, 1.0f);
	}
	
	if (can_move) {
		player_walk (scene, e, input);
	}
	
	const auto vel = scene.velocities.at (e);
	if (pos.z > 0.0f || vel.z > 0.0f) {
		// Mid-pounce
		scene.velocities [e] = vel + vec3 (0.0f, 0.0f, -0.125f);
		pos += scene.velocities.at (e);
		scene.positions [e] = pos;
		
		kill_pounce_victims (scene, pos);
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
}
