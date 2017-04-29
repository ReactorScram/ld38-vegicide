#include "gl-state-tracker.h"

#include "colorado/gl.h"

#include <cstring>

// Handles all state that isn't directly related to the current VBO
// or shader or texture

using namespace std;

GlState::GlState () :
	bools (),
	depthMask (true),
	stencilMask (0xff),
	frontFace (GL_CW),
	depthFunc (GL_LEQUAL)
{
	bools [GL_BLEND] = false;
	bools [GL_CULL_FACE] = false;
	bools [GL_DEPTH_TEST] = false;
	bools [GL_STENCIL_TEST] = false;
	bools [GL_TEXTURE_2D] = false;
	
	memset (blendFunc, 0, sizeof (blendFunc));
	memset (colorMask, true, sizeof (colorMask));
	memset (stencilOp, 0, sizeof (stencilOp));
	memset (stencilFunc, 0, sizeof (stencilFunc));
	memset (clearColor, 0, sizeof (clearColor));
}

const GlState & GlState::operator = (const GlState & o) {
	bools = o.bools;
	
	depthMask = o.depthMask;
	
	depthFunc = o.depthFunc;
	frontFace = o.frontFace;
	
	memcpy (clearColor, o.clearColor, sizeof (clearColor));
	memcpy (colorMask, o.colorMask, sizeof (colorMask));
	
	// Please let this optimization work
	// This is supposed to ignore blend / stencil state when it's
	// unused and leave it as-is rather than flipping it a lot
	if (o.bools.at (GL_BLEND)) {
		memcpy (blendFunc, o.blendFunc, sizeof (blendFunc));
	}
	
	if (o.bools.at (GL_STENCIL_TEST)) {
		stencilMask = o.stencilMask;
		memcpy (stencilOp, o.stencilOp, sizeof (stencilOp));
		memcpy (stencilFunc, o.stencilFunc, sizeof (stencilFunc));
	}
	
	return *this;
}

// Forces the GL state without optimizing
void GlState::Force () const {
	for (pair <GLenum, bool> p: bools) {
		if (p.second) {
			glEnable (p.first);
		}
		else {
			glDisable (p.first);
		}
	}
	
	{
		auto cc = clearColor;
		glClearColor (cc [0], cc [1], cc [2], cc [3]);
	}
	glDepthMask (depthMask);
	{
		auto cm = colorMask;
		glColorMask (cm [0], cm [1], cm [2], cm [3]);
	}
	glStencilMask (stencilMask);
	glBlendFunc (blendFunc [0], blendFunc [1]);
	
	{
		auto so = stencilOp;
		glStencilOp (so [0], so [1], so [2]);
	}
	{
		auto sf = stencilFunc;
		glStencilFunc (sf [0], sf [1], sf [2]);
	}
	
	glDepthFunc (depthFunc);
	glFrontFace (frontFace);
}

// Matches the GL state with optimization
void GlState::Match (const GlState & o) const {
	for (pair <GLenum, bool> p: o.bools) {
		bool us = bools.at (p.first);
		if (p.second && ! us) {
			glEnable (p.first);
		}
		else if (! p.second && us) {
			glDisable (p.first);
		}
	}
	
	if (0 != memcmp (clearColor, o.clearColor, sizeof (clearColor))) {
		auto cc = o.clearColor;
		glClearColor (cc [0], cc [1], cc [2], cc [3]);
	}
	if (depthMask != o.depthMask) {
		glDepthMask (o.depthMask);
	}
	if (0 != memcmp (colorMask, o.colorMask, sizeof (colorMask))) {
		auto cm = o.colorMask;
		glColorMask (cm [0], cm [1], cm [2], cm [3]);
	}
	
	// If blending is disabled, optimize out blend state changes
	if (o.bools.at (GL_BLEND)) {
		if (0 != memcmp (blendFunc, o.blendFunc, sizeof (blendFunc))) {
			glBlendFunc (o.blendFunc [0], o.blendFunc [1]);
		}
	}
	
	// According to the ES2 spec, stencil_test can disable all other
	// stencil state
	if (o.bools.at (GL_STENCIL_TEST)) {
		if (stencilMask != o.stencilMask) {
			glStencilMask (o.stencilMask);
		}
		if (0 != memcmp (stencilOp, o.stencilOp, sizeof (stencilOp))) {
			auto so = o.stencilOp;
			glStencilOp (so [0], so [1], so [2]);
		}
		if (0 != memcmp (stencilFunc, o.stencilFunc, sizeof (stencilFunc))) {
			auto sf = o.stencilFunc;
			glStencilFunc (sf [0], sf [1], sf [2]);
		}
	}
	
	if (o.depthFunc != depthFunc) {
		glDepthFunc (o.depthFunc);
	}
	
	if (o.frontFace != frontFace) {
		glFrontFace (o.frontFace);
	}
}

GlStateTracker::GlStateTracker () :
	unknown (false),
	internal_state ()
{
	
}

// Call when other code may have mutated the GL state
void GlStateTracker::MarkUnknown () {
	unknown = true;
}

void GlStateTracker::Force (const GlState & s) {
	s.Force ();
	internal_state = s;
}

void GlStateTracker::Match (const GlState & s) {
	if (unknown) {
		unknown = false;
		s.Force ();
	}
	else {
		internal_state.Match (s);
	}
	internal_state = s;
}
