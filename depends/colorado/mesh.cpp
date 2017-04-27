#include "colorado/mesh.h"

#include <stdio.h>

#include "colorado/triangle-optimizer.h"

namespace Colorado {
// Doesn't really do anything
Mesh::Mesh () {
	
}

void Mesh::clear () {
	vertexArray.clear ();
	vertIndexArray.clear ();
}

void Mesh::printMetadata () const {
	/*
	printf ("Mesh has:\n");
	printf ("%i vertices\n", numVerts ());
	printf ("%li indices\n", vertIndexArray.size ());
	*/
}

void Mesh::printData () const {
	/*
	for (unsigned int i = 0; i < numVerts (); ++i) {
		const VboElement & vertexElement = vertexArray.at (i);
		const float * pos = vertexElement.pos.element;
		const float * normal = vertexElement.normal.element;
		const float * texCoord = vertexElement.textureCoord.element;
		
		printf ("Vertex %i\n", i);
		printf ("pos\t%f, %f, %f\n", pos [0], pos [1], pos [2]);
		printf ("norm\t%f, %f, %f\n", normal [0], normal [1], normal [2]);
		printf ("tex\t%f, %f\n", texCoord [0], texCoord [1]);
		
	}
	
	for (unsigned int i = 0; i < vertIndexArray.size (); i += 3) {
		printf ("VI %i\t%i, %i, %i\n", i / 3,
		vertIndexArray [i + 0], vertIndexArray [i + 1], vertIndexArray [i + 2]);		
	}
	*/
}

AABB Mesh::getAABB () const {
	AABB result;
	
	float min [3];
	float max [3];
	
	if (numVerts () > 0) {
		for (int i = 0; i < 3; ++i) {
			min [i] = max [i] = vertexArray.at (i).pos.element [i];
		}
	
		for (unsigned int j = 1; j < numVerts (); j++) {
			for (int i = 0; i < 3; i++) {
				float value = vertexArray.at (j).pos.element [i];
				if (value < min [i]) {
					min [i] = value;
				}
				else if (value > max [i]) {
					max [i] = value;
				}
			}
		}
		
		result.min.x = min [0];
		result.min.y = min [1];
		result.min.z = min [2];
		
		result.max.x = max [0];
		result.max.y = max [1];
		result.max.z = max [2];
	}
	
	return result;
}

uint32_t Mesh::numVerts () const {
	return vertexArray.size ();
}

uint32_t Mesh::numIndices () const {
	return vertIndexArray.size ();
}

void Mesh::optimizeTriangleOrder () {
	std::vector <uint32_t> optimizedIndices (numIndices ());
	
	// NOTE: The types have to match up here, uint should be 32 bits
	Forsyth::OptimizeFaces (vertIndexArray.data (), 
		vertIndexArray.size (), numVerts (), optimizedIndices.data (), 32);
	
	vertIndexArray = optimizedIndices;
}

}
