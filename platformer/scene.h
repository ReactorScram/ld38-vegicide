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
	Components <EcsTrue> carrots;
	Components <Venus> venuses;
	Components <EcsTrue> player_input;
	
	float screenshake_t;
};

#endif
