#ifndef LUASTATE_H
#define LUASTATE_H

#include "lua.hpp"

#include "colorado/dllspec.h"
#include "colorado/lua-5.1-helper.h"

#include <string>
#include <vector>

namespace Colorado {
class COLORADO_DLLSPEC LuaState {
public:
	bool ownsState;
	lua_State * ls;
	
	LuaState ();
	LuaState (lua_State *);
	LuaState (const LuaState &) = delete;
	LuaState & operator = (const LuaState &) = delete;
	~LuaState ();
	
	// None of these functions should mutate LuaState, but
	// since they mutate the underlying lua_State, they should
	// not be const
	
	bool loadBuffer (const std::vector <unsigned char> &, int args = 0, int results = 0);
	bool loadFile (const char *, int args = 0, int results = 0);
	
	int pcall (int, int);
	
	// For config files
	bool openTable (const char *);
	bool openTable (int);
	void closeTable ();
	
	void read (const char *, bool &);
	void read (const char *, float &);
	void read (const char *, int &);
	
	std::string readString (int idx = -1);
	std::string readString (const char *);
	
	void setTable (const char *, bool, int index = -1);
	void setTable (const char *, int, int index = -1);
	void setTable (const char *, double, int index  = -1);
};
}

#endif // LUASTATE_H
