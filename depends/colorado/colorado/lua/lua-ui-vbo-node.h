#ifndef COLORADO_LUA_UI_VBO_NODE_H
#define COLORADO_LUA_UI_VBO_NODE_H

#include "colorado/dllspec.h"

struct lua_State;

namespace Colorado {
namespace Lua {

COLORADO_DLLSPEC int registerUiVboNodeLib (lua_State *);

}
}

#endif
