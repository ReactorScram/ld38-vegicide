#ifndef VEGICIDE_ANIM_H
#define VEGICIDE_ANIM_H

#include <glm/glm.hpp>

#include "scene.h"
#include "graphics/graphics.h"

namespace Colorado {
struct ScreenOptions;
}

struct Level;

ResourceTable make_resource_table ();

GraphicsEcs animate_title (long frames, float curtain_t, const Colorado::ScreenOptions & screen_opts);
GraphicsEcs animate_vegicide (const SceneEcs & ecs, const Level & level, long frames, const Colorado::ScreenOptions & screen_opts);

#endif
