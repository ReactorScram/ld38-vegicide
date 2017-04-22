#include "logic.h"

#include <glm/glm.hpp>

using namespace glm;

Logic::Logic () {
	{
		auto e = scene.add_entity ();
		scene.positions [e] = vec3 (0.0f, 0.0f, 0.0f);
		scene.anim_t [e] = 0.0f;
		scene.carrots [e] = EcsTrue ();
	}
	
	{
		auto e = scene.add_entity ();
		scene.positions [e] = vec3 (0.0f, -4.0f, 0.0f);
		scene.anim_t [e] = 0.0f;
		scene.venuses [e] = Venus ();
		scene.player_input [e] = EcsTrue ();
	}
}

void Logic::step (const InputFrame & input) {
	for (auto pair : scene.player_input) {
		auto e = pair.first;
		
		bool can_move = true;
		bool can_pounce = false;
		
		auto venus_it = scene.venuses.find (e);
		if (venus_it != scene.venuses.end ()) {
			auto & venus = (*venus_it).second;
			
			if (input.buttons [(int)InputButton::Pounce]) {
				venus.pounce_anim += 2.0f / 60.0f;
				can_move = false;
			}
			else {
				venus.pounce_anim -= 10.0f / 60.0f;
			}
			
			venus.pounce_anim = clamp (venus.pounce_anim, 0.0f, 1.0f);
			
			if (venus.pounce_anim == 1.0f) {
				can_pounce = true;
			}
		}
		
		if (can_move) {
			if (input.taps [(int)InputButton::Left]) {
				scene.positions [e] = scene.positions [e] + vec3 (-1.0f, 0.0f, 0.0f);
			}
			if (input.taps [(int)InputButton::Right]) {
				scene.positions [e] = scene.positions [e] + vec3 (1.0f, 0.0f, 0.0f);
			}
			if (input.taps [(int)InputButton::Up]) {
				scene.positions [e] = scene.positions [e] + vec3 (0.0f, 1.0f, 0.0f);
			}
			if (input.taps [(int)InputButton::Down]) {
				scene.positions [e] = scene.positions [e] + vec3 (0.0f, -1.0f, 0.0f);
			}
		}
	}
}
