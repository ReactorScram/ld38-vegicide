#ifndef LUAMATRIX_H
#define LUAMATRIX_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "colorado/dllspec.h"

struct lua_State;

namespace Colorado {
namespace Lua {

COLORADO_DLLSPEC void registerColoradoMatrix (lua_State *);

// Matrix userdata
glm::mat4 * toMatrix (lua_State *, int idx = -1);
void pushMatrix (lua_State *, glm::mat4 *);
COLORADO_DLLSPEC int Matrix_new (lua_State *);

glm::quat * toQuat (lua_State *, int idx = -1);
void pushQuat (lua_State *, glm::quat *);
COLORADO_DLLSPEC int Quat_new (lua_State *);

}
}

#endif // LUAMATRIX_H
