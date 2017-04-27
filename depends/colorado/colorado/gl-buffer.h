#ifndef GLBUFFER_H
#define GLBUFFER_H

#include "colorado/gl.h"
#include <stdint.h>

namespace Colorado {
class COLORADO_DLLSPEC GlBuffer {
public:
	enum Type {
		VertexBuffer = GL_ARRAY_BUFFER,
		IndexBuffer = GL_ELEMENT_ARRAY_BUFFER
	} type;
	
	GLuint bufferId;
	
	static const GLuint NO_BUFFER = 0;
	
	GlBuffer (Type);
	~GlBuffer ();
	
	// Grabs an ID for the buffer
	// This also binds it
	void create ();
	
	// Allocates some memory
	void allocate (int32_t) const;
	void allocate (int32_t, GLenum) const;
	
	// Call this right before you want to draw with it
	void bind () const;
	
	// Deletes the ID for the buffer
	// OpenGL should de-allocate the memory
	
	// The ES 2.0.25 spec says that buffer ID 0 will be ignored,
	// so this function doesn't check for it
	void destroy ();
};
}

#endif // GLBUFFER_H
