#include "colorado/plane.h"

namespace Colorado {
float Plane::testPoint (const glm::vec3 & p) const {
	return glm::dot (p, normal) - d;
}

void Plane::setPoint (const glm::vec3 & p) {
	d = glm::dot (p, normal);
}

bool Plane::isSphereVisible (glm::vec3 center, float radius, const Plane frustum [], int numPlanes) {
	for (int i = 0; i < numPlanes; ++i) {
		if (frustum [i].testPoint (center) > radius) {
			return false;
		}
	}
	
	return true;
}
}
