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

Logic::Logic () {
	place_carrot (scene, vec3 (3.0f, -3.0f, 0.0f));
	place_carrot (scene, vec3 (3.0f, -5.0f, 0.0f));
	place_carrot (scene, vec3 (3.0f, -4.0f, 0.0f));
	
	place_carrot (scene, vec3 (7.0f, -4.0f, 0.0f));
	
	{
		auto e = scene.add_entity ();
		scene.positions [e] = vec3 (-7.0f, -4.0f, 0.0f);
		scene.velocities [e] = vec3 (0.0f);
		scene.anim_t [e] = 0.0f;
		scene.venuses [e] = Venus ();
		scene.player_input [e] = EcsTrue ();
	}
}

void Logic::step (const InputFrame & input) {
	scene.targeted.clear ();
	
	for (auto pair : scene.player_input) {
		auto e = pair.first;
		
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
			
			vec2 pounce_vec (10.0f, 0.0f);
			
			if (venus.pounce_anim == 1.0f) {
				can_pounce = true;
				// Search right for pouncables
				
				auto closest_victim = -1;
				vec2 closest_vec;
				const auto pounce_range = 10.0f;
				
				// Find closest pouncable
				for (auto pair : scene.pouncables) {
					auto victim_e = pair.first;
					
					if (! pair.second) {
						continue;
					}
					
					auto victim_pos = scene.positions.at (victim_e);
					
					const auto diff = vec2 (victim_pos - pos);
					if (length (diff) > 0.0f && length (diff) <= pounce_range && dot (normalize (diff), vec2 (1.0, 0.0)) >= 0.707f) {
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
			}
			
			if (input.buttons [(int)InputButton::Pounce] && pos.z == 
			0.0f) 
			{
				venus.pounce_anim += 2.0f / 60.0f;
				can_move = false;
			}
			else {
				if (venus.pounce_anim == 1.0f) {
					// Pounce!
					vec2 pounce_xy = pounce_vec * 40.0f / 10.0f / 60.0f;
					scene.velocities [e] = vec3 (pounce_xy.x, pounce_xy.y, 1.0f);
					venus.pounce_anim = 0.0f;
				}
				else {
					venus.pounce_anim -= 10.0f / 60.0f;
				}
			}
			
			venus.pounce_anim = clamp (venus.pounce_anim, 0.0f, 1.0f);
		}
		
		if (can_move) {
			if (input.taps [(int)InputButton::Left]) {
				scene.positions [e] = pos + vec3 (-1.0f, 0.0f, 0.0f);
			}
			if (input.taps [(int)InputButton::Right]) {
				scene.positions [e] = pos + vec3 (1.0f, 0.0f, 0.0f);
			}
			if (input.taps [(int)InputButton::Up]) {
				scene.positions [e] = pos + vec3 (0.0f, 1.0f, 0.0f);
			}
			if (input.taps [(int)InputButton::Down]) {
				scene.positions [e] = pos + vec3 (0.0f, -1.0f, 0.0f);
			}
		}
		
		const auto vel = scene.velocities.at (e);
		if (pos.z > 0.0f || vel.z > 0.0f) {
			// Mid-pounce
			scene.velocities [e] = vel + vec3 (0.0f, 0.0f, -0.125f);
			scene.positions [e] = pos + scene.velocities.at (e);
			
			// Pounced onto target?
			for (auto pair : scene.pounce_target) {
				Entity pouncee_e = pair.first;
				
				auto pos = scene.positions.at (pouncee_e);
				
				if (length (pos - scene.positions.at (e)) < 0.5f) {
					// Yes
					scene.dead [pouncee_e] = true;
					scene.pouncables [pouncee_e] = false;
				}
			}
		}
		
		bool debug = false;
		if (debug) {
			cerr << "X = " << scene.positions [e].x << endl;
		}
	}
}
