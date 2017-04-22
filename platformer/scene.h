#ifndef VEGICIDE_SCENE_H
#define VEGICIDE_SCENE_H

#include <glm/glm.hpp>

// TODO: Move ECS out of graphics
#include "graphics/ecs.h"

struct SceneEcs : Ecs {
	Components <glm::vec2> positions;
	Components <float> anim_t;
	Components <EcsTrue> carrots;
	Components <EcsTrue> venuses;
	Components <EcsTrue> player_input;
};

#endif
