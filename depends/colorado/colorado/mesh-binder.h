#ifndef Mesh2BINDER_H
#define Mesh2BINDER_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "colorado/dllspec.h"
#include "colorado/vbo.h"
#include "obj-loader.h"

namespace Colorado {

struct COLORADO_DLLSPEC Mesh2 {
	Colorado::Vbo vbo;
	std::vector <std::string> names;
	std::vector <Colorado::VboNodePlacement> placements;
	
	Mesh2 ();
	Mesh2 (const std::string &);
	
	void load (const std::string &);
	Colorado::VboNodePlacement lookupName (const std::string &) const;
	static void renderPlacement (const Colorado::VboNodePlacement &);
	void renderPlacement (int) const;
	void renderPlacement (const std::string &) const;
	
	static void loadObjIntoVbo (
		const std::vector <ObjMesh> & obj,
		Colorado::Vbo & vbo,
		std::vector <Colorado::VboNodePlacement> & placements);
};

struct COLORADO_DLLSPEC Mesh2Binder {
	std::shared_ptr <const Mesh2> currentMesh2;
	std::map <std::string, std::shared_ptr <const Mesh2> > Mesh2es;
	
	// Can lazy-load a Mesh2 if it was not already loaded
	std::shared_ptr <const Mesh2> getMesh2 (const std::string &);
	std::shared_ptr <const Mesh2> loadMesh2 (const std::string &);
	void reloadMesh2es ();
	
	void bind (std::shared_ptr <const Mesh2>);
	void bind (const std::string &);
	void renderPlacement (int) const;
	void renderPlacement (const std::string &) const;
};

}

#endif // Mesh2BINDER_H
