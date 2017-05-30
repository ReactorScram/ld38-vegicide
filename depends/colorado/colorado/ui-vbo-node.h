#ifndef UIVBONODE_H
#define UIVBONODE_H

#include <string>
#include <vector>

#include "colorado/dllspec.h"
#include "colorado/gl-buffer.h"
#include "colorado/ui-vbo-element.h"

namespace Colorado {
typedef unsigned char uchar;

class Shader;

class COLORADO_DLLSPEC UiVboNode {
public:
	float characterWidth;
	UiVboElement characterBase [4];
	
	UiVboNode ();
	
	void initializeGl ();
	
	void clearLayout ();
	void layoutLine (float x, float y, const std::string & line);
	void layoutLine (float x, float y, const char * line, int length);
	void layoutChar (int, float, float, uchar);
	
	// These 4 functions are usually called in order
	void bind () const;
	void uploadVbo () const;
	void prepareVbo (const Shader &) const;
	void render () const;
	
	// So you can instead call this and it calls all 4 for you
	void render2 (const Shader &) const;
	
	void setCapacity (int);
	void setSize (int);
	
protected:
	int capacity;
	
	int cursor;
	
	std::vector <UiVboElement> vertices;
	std::vector <unsigned int> indices;
	
	GlBuffer vbo;
	GlBuffer ibo;
};
}

#endif // UIVBONODE_H
