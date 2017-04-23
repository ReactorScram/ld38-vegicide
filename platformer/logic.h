#ifndef VEGICIDE_LOGIC_H
#define VEGICIDE_LOGIC_H

#include "input.h"
#include "level.h"
#include "scene.h"

#include "audio/audio-frame.h"

struct Logic {
	SceneEcs scene;
	Level level;
	
	Logic (const Level & l);
	void step (const InputFrame & input, long frames);
};

#endif
