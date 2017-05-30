#include "colorado/vbo-element.h"

namespace Colorado {
void VboElement::setPos (float x, float y, float z) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

void VboElement::setNormal (float x, float y, float z, float w) {
	normal.x = x;
	normal.y = y;
	normal.z = z;
	normal.w = w;
}

void VboElement::setTexCoord (float u, float v) {
	textureCoord.u = u;
	textureCoord.v = v;
}
}
