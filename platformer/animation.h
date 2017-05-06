#ifndef VEGICIDE_ANIM_H
#define VEGICIDE_ANIM_H

#include <glm/glm.hpp>

#include "scene.h"
#include "../ecs/ecs.h"

namespace Colorado {
struct ScreenOptions;
}

struct Level;

GraphicsEcs animate_title (long frames, float curtain_t, const Colorado::ScreenOptions & screen_opts);
GraphicsEcs animate_vegicide (const SceneEcs & ecs, const Level & level, long frames, float aspect);

#endif
