#ifndef GLSTATETRACKER_H
#define GLSTATETRACKER_H

#include "gl-state.h"

// Handles all state that isn't directly related to the current VBO
// or shader or texture

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
