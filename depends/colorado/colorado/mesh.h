#ifndef LUL_MESH_H
#define LUL_MESH_H

#include <vector>
#include <stdint.h>

#include "colorado/aabb.h"
#include "colorado/dllspec.h"
#include "colorado/vbo-element.h"

struct lua_State;

namespace Colorado {
class COLORADO_DLLSPEC Mesh {
public:
	// Mesh data
	std::vector <VboElement> vertexArray;
	std::vector <uint32_t> vertIndexArray;
	
	Mesh ();
	
	AABB getAABB () const;
	
	void clear ();
	
	void printMetadata () const;
	void printData () const;
	
	void loadMeshLua (lua_State *, bool debug = false);
	
	void loadVerticesLua (lua_State *, bool debug = false);
	void loadFacesLua (lua_State *, bool debug = false);
	
	uint32_t numVerts () const;
	uint32_t numIndices () const;
	
	// Uses the Forsyth optimizer to reorder triangles
	void optimizeTriangleOrder ();
};
}

#endif
