#include "logic.h"

#include <iostream>

#include <glm/glm.hpp>

using namespace glm;
using namespace std;

Logic::Logic () {
	{
		auto e = scene.add_entity ();
		scene.positions [e] = vec3 (3.0f, -4.0f, 0.0f);
		scene.anim_t [e] = 0.0f;
		scene.carrots [e] = EcsTrue ();
		scene.pouncables [e] = true;
	}
	
	{
		auto e = scene.add_entity ();
		scene.positions [e] = vec3 (7.0f, -4.0f, 0.0f);
		scene.anim_t [e] = 0.0f;
		scene.carrots [e] = EcsTrue ();
		scene.pouncables [e] = true;
	}
	
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
			
			float pounce_range = 10.0f;
			
			if (venus.pounce_anim == 1.0f) {
				can_pounce = true;
				// Search right for pouncables
				
				auto closest_victim = -1;
				auto closest_range = pounce_range;
				
				// Find closest pouncable
				for (auto pair : scene.pouncables) {
					auto victim_e = pair.first;
					auto victim_pos = scene.positions.at (victim_e);
					
					auto diff = victim_pos - pos;
					if (diff.x > 0.0f && diff.x <= pounce_range && diff.y == 0.0f) {
						if (closest_victim == -1 || diff.x < closest_range) {
							closest_victim = victim_e;
							closest_range = diff.x;
						}
					}
				}
				
				scene.targeted [closest_victim] = true;
				pounce_range = closest_range;
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
					scene.velocities [e] = vec3 (40.0f * pounce_range / 10.0f / 60.0f, 0.0f, 1.0f);
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
		}
		
		bool debug = false;
		if (debug) {
			cerr << "X = " << scene.positions [e].x << endl;
		}
	}
}
