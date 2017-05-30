#include "colorado/vbo.h"

#include "colorado/vbo-element.h"

//#include "colorado/node.h"
//#include "colorado/node-renderer.h"
#include "colorado/triangle-shader.h"
#include "colorado/mesh.h"

namespace Colorado {
void VboNodePlacement::render () const {
	glDrawElements (GL_TRIANGLES, length, GL_UNSIGNED_SHORT, (GLvoid *)(0 + (size_t)offset));
}
/*
void VboNodePlacement::render (const NodeRenderer & renderer, 
	glm::mat4 modelMatrix, glm::mat3 normalMatrix) const
{
	auto mvp = renderer.viewMatrix * modelMatrix;
	renderer.shader->setMvpMatrix (mvp);
	renderer.shader->setNormalMatrix (normalMatrix);
	render ();
}
*/
Vbo::Vbo () : vertexBuffer (GlBuffer::VertexBuffer), indexBuffer (GlBuffer::IndexBuffer) {}

void Vbo::bind () const {
	vertexBuffer.bind ();
	indexBuffer.bind ();
}

void setShaderVertexAttribs (const TriangleShader & s) {
	// TODO: Magic numbers
	// Note that the sizes and offsets here are based on how the data is represented in RAM.
	// So even though a shader may say that position is vec4 and normal is vec3, VboElement
	// can say that position is vec3 and normal is vec4, and everything will be fine.
	
	if (s.vertPosAttribute >= 0) {
	glVertexAttribPointer (s.vertPosAttribute, 3, GL_FLOAT, false, sizeof (VboElement), (char *)NULL + 0);
	}
	if (s.vertNormAttribute >= 0) {
	glVertexAttribPointer (s.vertNormAttribute, 4, GL_FLOAT, false, sizeof (VboElement), (char *)NULL + sizeof (GLfloat) * 3);
	}
	if (s.vertMatrixIndexAttribute >= 0) {
	glVertexAttribPointer (s.vertMatrixIndexAttribute, 1, GL_FLOAT, false, sizeof (VboElement),(char *)NULL + sizeof (GLfloat) * 7);
	}
	if (s.vertTexCoordAttribute >= 0) {
	glVertexAttribPointer (s.vertTexCoordAttribute, 2, GL_FLOAT, false, sizeof (VboElement), (char *)NULL + sizeof (GLfloat) * 8);
	}
}

// Note: vboOffset is in vertex count, not bytes

static void loadMeshIntoVbo (int vboOffset, int iboOffset, int meshIndex, const Mesh & mesh,
	bool mirror)
{
	int numIndices = mesh.vertIndexArray.size ();
	
	std::vector <VboElement> verticesVector = mesh.vertexArray;
	VboElement * vertices = verticesVector.data ();
	
	for (uint32_t i = 0; i < mesh.numVerts (); i++) {
		VboElement & v = vertices [i];
		v.matrixIndex = meshIndex;
	}
	
	glBufferSubData (GL_ARRAY_BUFFER, sizeof (VboElement) * vboOffset, 
		sizeof (VboElement) * verticesVector.size (), vertices);
	
	std::vector <VboIndex> alteredIndicesVector 
	(mesh.vertIndexArray.size ());
	VboIndex * alteredIndices = alteredIndicesVector.data ();
	
	// Add the VBO offset to our new index data
	for (int i = 0; i < numIndices; ++i) {
		alteredIndices [i] = mesh.vertIndexArray [i] + vboOffset;
	}
	
	// Flip the triangles around if necessary
	if (mirror) {
		for (int i = 0; i < numIndices; i += 3) {
			std::swap (alteredIndices [i + 1], alteredIndices [i + 2]);
		}
	}
	
	glBufferSubData (GL_ELEMENT_ARRAY_BUFFER, sizeof (VboIndex) * iboOffset, 
		sizeof (VboIndex) * alteredIndicesVector.size (), alteredIndices);
}
/*
void loadVbo (std::vector <Node *> nodes, Vbo & vbo, std::vector <VboNodePlacement> & placements) {
	int numVertices = 0;
	int numIndices = 0;
	
	for (unsigned int i = 0; i < nodes.size (); ++i) {
		Node * node = nodes [i];
		
		numVertices += node->getNumVertices ();
		numIndices += node->getNumIndices ();
	}
	
	vbo.vertexBuffer.create ();
	vbo.indexBuffer.create ();
	
	vbo.vertexBuffer.bind ();
	vbo.indexBuffer.bind ();
	
	vbo.vertexBuffer.allocate (numVertices * sizeof (VboElement));
	vbo.indexBuffer.allocate (numIndices * sizeof (VboIndex));
	
	// These are counts, not bytes
	int vboOffset = 0;
	int iboOffset = 0;
	
	for (unsigned int k = 0; k < nodes.size (); ++k) {
		Node * node = nodes [k];
		
		VboNodePlacement vnp;
		vnp.offset = iboOffset * sizeof (VboIndex);
		
		int totalNodeIndices = 0;
		
		for (int j = 0; j < node->getNumObjects (); j++) {
			const SubNode & so = node->objectArray.at (j);
			for (unsigned int i = 0; i < so.meshArray.size (); ++i) {
				const Mesh & currentMesh = node->meshArray [so.meshArray.at (i)];
				
				loadMeshIntoVbo (vboOffset, iboOffset, j, currentMesh, so.initialMatrixIsMirrored);
				
				vboOffset += currentMesh.numVerts ();
				iboOffset += currentMesh.vertIndexArray.size ();
				
				totalNodeIndices += currentMesh.vertIndexArray.size ();
			}
		}
		
		vnp.length = totalNodeIndices;
		
		placements.push_back (vnp);
	}
}
*/
}
