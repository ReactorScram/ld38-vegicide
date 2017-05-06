#include "gl-state-tracker.h"

GlState::GlState () :
	bools (),
	depthMask (true),
	stencilMask (0xff),
	frontFace (0),
	depthFunc (0)
{
	/*
	bools [GL_BLEND] = false;
	bools [GL_CULL_FACE] = false;
	bools [GL_DEPTH_TEST] = false;
	bools [GL_STENCIL_TEST] = false;
	bools [GL_TEXTURE_2D] = false;
	*/
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
	/*
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
	*/
	return *this;
}
