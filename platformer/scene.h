#ifndef VEGICIDE_SCENE_H
#define VEGICIDE_SCENE_H

#include <glm/glm.hpp>

#include "graphics/ecs.h"

namespace Colorado {
struct ScreenOptions;
}

struct SceneEcs : Ecs {
	Components <glm::vec2> positions;
	Components <float> anim_t;
	Components <EcsTrue> carrots;
	Components <EcsTrue> venuses;
	Components <EcsTrue> player_input;
};

enum class ETexture {
	BenchAo,
	Carrot,
	Farm,
	Gear8,
	Gear32,
	Lenna,
	Noise,
	Shadow,
	White,
};

enum class EMesh {
	Bench,
	BenchUpper,
	Gear8,
	Gear32,
	Square,
	Venus,
};

enum class EShader {
	Opaque,
	Particle,
	Shadow,
};

GraphicsEcs animate_vegicide (const SceneEcs & ecs, long frames, const Colorado::ScreenOptions & screen_opts);

#endif
