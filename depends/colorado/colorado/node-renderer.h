#ifndef NODERENDERER_H
#define NODERENDERER_H

#include <glm/glm.hpp>

#include "colorado/dllspec.h"
#include "colorado/triangle-shader.h"

namespace Colorado {
class COLORADO_DLLSPEC NodeRenderer {
public:
	glm::mat4 viewMatrix;
	TriangleShader * shader;
	
	NodeRenderer (const glm::mat4 & mat = glm::mat4 (), TriangleShader * s = NULL);
};
}

#endif // NODERENDERER_H
