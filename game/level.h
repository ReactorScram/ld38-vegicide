#ifndef VEGICIDE_MAP_H
#define VEGICIDE_MAP_H

#include "stdint.h"
#include <string>
#include <vector>

struct LevelObj {
	int64_t id;
	std::string name;
	std::string type;
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
};

struct Level {
	int16_t width;
	int16_t height;
	std::vector <int16_t> data;
	std::vector <LevelObj> objects;
	
	Level ();
	Level (const std::vector <uint8_t> & bytes);
	
	void load_sqlite_objects (std::string db_name);
	
	std::vector <uint8_t> to_iqm () const;
};

#endif
