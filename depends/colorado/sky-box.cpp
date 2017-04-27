#include "colorado/sky-box.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Colorado {
SkyPlane::SkyPlane () {
	// Pass
}

SkyPlane::SkyPlane (glm::mat4 m) {
	matrix = m;
	texture = 0;
}

SkyBox::SkyBox () {
	glm::mat4 xRotate (
		1, 0, 0, 0,
		0, 0, -1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1
	);
	
	SkyPlane skyPlanes [SKY_RIGHT + 1] = {
		glm::mat4 (
			-1, 0, 0, 0,
			0, -1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		),
		glm::mat4 (
			0, -1, 0, 0,
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		),
		glm::mat4 (),
		glm::mat4 (
			0, 1, 0, 0,
			-1, 0, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		)
	};
	
	for (int i = SKY_FRONT; i <= SKY_RIGHT; ++i) {
		glm::mat4 m = skyPlanes [i].matrix;
		
		m = glm::scale (m, glm::vec3 (500.0f));
		m = glm::translate (m, glm::vec3 (0.0f, -1.0f, 0.0f));
		m = m * xRotate;
		
		planes [i].matrix = m;
	}
}
}
