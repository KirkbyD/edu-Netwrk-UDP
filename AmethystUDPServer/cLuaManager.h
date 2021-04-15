#pragma once

extern "C" {
	#include <Lua5.3.5/lua.h>
	#include <Lua5.3.5/lauxlib.h>
	#include <Lua5.3.5/lualib.h>
}

#include <string>
#include <map>
#include "cCommandManager.hpp"



class cLuaManager {
	std::map<std::string, std::string> m_script_name_to_source;
	lua_State* _pLuaState;
	std::string luaErrorToString(int error);

	// static required variables
	static sData data;
	static std::map<std::string, size_t> mapSubsystems;

public:
	cLuaManager();
	~cLuaManager();

	// loading/unloading scripts
	void LoadScript(std::string name, std::string source);
	void DeleteScript(std::string name);

	// running scripts
	void ExecuteScript(std::string script);

	// subsystem population
	void populateSubsystemMap();

	// actual script called methods
	// lua script function examples, these are in feeneys code and are what the lua scripts call.
	static int l_MoveToCmd(lua_State *L);
	static int l_RotateToCmd(lua_State* L);
	static int l_setSubsystem(lua_State* L);
	static int l_setTarget(lua_State* L);
	static int l_setEndPos(lua_State* L);
	static int l_setEndOrientation(lua_State* L);
	static int l_setDuration(lua_State* L);
	static int l_setEaseInOut(lua_State* L);
};
