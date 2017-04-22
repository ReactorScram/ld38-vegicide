#ifndef VEGICIDE_ANIM_H
#define VEGICIDE_ANIM_H

#include <glm/glm.hpp>

#include "scene.h"
#include "graphics/graphics.h"

namespace Colorado {
struct ScreenOptions;
}

enum class ETexture {
	BenchAo,
	Carrot,
	CarrotDead,
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

ResourceTable make_resource_table ();
GraphicsEcs animate_vegicide (const SceneEcs & ecs, long frames, const Colorado::ScreenOptions & screen_opts);

#endif
