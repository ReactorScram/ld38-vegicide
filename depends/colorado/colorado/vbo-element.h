#ifndef VBOELEMENT_H
#define VBOELEMENT_H

#include "colorado/dllspec.h"

namespace Colorado {
struct COLORADO_DLLSPEC VboElement {
	union {
		struct {
			float x;
			float y;
			float z;
		};
		float element [3];
	} pos;
	
	union {
		struct {
			float x;
			float y;
			float z;
			float w;
		};
		float element [4];
	} normal;
	
	// This is set by Node
	float matrixIndex;
	
	union {
		struct {
			float u;
			float v;
		};
		float element [2];
	} textureCoord;
	
	void setPos (float, float, float);
	void setNormal (float, float, float, float w = 0);
	void setTexCoord (float, float);
};
}

#endif // VBOELEMENT_H
