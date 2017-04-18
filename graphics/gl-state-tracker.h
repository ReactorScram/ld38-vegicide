#ifndef GLSTATETRACKER_H
#define GLSTATETRACKER_H

#include <stdint.h>
#include <unordered_map>

#include "colorado/gl.h"

// Handles all state that isn't directly related to the current VBO
// or shader or texture

struct GlState {
	std::unordered_map <GLenum, bool> bools;
	GLclampf clearColor [4];
	bool depthMask;
	bool colorMask [4];
	int stencilMask;
	
	int frontFace;
	int depthFunc;
	int blendFunc [2];
	int stencilOp [3];
	int stencilFunc [3];
	
	GlState ();
	
	const GlState & operator = (const GlState & o);
	
	// Forces the GL state without optimizing
	void Force () const;
	
	// Matches the GL state with optimization
	void Match (const GlState & o) const;
};

struct GlStateTracker {
	GlStateTracker ();
	
	// Call when other code may have mutated the GL state
	void MarkUnknown ();
	
	void Force (const GlState & s);
	
	void Match (const GlState & s);
	
private:
	bool unknown;
	GlState internal_state;
};
#endif // GLSTATETRACKER_H
