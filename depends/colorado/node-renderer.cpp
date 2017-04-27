#include "colorado/node-renderer.h"

namespace Colorado {
NodeRenderer::NodeRenderer (const glm::mat4 & mat, TriangleShader * s) :
	viewMatrix (mat), shader (s)
{
	
}
}
