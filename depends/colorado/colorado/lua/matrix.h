#ifndef LUAMATRIX_H
#define LUAMATRIX_H

#include "colorado/dllspec.h"

#include <glm/glm.hpp>

struct lua_State;

namespace Colorado {

COLORADO_DLLSPEC void registerColoradoMatrix (lua_State *);

// Matrix userdata
glm::mat4 * toMatrix (lua_State *, int idx = -1);
void pushMatrix (lua_State *, glm::mat4 *);
int Matrix_new (lua_State *);
int Matrix__gc (lua_State *);
int Matrix__mul (lua_State *);
int Matrix_frustum (lua_State *);
int Matrix_map (lua_State *);
int Matrix_rotate (lua_State *);
int Matrix_scale1 (lua_State *);
int Matrix_scale3 (lua_State *);
int Matrix_translate (lua_State *);

}

#endif // LUAMATRIX_H
