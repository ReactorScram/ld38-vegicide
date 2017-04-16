#include <map>
#include <stdint.h>

#include <glm/glm.hpp>

#include "mesh-binder.h"

typedef int32_t Entity;

template <typename T>
using Components = std::map <Entity, T>;

struct EcsTrue {
	//
};

struct Ecs {
	Entity next_entity = 0;
	Entity add_entity ();
};

struct GraphicsEcs : Ecs {
	Components <EcsTrue> opaque_pass;
	Components <glm::mat4> rigid_mats;
	Components <glm::vec3> diffuse_colors;
	Components <MeshKey> meshes;
};
