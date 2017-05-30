#ifndef FIXEDTIMESTEP_H
#define FIXEDTIMESTEP_H

#include "colorado/dllspec.h"

namespace Colorado {

// Fixed timestep class a la Gaffer On Games that uses integral milliseconds
class COLORADO_DLLSPEC FixedTimestep {
public:
	int accumulator;
	int fpsNum;
	int fpsDen;
	int maxSteps;
	
	FixedTimestep (int num, int den);
	
	int step (int dt);
};

}

#endif // FIXEDTIMESTEP_H
