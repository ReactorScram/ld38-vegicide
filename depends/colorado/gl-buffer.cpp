#include "colorado/gl-buffer.h"

namespace Colorado {
GlBuffer::GlBuffer (Type t) {
	type = t;
	bufferId = NO_BUFFER;
}

GlBuffer::~GlBuffer () {
	destroy ();
}

void GlBuffer::create () {
	if (bufferId == NO_BUFFER) {
		glGenBuffers (1, &bufferId);
	}
}

void GlBuffer::allocate (int32_t size) const {
	allocate (size, GL_STATIC_DRAW);
}

void GlBuffer::allocate (int32_t size, GLenum drawType) const {
	glBufferData (type, size, 0, drawType);
}

void GlBuffer::bind () const {
	glBindBuffer (type, bufferId);
}

void GlBuffer::destroy () {
	glDeleteBuffers (1, &bufferId);
	bufferId = NO_BUFFER;
}
}
