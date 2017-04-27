#ifndef COLORADO_LUA_GL_H
#define COLORADO_LUA_GL_H

#include "colorado/dllspec.h"

struct lua_State;

namespace Colorado {
namespace Lua {

COLORADO_DLLSPEC int registerGlLib (lua_State *);

}
}

#endif // GL_H
