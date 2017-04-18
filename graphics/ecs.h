#include <map>
#include <stdint.h>

#include <glm/glm.hpp>

#include "gl-state-tracker.h"
#include "mesh-binder.h"
#include "shader-binder.h"
#include "texture-binder.h"

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

struct Pass {
	GlState gl_state;
	ShaderKey shader;
	// All rigid right now but may have further typing
	Components <EcsTrue> renderables;
	
	// Dumb hack to try to get shadows working
	bool clear_depth_before;
};

struct GraphicsEcs : Ecs {
	std::vector <Pass> passes;
	Components <glm::mat4> rigid_mats;
	Components <glm::vec3> diffuse_colors;
	Components <MeshKey> meshes;
	Components <TextureKey> textures;
};
