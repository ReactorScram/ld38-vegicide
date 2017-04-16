#include "mesh-binder.h"

#include <iostream>

#include "iqm/iqm.h"

using namespace std;
using namespace Colorado;

Mesh::Mesh () :
	vbo (),
	names (),
	placements ()
{
	
}

void Mesh::loadIqm (const vector <uint8_t> & buffer) {
	if (buffer.size () < sizeof (iqmheader)) {
		// File too small
		return;
	}
	auto header = (const iqmheader *)&buffer [0];
	
	auto meshes = (const iqmmesh *)&buffer [header->ofs_meshes];
	auto triangles = (const iqmtriangle *)&buffer [header->ofs_triangles];
	auto vertexarrays = (const iqmvertexarray *)&buffer [header->ofs_vertexarrays];
	string text ((const char *)&buffer [header->ofs_text], header->num_text);
	
	int numFloats = 0;
	for (int i = 0; i < (int)header->num_vertexarrays; i++) {
		auto vertexarray = &vertexarrays [i];
		numFloats += vertexarray->size;
	}
	
	vector <float> vboSource;
	vboSource.resize (numFloats * header->num_vertexes);
	int floatOffset = 0;
	for (int i = 0; i < (int)header->num_vertexarrays; i++) {
		auto vertexarray = &vertexarrays [i];
		auto vertexData = (const float *)&buffer [vertexarray->offset];
		
		for (int j = 0; j < (int)header->num_vertexes; j++) {
			for (int k = 0; k < (int)vertexarray->size; k++) {
				vboSource [j * numFloats + floatOffset + k] = vertexData [vertexarray->size * j + k];
			}
		}
		
		floatOffset += vertexarray->size;
	}
	
	vbo.vertexBuffer.create ();
	vbo.vertexBuffer.bind ();
	glBufferData (GL_ARRAY_BUFFER, sizeof (float) * vboSource.size (), vboSource.data (), GL_STATIC_DRAW);
	
	vector <int16_t> iboSource;
	const int indicesPerTriangle = 3;
	iboSource.resize (header->num_triangles * indicesPerTriangle);
	for (int i = 0; i < (int)header->num_triangles; i++) {
		for (int j = 0; j < indicesPerTriangle; j++) {
			iboSource [i * indicesPerTriangle + j] = triangles [i].vertex [j];
		}
	}
	
	vbo.indexBuffer.create ();
	vbo.indexBuffer.bind ();
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (int16_t) * iboSource.size (), iboSource.data (), GL_STATIC_DRAW);
	
	for (int i = 0; i < (int)header->num_meshes; i++) {
		auto mesh = &meshes [i];
		VboNodePlacement vnp;
		vnp.offset = mesh->first_triangle * 3;
		vnp.length = mesh->num_triangles * 3;
		
		placements.push_back (vnp);
	}
}

VboNodePlacement Mesh::lookupName (const string & name) const {
	for (int i = 0; i < (int)names.size (); i++) {
		if (names.at (i) == name) {
			return placements.at (i);
		}
	}
	return placements.at (0);
}

void Mesh::renderPlacement (const VboNodePlacement & vnp) {
	glDrawArrays (GL_TRIANGLES, vnp.offset, vnp.length);
}

void Mesh::renderPlacement (const string & s) const {
	renderPlacement (lookupName (s));
}

void Mesh::renderPlacement (int i) const {
	renderPlacement (placements.at (i));
}

void Mesh::renderPlacementIndexed (const VboNodePlacement & vnp) {
	glDrawElements (GL_TRIANGLES, vnp.length, GL_UNSIGNED_SHORT, (char *)(vnp.offset * sizeof (int16_t)));
}

void Mesh::renderPlacementIndexed (int i) const {
	renderPlacementIndexed (placements.at (i));
}

MeshBinder::MeshBinder () :
	meshes (),
	m_currentMesh (nullptr)
{
	
}

void MeshBinder::bind (const Mesh * m) {
	m_currentMesh = m;
	m_currentMesh->vbo.bind ();
}

void MeshBinder::bind (const MeshKey & code) {
	bind (getMesh (code));
}

const Mesh * MeshBinder::currentMesh () const {
	return m_currentMesh;
}

const Mesh * MeshBinder::getMesh (const MeshKey & code) const {
	const Mesh * result = meshes.at (code).get ();
	if (result) {
		return result;
	}
	else {
		cerr << "Warning: Lazy mesh load failed - " << code << endl;
	}
	return result;
}
