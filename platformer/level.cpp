#include "level.h"

#include <string.h>

#include "iqm/iqm.h"

using namespace std;

Level::Level (): width (0), height (0), data () {}

Level::Level (const std::vector <uint8_t> & bytes) {
	width = *(int16_t *)&bytes [0];
	height = *(int16_t *)&bytes [2];
	int size = width * height;
	
	data = vector <int16_t> ((int16_t *)&bytes [4], (int16_t *)&bytes [4 + 2 * size]);
}

struct Vertex {
	float x, y, z;
	float u, v;
	float unused [7];
};

// Everything that must be in a fixed position within the file
struct IqmLevel {
	iqmheader header;
	iqmmesh mesh;
	iqmvertexarray vertex_arrays [4];
};

vector <uint8_t> Level::to_iqm () const {
	iqmheader header;
	memset (&header, 0, sizeof (header));
	memcpy (header.magic, "INTERQUAKEMODEL", 16);
	header.ofs_text = 0;
	header.num_text = 0;
	header.num_triangles = 2 * width * height;
	header.num_vertexarrays = 4;
	header.num_vertexes = 4 * width * height;
	header.num_meshes = 1;
	
	iqmmesh mesh;
	memset (&mesh, 0, sizeof (mesh));
	mesh.first_triangle = 0;
	mesh.num_triangles = 2 * width * height;
	
	iqmvertexarray vertex_arrays [4];
	memset (&vertex_arrays, 0, sizeof (vertex_arrays));
	// Pos
	vertex_arrays [0].size = 3;
	// UV
	vertex_arrays [1].size = 2;
	// N/A
	vertex_arrays [2].size = 3;
	vertex_arrays [3].size = 4;
	
	vector <Vertex> vertices;
	for (int ty = 0; ty < height; ty++) {
		for (int tx = 0; tx < width; tx++) {
			float x = tx;
			float y = ty;
			
			auto tex_i = data.at (tx + ty * width);
			float tex_size = 1.0 / 8.0;
			float tex_x = (tex_i % 8) * tex_size;
			float tex_y = (tex_i / 8) * tex_size;
			
			vertices.push_back (Vertex { x, y, 0, tex_x, tex_y, {0,0,0,0,0,0,0}});
			vertices.push_back (Vertex { x + 1, y, 0, tex_x + tex_size, tex_y, {0,0,0,0,0,0,0}});
			vertices.push_back (Vertex { x + 1, y + 1, 0, tex_x + tex_size, tex_y + tex_size, {0,0,0,0,0,0,0}});
			vertices.push_back (Vertex { x, y + 1, 0, tex_x, tex_y + tex_size, {0,0,0,0,0,0,0}});
		}
	}
	
	vector <int32_t> indices;
	for (int i = 0; i < width * height; i++) {
		indices.push_back (i * 4 + 0);
		indices.push_back (i * 4 + 1);
		indices.push_back (i * 4 + 2);
		indices.push_back (i * 4 + 0);
		indices.push_back (i * 4 + 2);
		indices.push_back (i * 4 + 3);
	}
	
	IqmLevel level;
	level.header = header;
	level.mesh = mesh;
	for (int i = 0; i < 4; i++) {
		level.vertex_arrays [i] = vertex_arrays [i];
	}
	
	level.header.ofs_meshes = (int)((char *)&level.mesh - (char *)&level.header);
	// Triangles go immediately after the fixed portion
	level.header.ofs_triangles = (int)sizeof (IqmLevel);
	level.header.ofs_vertexarrays = (int)((char *)&level.vertex_arrays - (char *)&level.header);
	
	auto ofs_va = level.header.ofs_triangles + sizeof (int32_t) * indices.size ();
	level.vertex_arrays [0].offset = ofs_va;
	level.vertex_arrays [1].offset = ofs_va + vertices.size () * sizeof (float) * 3;
	level.vertex_arrays [2].offset = ofs_va + vertices.size () * sizeof (float) * (3 + 2);
	level.vertex_arrays [3].offset = ofs_va + vertices.size () * sizeof (float) * (3 + 2);
	
	vector <uint8_t> result;
	
	result.insert (result.end (), (uint8_t *)&level, (uint8_t *)&level + sizeof (IqmLevel));
	
	result.insert (result.end (), (uint8_t *)&indices [0], (uint8_t *)&indices [0] + indices.size () * sizeof (int32_t));
	
	vector <float> unrolled_vertices;
	for (const Vertex & v : vertices) {
		unrolled_vertices.push_back (v.x);
		unrolled_vertices.push_back (v.y);
		unrolled_vertices.push_back (0);
	}
	
	for (const Vertex & v : vertices) {
		unrolled_vertices.push_back (v.u);
		unrolled_vertices.push_back (v.v);
	}
	
	for (const Vertex & v : vertices) {
		for (int i = 0; i < 4; i++) {
			unrolled_vertices.push_back (v.x * 0);
		}
	}
	
	result.insert (result.end (), (uint8_t *)&unrolled_vertices [0], (uint8_t *)&unrolled_vertices [0] + unrolled_vertices.size () * sizeof (float));
	
	return result;
}
