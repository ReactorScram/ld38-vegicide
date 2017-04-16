#include "ecs.h"

Entity Ecs::add_entity () {
	auto e = next_entity;
	next_entity++;
	return e;
}
