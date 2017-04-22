#ifndef VEGICIDE_LOGIC_H
#define VEGICIDE_LOGIC_H

#include "input.h"
#include "scene.h"

struct Logic {
	SceneEcs scene;
	
	Logic ();
	void step (const InputFrame & input);
};

#endif
