#ifndef MESHBINDER_H
#define MESHBINDER_H

#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include "../ecs/ecs.h"
#include "colorado/vbo.h"
#include "colorado/obj-loader.h"
#include "terf/terf.h"

struct Mesh {
	Colorado::Vbo vbo;
	std::vector <std::string> names;
	std::vector <Colorado::VboNodePlacement> placements;
	
	Mesh ();
	
	void loadIqm (const std::vector <uint8_t> & buffer);
	Colorado::VboNodePlacement lookupName (const std::string &) const;
	
	static void renderPlacement (const Colorado::VboNodePlacement &);
	void renderPlacement (int) const;
	void renderPlacement (const std::string &) const;
	
	static void renderPlacementIndexed (const Colorado::VboNodePlacement &);
	void renderPlacementIndexed (int) const;
};

struct MeshBinder {
	MeshBinder ();
	
	MeshBinder (const MeshBinder &) = delete;
	MeshBinder & operator = (const MeshBinder &) = delete;
	
	const Mesh * currentMesh () const;
	std::map <MeshKey, std::unique_ptr <const Mesh> > meshes;
	
	void add (MeshKey, Mesh *);
	void add_iqm (MeshKey, const std::vector <uint8_t> & buffer);
	void bind (const MeshKey &);
	
private:
	const Mesh * m_currentMesh;
	
	// Can no longer lazy-load meshes
	const Mesh * getMesh (const MeshKey & fn) const;
	void bind (const Mesh *);
};

#endif // MESHBINDER_H
