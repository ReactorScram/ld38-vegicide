#include "level.h"

using namespace std;

Level::Level (): width (0), height (0), data () {}

Level::Level (const std::vector <uint8_t> & bytes) {
	width = *(int16_t *)&bytes [0];
	height = *(int16_t *)&bytes [2];
	int size = width * height;
	
	data = vector <int16_t> ((int16_t *)&bytes [4], (int16_t *)&bytes [4 + 2 * size]);
}
