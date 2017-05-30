#ifndef PARTICLENODE_H
#define PARTICLENODE_H

#include <vector>

#include "colorado/dllspec.h"
#include "colorado/gl-buffer.h"
#include "colorado/particle-element.h"
#include "colorado/triangle-shader.h"

namespace Colorado {
class COLORADO_DLLSPEC ParticleNode {
public:
	ParticleNode ();
	
	void initVbo (int);
	void load (const std::vector <ParticleElement> &);
	void load (const ParticleElement *, int);
	
	void prepareVbo (const TriangleShader &) const;
	void render () const;
	
protected:
	GlBuffer vbo;
	
	int maxNumVertices;
	int numVertices;
	
	// This assumes that the buffer is bound
	//void loadTestIntoVbo (int);
};
}

#endif // PARTICLENODE_H
