#ifndef VEGICIDE_ECS_H
#define VEGICIDE_ECS_H

#include <map>
#include <stdint.h>
#include <vector>

#include <glm/glm.hpp>

#include "../graphics/gl-state.h"

typedef int32_t TextureKey;
typedef int32_t ShaderKey;
typedef int32_t MeshKey;

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
	std::vector <Entity> particle_arrays;
	
	glm::mat4 proj_view_mat;
};

struct Particle {
	glm::mat4 mat;
	glm::vec4 color;
};

struct ParticleArray {
	std::vector <Particle> particles;
	MeshKey mesh;
	TextureKey texture;
};

template <typename T>
T get_component (const Components <T> & comps, Entity e, T default_comp) 
{
	auto it = comps.find (e);
	if (it != comps.end ()) {
		return (*it).second;
	}
	else {
		return default_comp;
	}
}

struct GraphicsEcs : Ecs {
	std::vector <Pass> passes;
	
	Components <float> transparent_z;
	Components <glm::mat4> rigid_mats;
	Components <glm::vec4> diffuse_colors;
	Components <MeshKey> meshes;
	Components <TextureKey> textures;
	Components <ParticleArray> particle_arrays;
};

#endif
