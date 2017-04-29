#ifndef COLORADO_GL_STATE_H
#define COLORADO_GL_STATE_H

#include <map>

struct GlState {
	std::map <int, bool> bools;
	float clearColor [4];
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

#endif
