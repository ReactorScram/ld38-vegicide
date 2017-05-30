#ifndef LUACOMMON_H
#define LUACOMMON_H

#include "colorado/dllspec.h"

struct lua_State;

namespace Colorado {
namespace Lua {

struct COLORADO_DLLSPEC KeyValueInt {
	const char * key;
	int value;
};

COLORADO_DLLSPEC int registerColorado (lua_State *);

// Registers a list of integers (enums, usually) with the table on top of the stack
COLORADO_DLLSPEC void registerInts (lua_State *, KeyValueInt []);

}
}

#endif // LUACOMMON_H
