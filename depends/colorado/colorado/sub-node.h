#ifndef LUL_SCENE_H
#define LUL_SCENE_H

#include <vector>


#include <glm/glm.hpp>

#include "colorado/dllspec.h"
#include "colorado/mesh.h"

// NOTE: SubNode is not meant to be used without Node

namespace Colorado {
class COLORADO_DLLSPEC SubNode {
public:
	std::vector <int32_t> meshArray;
	glm::mat4 initialMatrix;
	bool initialMatrixIsMirrored;
	int32_t parent;
	
	std::vector <SubNode *> children;
	
	SubNode ();
	~SubNode ();
	
	virtual void loadObjLua (lua_State *);
	void addChild (SubNode *);
};
}

#endif
