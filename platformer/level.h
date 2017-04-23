#ifndef VEGICIDE_MAP_H
#define VEGICIDE_MAP_H

#include "stdint.h"
#include <vector>

struct Level {
	int16_t width;
	int16_t height;
	std::vector <int16_t> data;
	
	Level ();
	Level (const std::vector <uint8_t> & bytes);
	
	std::vector <uint8_t> to_iqm () const;
};

#endif
