#ifndef LUA5_1HELPER_H
#define LUA5_1HELPER_H

#include "colorado/dllspec.h"
#include "lua.hpp"

struct lua_State;
struct luaL_Reg;

#define LUA_OK 0
#define lua_rawlen lua_objlen

#define luaL_newlib(L,l) (luaL_newlibtable(L,l), luaL_setfuncs(L,l,0))

void COLORADO_DLLSPEC luaL_newlibtable (lua_State *, const luaL_Reg []);

// int nup is not implemented
void COLORADO_DLLSPEC luaL_setfuncs (lua_State *, const luaL_Reg *, int);
void COLORADO_DLLSPEC luaL_setmetatable (lua_State *, const char *);

#endif // LUA5_1HELPER_H
