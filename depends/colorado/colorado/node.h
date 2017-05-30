#ifndef SCENEOBJECTTREE_H
#define SCENEOBJECTTREE_H

#include "colorado/dllspec.h"
#include "colorado/sub-node.h"

typedef unsigned char uchar;

namespace Colorado {
class LuaState;

class COLORADO_DLLSPEC Node {
public:
	std::vector <Mesh> meshArray;
	std::vector <SubNode> objectArray;
	
	SubNode root;
	
	Node ();
	~Node ();
	
	// This information can be retrieved directly from the containers,
	// but I don't think it happens often enough to be needed.
	int getNumMeshes () const;
	int getNumObjects () const;
	
	int getNumVertices () const;
	int getNumIndices () const;
	
	bool loadFromBuffer (const std::vector <uchar> &, bool debug = false);
	bool loadFromLuaState (lua_State *, bool debug = false);
	
	void loadFromMesh (const Mesh &);
	
	void unload ();
	
	void loadScreenQuad ();
	void calculateStats ();
	
protected:
	// Stats
	int numVertices;
	int numIndices;
	
	
};
}

#endif // SCENEOBJECTTREE_H
