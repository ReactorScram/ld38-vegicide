#ifndef VEGICIDE_SCENE_H
#define VEGICIDE_SCENE_H

#include <glm/glm.hpp>

// TODO: Move ECS out of graphics
#include "graphics/ecs.h"

struct Venus {
	float pounce_anim;
};

struct SceneEcs : Ecs {
	Components <glm::vec3> positions;
	Components <glm::vec3> velocities;
	Components <bool> targeted;
	Components <float> anim_t;
	Components <bool> pouncables;
	Components <EcsTrue> pounce_target;
	Components <glm::vec2> pounce_vec;
	Components <bool> dead;
	Components <int> hp;
	Components <EcsTrue> carrots;
	Components <EcsTrue> pumpkings;
	Components <Venus> venuses;
	Components <EcsTrue> player_input;
	
	glm::vec2 camera;
	glm::vec2 last_walk;
	float screenshake_t;
};

#endif
