#include "colorado/particle-node.h"

#include "colorado/gl.h"

#include <cmath>

namespace Colorado {
ParticleNode::ParticleNode () :
	vbo (GlBuffer::VertexBuffer) {
	
	maxNumVertices = 0;
	numVertices = 0;
}

void ParticleNode::initVbo (int x) {
	maxNumVertices = x;
	
	vbo.create ();
	vbo.bind ();
	vbo.allocate (maxNumVertices * sizeof (ParticleElement));
}

void ParticleNode::prepareVbo (const TriangleShader & shader) const {
	vbo.bind ();
	
	glVertexAttribPointer (shader.vertPosAttribute, 4, GL_FLOAT, false, sizeof (ParticleElement), (char *)NULL + 0);
	glVertexAttribPointer (shader.vertColorAttribute, 4, GL_FLOAT, false, sizeof (ParticleElement), (char *)NULL + sizeof (float) * 4);
}

void ParticleNode::load (const std::vector <ParticleElement> & particles) {
	load (particles.data (), particles.size ());
}

void ParticleNode::load (const ParticleElement * data, int size) {
	if (size > maxNumVertices) {
		return;
	}
	
	numVertices = size;
	
	glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof (ParticleElement) * size, data);
}

void ParticleNode::render () const {
	glDrawArrays (GL_POINTS, 0, numVertices);
}
}
