#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <stdint.h>
#include <string>
#include <vector>

namespace Colorado {
class LuaState;
}

class ObjMesh {
public:
	std::string name;
	/* 
	Interleaved attributes
	pos x, y, z, normal x, y, z, texture u, v
	8 floats make a vertex, 3 vertices make a triangle
	no quads
	*/
	std::vector <float> attributes;
	
	static std::vector <ObjMesh> loadObjFromBuffer (const std::vector <unsigned char> & buffer);
	static std::vector <ObjMesh> loadObjFromFile (std::string fileName);
	
protected:
	static std::vector <ObjMesh> loadObjFromLua (Colorado::LuaState & l);
	static ObjMesh loadMeshFromLua (Colorado::LuaState & l);
};

#endif // OBJLOADER_H
