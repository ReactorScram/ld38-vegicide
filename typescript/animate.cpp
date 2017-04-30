#include <cmath>

extern "C" {
	double animate (double t);
}

double animate (double t) {
	return fmod (t, 60.0) / 60.0;
}
