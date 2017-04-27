#include "colorado/ui-vbo-node.h"
using std::vector;

#include <stdint.h>
#include <string>

#include "colorado/gl.h"
#include "colorado/triangle-shader.h"

namespace Colorado {
UiVboNode::UiVboNode () : vbo (GlBuffer::VertexBuffer), ibo (GlBuffer::IndexBuffer) {
	capacity = 0;
	cursor = 0;
	
	float characterTrim = 1;
	characterWidth = 16 - 2 * characterTrim;
	
	characterBase [0].x = 0;
	characterBase [0].y = 0;
	characterBase [0].u = characterTrim / 256.0;
	characterBase [0].v = 0;
	
	characterBase [1].x = characterWidth;
	characterBase [1].y = 0;
	characterBase [1].u = (16.0 - characterTrim) / 256.0;
	characterBase [1].v = 0;
	
	characterBase [2].x = characterWidth;
	characterBase [2].y = 16;
	characterBase [2].u = (16.0 - characterTrim) / 256.0;
	characterBase [2].v = 16.0 / 256.0;;
	
	characterBase [3].x = 0;
	characterBase [3].y = 16;
	characterBase [3].u = characterTrim / 256.0;
	characterBase [3].v = 16.0 / 256.0;
}

void UiVboNode::initializeGl () {
	vbo.create ();
	ibo.create ();
}

void UiVboNode::clearLayout () {
	cursor = 0;
}

void UiVboNode::layoutLine (float x, float y, const std::string &line) {
	layoutLine (x, y, line.data (), line.length ());
}

void UiVboNode::layoutLine (float x, float y, const char * data, int length) {
	int endChar = std::min (capacity, cursor + length);
	for (int charIndex = 0; charIndex + cursor < endChar; ++charIndex) {
		layoutChar (charIndex + cursor, x + charIndex * characterWidth, y, data [charIndex]);
	}
	cursor += length;
}

void UiVboNode::layoutChar (int charIndex, float x, float y, uchar ascii) {
	for (int vertexIndex = 0; vertexIndex < 4; ++vertexIndex) {
		int index = vertexIndex + charIndex * 4;
		
		UiVboElement & element = vertices [index];
		const UiVboElement & base = characterBase [vertexIndex];
		
		element.x = base.x + x;
		element.y = base.y + y;
		
		uchar asciiX = ascii % 16;
		uchar asciiY = ascii / 16;
		
		element.u = base.u + asciiX * 16.0 / 256.0;
		element.v = 1.0 - (base.v + asciiY * 16.0 / 256.0);
	}
}

void UiVboNode::bind () const {
	vbo.bind ();
	ibo.bind ();
}

void UiVboNode::uploadVbo () const {
	glBufferData (GlBuffer::VertexBuffer, sizeof (UiVboElement) * capacity, vertices.data (), GL_STREAM_DRAW);
	glBufferData (GlBuffer::IndexBuffer, sizeof (uint32_t) * capacity, indices.data (), GL_STREAM_DRAW);
}

void UiVboNode::prepareVbo (const Shader & s) const {
	glVertexAttribPointer (s.attribPos, 2, GL_FLOAT, false, sizeof (UiVboElement), (char *)NULL + 0);
	glVertexAttribPointer (s.attribTexCoord, 2, GL_FLOAT, false, sizeof (UiVboElement), (char *)NULL + 2 * sizeof (GLfloat));
}

void UiVboNode::render () const {
	glDrawElements (GL_TRIANGLES, cursor * 6, GL_UNSIGNED_INT, (char *)0);
}

void UiVboNode::render2 (const Shader & shader) const {
	bind ();
	uploadVbo ();
	prepareVbo (shader);
	render ();
}

void UiVboNode::setCapacity (int s) {
	vertices.resize (s * 4);
	indices.resize (s * 6);
	
	capacity = s;
	
	uint32_t indexBase [6] = {
		0, 3, 1,
		1, 3, 2
	};
	
	for (int charIndex = 0; charIndex < capacity; ++charIndex) {
		for (int vertexIndex = 0; vertexIndex < 6; ++vertexIndex) {
			indices [charIndex * 6 + vertexIndex] = charIndex * 4 + indexBase [vertexIndex];
		}
	}
}
}
