#ifndef VBO_H
#define VBO_H

#include <vector>

#include <glm/glm.hpp>

#include "colorado/dllspec.h"
#include "colorado/gl-buffer.h"

namespace Colorado {
//class Node;
//class NodeRenderer;
class TriangleShader;

// Make 16-bit or 32-bit as desired
typedef unsigned short VboIndex;

struct COLORADO_DLLSPEC VboNodePlacement {
	uint32_t offset;		// Bytes
	uint32_t length;		// Indices
	
	void render () const;
	//void render (const NodeRenderer &, glm::mat4, glm::mat3) const;
};

struct COLORADO_DLLSPEC Vbo {
	GlBuffer vertexBuffer;
	GlBuffer indexBuffer;
	
	Vbo ();
	
	void bind () const;
};

void COLORADO_DLLSPEC setShaderVertexAttribs (const TriangleShader &);
//void COLORADO_DLLSPEC loadVbo (std::vector <Node *>, Vbo &, std::vector <VboNodePlacement> &);
}

#endif // VBO_H
