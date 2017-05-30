#include "colorado/fixed-timestep.h"

namespace Colorado {

FixedTimestep::FixedTimestep (int num, int den) {
	accumulator = 0;
	fpsNum = num;
	// Convert to seconds
	fpsDen = den * 1000;
	
	maxSteps = 4;
}

int FixedTimestep::step (int dt) {
	accumulator += dt * fpsNum;
	
	int result = 0;
	
	for (int i = 0; i < maxSteps; i++) {
		if (accumulator >= 0) {
			accumulator -= fpsDen;
			
			result++;
		}
		
		if (accumulator <= 0) {
			break;
		}
	}
	
	if (accumulator > 0) {
		accumulator = 0;
	}
	
	return result;
}

}
