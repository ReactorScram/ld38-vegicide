#ifndef VBONODE_H
#define VBONODE_H

#include "colorado/dllspec.h"
#include "colorado/gl.h"
#include "colorado/gl-buffer.h"
#include "colorado/node.h"
#include "colorado/node-renderer.h"

namespace Colorado {
class COLORADO_DLLSPEC VboNode {
public:
	VboNode ();
	
	int getObjectCount () const;
	const SubNode & getObject (int) const;
	
	int getNumIndices () const;
	
	void load (const Node &);
	
	// Prepares for rendering
	// Usually this should be called on every frame
	void prepareVbo (const TriangleShader &);
	
	// NOTE: None of these render functions will bind a shader
	// They just need the shader so they can set uniforms
	
	void render () const;
	void render (int, int) const;
	
	void render (TriangleShader &, const std::vector <glm::mat4> &, const std::vector <glm::mat3> &) const;
	
	// NOTE: This does not allow for any sort of per-object animation
	void render (const glm::mat4 &, const glm::mat4 &, const NodeRenderer &) const;
	
	// Renders with per-object matrices from a pointer
	// It expects as many matrices available as there are objects
	// in objectArray.
	void render (const glm::mat4 &, const glm::mat4 &, const NodeRenderer &, 
				 const std::vector <glm::mat4> &) const;
	
protected:
	GlBuffer vbo;
	GlBuffer ibo;
	
	int numVertices;
	int numIndices;
	
	std::vector <SubNode> objectArray;
	
	void createBuffers ();
	void initVbo (int, int);
	
	// Assumes that the VBO / IBO pair has been bound
	void loadMeshIntoVbo (int, int, int, const Mesh &, bool mirror = false);
};
}

#endif // VBONODE_H
