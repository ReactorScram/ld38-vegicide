#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "colorado/attribute-enabler.h"
#include "colorado/screen-options.h"

#include "ecs.h"

struct Graphics {
	ShaderBinder shaders;
	Colorado::AttributeEnabler ae;
	std::unordered_set <int> attrib_set;
	TextureBinder textures;
	MeshBinder meshes;
	
	const Colorado::TriangleShader * current_shader () const;
	
	void render_rigid (const GraphicsEcs & ecs, const std::pair <Entity, EcsTrue> & p, const glm::mat4 & proj_view_mat);
	
	void render_pass (const GraphicsEcs & ecs, const Colorado::ScreenOptions & screen_opts, const std::pair <Entity, Pass> & p);
	
	void render (const GraphicsEcs & ecs, const Colorado::ScreenOptions & screen_opts);
};

#endif
