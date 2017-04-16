#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "colorado/attribute-enabler.h"
#include "colorado/screen-options.h"

#include "ecs.h"
#include "mesh-binder.h"
#include "shader-binder.h"
#include "texture-binder.h"

enum class EShader {
	Debug,
};

struct Graphics {
	ShaderBinder shaders;
	Colorado::AttributeEnabler ae;
	std::unordered_set <int> attrib_set;
	TextureBinder textures;
	MeshBinder meshes;
	
	const Colorado::TriangleShader * current_shader () const;
	
	void render (const GraphicsEcs & ecs, const Colorado::ScreenOptions & screen_opts);
};

#endif
