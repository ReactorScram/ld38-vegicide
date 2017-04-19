#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <map>
#include <string>

#include "colorado/attribute-enabler.h"
#include "colorado/screen-options.h"

#include "ecs.h"

struct ShaderFiles {
	std::string vert_fn;
	std::string frag_fn;
};

struct ResourceTable {
	std::map <ShaderKey, ShaderFiles> shaders;
	std::map <TextureKey, std::string> textures;
	std::map <MeshKey, std::string> meshes;
};

namespace Terf {
class Archive;	
}

struct Graphics {
	ShaderBinder shaders;
	Colorado::AttributeEnabler ae;
	std::map <ShaderKey, std::unordered_set <int> > attrib_sets;
	TextureBinder textures;
	MeshBinder meshes;
	GlStateTracker state_tracker;
	
	Graphics ();
	
	void load (const Terf::Archive & terf, const ResourceTable & rc);
	
	const Colorado::TriangleShader * current_shader () const;
	
	void render_rigid (const GraphicsEcs & ecs, const std::pair <Entity, EcsTrue> & p, const glm::mat4 & proj_view_mat);
	
	void render_particle_array (const GraphicsEcs & ecs, const Entity e, const glm::mat4 & proj_view_mat);
	
	void render_pass (const GraphicsEcs & ecs, const Colorado::ScreenOptions & screen_opts, const Pass & pass);
	
	void render (const GraphicsEcs & ecs, const Colorado::ScreenOptions & screen_opts);
};

#endif
