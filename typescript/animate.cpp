#include <cmath>

extern "C" {
	double animate (double t);
	char * get_msg (double t);
}

char foo[] = "Foosball";
char bar[] = "Bartender";

double animate (double t) {
	return fmod (t, 60.0) / 60.0;
}

char * get_msg (double t) {
	t = animate (t);
	
	if (t > 0.5) {
		return foo;
	}
	else {
		return bar;
	}
}
