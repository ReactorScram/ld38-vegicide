#include "logic.h"

#include <glm/glm.hpp>

using namespace glm;

Logic::Logic () {
	{
		auto e = scene.add_entity ();
		scene.positions [e] = vec2 (0.0f, 0.0f);
		scene.anim_t [e] = 0.0f;
		scene.carrots [e] = EcsTrue ();
	}
	
	{
		auto e = scene.add_entity ();
		scene.positions [e] = vec2 (0.0f, -4.0f);
		scene.anim_t [e] = 0.0f;
		scene.venuses [e] = EcsTrue ();
		scene.player_input [e] = EcsTrue ();
	}
}

void Logic::step (const InputFrame & input) {
	for (auto pair : scene.player_input) {
		auto e = pair.first;
		
		if (input.taps [(int)InputButton::Left]) {
			scene.positions [e] = scene.positions [e] + vec2 (-1.0f, 0.0f);
		}
		if (input.taps [(int)InputButton::Right]) {
			scene.positions [e] = scene.positions [e] + vec2 (1.0f, 0.0f);
		}
		if (input.taps [(int)InputButton::Up]) {
			scene.positions [e] = scene.positions [e] + vec2 (0.0f, 1.0f);
		}
		if (input.taps [(int)InputButton::Down]) {
			scene.positions [e] = scene.positions [e] + vec2 (0.0f, -1.0f);
		}
	}
}
