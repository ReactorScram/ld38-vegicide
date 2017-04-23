#ifndef VEGICIDE_ANIM_H
#define VEGICIDE_ANIM_H

#include <glm/glm.hpp>

#include "scene.h"
#include "graphics/graphics.h"

namespace Colorado {
struct ScreenOptions;
}

struct Level;

enum class ETexture {
	Blood,
	Carrot,
	CarrotDead,
	Farm,
	Shadow,
	Tiles,
	White,
};

enum class EMesh {
	DangerZone,
	Level,
	Square,
	Venus,
};

enum class EShader {
	Opaque,
	Particle,
	Shadow,
};

ResourceTable make_resource_table ();
GraphicsEcs animate_vegicide (const SceneEcs & ecs, const Level & level, long frames, const Colorado::ScreenOptions & screen_opts);

#endif
