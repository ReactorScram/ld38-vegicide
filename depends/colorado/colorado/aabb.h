#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>

namespace Colorado {
class AABB {
public:
	glm::vec3 min;
	glm::vec3 max;
};
}

#endif // AABB_H
