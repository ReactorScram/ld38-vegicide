#include <unordered_set>

#include <SDL/SDL.h>

#include "colorado/attribute-enabler.h"
#include "colorado/game.h"
#include "colorado/fixed-timestep.h"
#include "colorado/gl.h"
#include "colorado/image-to-texture.h"
#include "colorado/screen-options.h"
#include "colorado/triangle-shader.h"
#include "terf/terf.h"

#include "mesh-binder.h"
#include "shader-binder.h"

using namespace std;
using namespace Colorado;

struct Texture {
	GLuint handle;
	
	Texture (const Terf::Archive & terf, std::string);
	~Texture ();
	
	void bind () const;
};

Texture::Texture (const Terf::Archive & terf, std::string fn) :
	handle (0)
{
	autoImageToTexture (terf.lookupFile (fn), handle, 4);
}

Texture::~Texture () {
	if (handle > 0) {
		glDeleteTextures (1, &handle);
	}
}

void Texture::bind () const {
	glBindTexture (GL_TEXTURE_2D, handle);
}

enum class EMesh {
	Square,
};

enum class EShader {
	Debug,
};

int main () {
	ScreenOptions screen_opts;
	screen_opts.fullscreen = false;
	screen_opts.width = 960;
	screen_opts.height = 960;
	
	Colorado::Game game (screen_opts);
	SDL_WM_SetCaption ("Colorado Hexture Map", nullptr);
	
	GLeeInit ();
	
	Terf::Archive terf ("rom.tar", "rom.tar.index");
	terf.enableTerfLookup = false;
	
	FixedTimestep timestep (60, 1);
	auto last_frame_time = SDL_GetTicks ();
	
	//glEnable (GL_DEPTH_TEST);
	//glEnable (GL_CULL_FACE);
	glEnable (GL_TEXTURE_2D);
	glFrontFace (GL_CW);
	
	bool running = true;
	
	ShaderBinder <EShader> shaders;
	shaders.addShader (EShader::Debug, newShader (terf, "shader.vert", "shader.frag"));
	
	shaders.bind (EShader::Debug);
	
	unordered_set <int> attrib_set;
	attrib_set.insert (shaders.currentShader ()->vertPosAttribute);
	
	AttributeEnabler ae;
	
	Texture texture (terf, "hexture/noise.png");
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	MeshBinder meshes;
	{
		Mesh * square = new Mesh ();
		square->loadIqm (terf.lookupFile ("square.iqm"));
		meshes.meshes ["square"] = unique_ptr <const Mesh> (square);
	}
	
	while (running) {
		SDL_Event ev;
		
		while (SDL_PollEvent (&ev)) {
			if (ev.type == SDL_QUIT) {
				running = false;
			}
		}
		
		auto frame_time = SDL_GetTicks ();
		auto numSteps = timestep.step (frame_time - last_frame_time);
		last_frame_time = frame_time;
		
		for (int i = 0; i < numSteps; i++) {
			// Step
		}
		
		// Render
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shaders.bind (EShader::Debug);
		
		ae.enableAttributes (attrib_set);
		
		meshes.bind ("square");
		texture.bind ();
		
		const int floats_per_vert = 3 + 2 + 3 + 4;
		const int stride = sizeof (GLfloat) * floats_per_vert;
		
		glVertexAttribPointer (shaders.currentShader ()->vertPosAttribute, 3, GL_FLOAT, false, stride, (char *)nullptr + 0);
		
		meshes.currentMesh ()->renderPlacementIndexed (0);
		
		Colorado::swapBuffers ();
	}
	
	return 0;
}
