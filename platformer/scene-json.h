#ifndef VG_SCENE_JSON_H
#define VG_SCENE_JSON_H

#include <sstream>

struct SceneEcs;
struct GraphicsEcs;
struct Level;

void to_json (std::stringstream & s, const SceneEcs & ecs);
void to_json (std::stringstream & s, const GraphicsEcs & ecs);
void to_json (std::stringstream & s, const Level & level);

#endif
