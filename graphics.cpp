#include "graphics.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "colorado/camera.h"

using namespace Colorado;
using namespace glm;
using namespace std;

const Colorado::TriangleShader * Graphics::current_shader () const {
	return shaders.currentShader ();
}

void Graphics::render_rigid (const GraphicsEcs & ecs, const pair <Entity, EcsTrue> & p, const mat4 & proj_view_mat) {
	auto e = p.first;
	auto model_mat = ecs.rigid_mats.at (e);
	auto color = ecs.diffuse_colors.at (e);
	auto mesh = ecs.meshes.at (e);
	if (ecs.textures.count (e) >= 1) {
		textures.bind (ecs.textures.at (e));
	}
	else {
		glBindTexture (GL_TEXTURE_2D, 0);
	}
	
	current_shader ()->setMvpMatrix (proj_view_mat * model_mat);
	
	auto light_mat = inverse (model_mat);
	auto object_up = light_mat * vec4 (0.0, 1.0, 0.0, 0.0);
	
	// TODO: Cache per-shader
	auto uni_up = current_shader ()->uniformLocation ("up");
	auto uni_color = current_shader ()->uniformLocation ("diffuseColor");
	
	glUniform3f (uni_up, object_up.x, object_up.y, object_up.z);
	glUniform3f (uni_color, color.r, color.g, color.b);
	
	meshes.bind (mesh);
	
	const int floats_per_vert = 3 + 2 + 3 + 4;
	const int stride = sizeof (GLfloat) * floats_per_vert;
	
	glVertexAttribPointer (current_shader ()->vertPosAttribute, 3, GL_FLOAT, false, stride, (char *)nullptr + 0);
	glVertexAttribPointer (current_shader ()->vertNormAttribute, 3, GL_FLOAT, false, stride, (char *)nullptr + sizeof (GLfloat) * (3 + 2));
	glVertexAttribPointer (current_shader ()->vertTexCoordAttribute, 2, GL_FLOAT, false, stride, (char *)nullptr + sizeof (GLfloat) * 3);
	
	meshes.currentMesh ()->renderPlacementIndexed (0);
}

void Graphics::render_pass (const GraphicsEcs & ecs, const ScreenOptions & screen_opts, const pair <Entity, Pass> & p)
{
	shaders.bind (p.second.shader);
	
	ae.enableAttributes (attrib_set);
	
	Camera camera;
	camera.fov = 0.25;
	auto proj_mat = camera.generateProjectionMatrix (screen_opts.width, screen_opts.height);
	
	auto view_mat = mat4 (1.0f);
	view_mat = rotate (rotate (translate (view_mat, vec3 (-0.0f, 0.5f, -15.0f)), radians (20.0f), vec3 (1.0f, 0.0f, 0.0f)), radians (30.0f), vec3 (0.0f, 1.0f, 0.0f));
	
	auto proj_view_mat = proj_mat * view_mat;
	
	// Assuming they are all rigid
	for (const auto pair: p.second.renderables) {
		render_rigid (ecs, pair, proj_view_mat);
	}
}

void Graphics::render (const GraphicsEcs & ecs, const ScreenOptions & screen_opts) {
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (const auto pass_pair: ecs.passes) {
		render_pass (ecs, screen_opts, pass_pair);
	}
	
	Colorado::swapBuffers ();
}
