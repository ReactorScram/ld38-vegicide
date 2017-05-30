#ifndef PLANE_H
#define PLANE_H

#include <glm/glm.hpp>

#include "colorado/dllspec.h"

namespace Colorado {
class COLORADO_DLLSPEC Plane {
public:
	glm::vec3 normal;
	float d;
	
	// Distance from plane. Positive is "outside" 
	float testPoint (const glm::vec3 &) const;
	
	void setPoint (const glm::vec3 &);
	
	static bool isSphereVisible (glm::vec3, float, const Plane [], int);
};
}

#endif
