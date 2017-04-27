#ifndef SKYBOX_H
#define SKYBOX_H

#include <glm/glm.hpp>

#include "colorado/dllspec.h"
#include "colorado/gl.h"

namespace Colorado {
enum SkyBoxDirection {
	SKY_FRONT = 0,
	SKY_LEFT,
	SKY_BACK,
	SKY_RIGHT,
	SKY_TOP,		// Up and down aren't implemented yet
	SKY_BOTTOM,
	SKY_COUNT
};

struct COLORADO_DLLSPEC SkyPlane {
	glm::mat4 matrix;
	GLuint texture;
	
	SkyPlane ();
	SkyPlane (glm::mat4);
};

struct COLORADO_DLLSPEC SkyBox {
	SkyPlane planes [6];
	
	SkyBox ();
};
}

#endif // SKYBOX_H
