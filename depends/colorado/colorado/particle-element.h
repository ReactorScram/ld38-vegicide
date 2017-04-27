#ifndef PARTICLEELEMENT_H
#define PARTICLEELEMENT_H

#include <glm/glm.hpp>

#include "colorado/dllspec.h"

namespace Colorado {
class COLORADO_DLLSPEC ParticleElement {
public:
	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		float element [4];
	} pos;
	
	union {
		struct {
			float r;
			float g;
			float b;
			float a;
		};
		float element [4];
	} color;
	
	void setPos (float, float, float);
	void setPos (glm::vec3);
};
}

#endif // PARTICLEELEMENT_H
