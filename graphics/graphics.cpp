#include "graphics.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "colorado/camera.h"
#include "colorado/gl.h"
#include "terf/terf.h"

using namespace Colorado;
using namespace glm;
using namespace std;

Graphics::Graphics () {
	GLeeInit ();
}

void Graphics::load (const Terf::Archive & terf, const ResourceTable & rc) {
	for (auto pair: rc.shaders) {
		shaders.addShader (pair.first, newShader (terf, pair.second.vert_fn, pair.second.frag_fn));
		
		shaders.bind (pair.first);
		
		unordered_set <int> attrib_set;
		auto s = current_shader ();
		attrib_set.insert (s->vertPosAttribute);
		attrib_set.insert (s->vertNormAttribute);
		attrib_set.insert (s->vertTexCoordAttribute);
		
		attrib_sets [pair.first] = attrib_set;
	}
	
	for (auto pair: rc.textures) {
		textures.add (pair.first, new Texture (terf, pair.second));
	}
	
	for (auto pair: rc.meshes) {
		meshes.add_iqm (pair.first, terf.lookupFile (pair.second));
	}
}

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

void Graphics::render_pass (const GraphicsEcs & ecs, const ScreenOptions & screen_opts, const Pass & pass)
{
	if (pass.clear_depth_before) {
		//glClear (GL_DEPTH_BUFFER_BIT);
	}
	
	ShaderKey shader_key = pass.shader;
	
	shaders.bind (shader_key);
	
	ae.enableAttributes (attrib_sets.at (shader_key));
	
	state_tracker.Match (pass.gl_state);
	
	Camera camera;
	camera.fov = 0.25;
	auto proj_mat = camera.generateProjectionMatrix (screen_opts.width, screen_opts.height);
	
	auto view_mat = mat4 (1.0f);
	view_mat = rotate (rotate (translate (view_mat, vec3 (-0.0f, 0.5f, -15.0f)), radians (20.0f), vec3 (1.0f, 0.0f, 0.0f)), radians (30.0f), vec3 (0.0f, 1.0f, 0.0f));
	
	auto proj_view_mat = proj_mat * view_mat;
	
	// Assuming they are all rigid
	for (const auto pair: pass.renderables) {
		render_rigid (ecs, pair, proj_view_mat);
	}
}

void Graphics::render (const GraphicsEcs & ecs, const ScreenOptions & screen_opts) {
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	for (const auto & pass: ecs.passes) {
		render_pass (ecs, screen_opts, pass);
	}
	
	Colorado::swapBuffers ();
}
