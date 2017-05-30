#include "colorado/particle-element.h"

namespace Colorado {
void ParticleElement::setPos (float x, float y, float z) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

void ParticleElement::setPos (glm::vec3 v) {
	pos.x = v.x;
	pos.y = v.y;
	pos.z = v.z;
}
}
