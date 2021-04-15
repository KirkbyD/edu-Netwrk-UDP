#include <iostream>
#include "cLuaManager.h"

cLuaManager::cLuaManager() {
	this->_pLuaState = luaL_newstate();

	luaL_openlibs(this->_pLuaState);

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_MoveToCmd);
	lua_setglobal(this->_pLuaState, "moveto");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_RotateToCmd);
	lua_setglobal(this->_pLuaState, "rotateto");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setSubsystem);
	lua_setglobal(this->_pLuaState, "setsubsystem");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setTarget);
	lua_setglobal(this->_pLuaState, "settarget");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setEndPos);
	lua_setglobal(this->_pLuaState, "setendposition");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setEndOrientation);
	lua_setglobal(this->_pLuaState, "setendorientation");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setDuration);
	lua_setglobal(this->_pLuaState, "setduration");

	lua_pushcfunction(this->_pLuaState, cLuaManager::l_setEaseInOut);
	lua_setglobal(this->_pLuaState, "seteaseinout");

	return;
}

cLuaManager::~cLuaManager() {
	lua_close(this->_pLuaState);
	return;
}


// note this overrides like named lua scripts with the last script passed to it.
void cLuaManager::LoadScript(std::string name, std::string source) {
	this->m_script_name_to_source[name] = source;
	return;
}


void cLuaManager::DeleteScript(std::string name) {
	// TODO: Delete the script...
	return;
}



std::string cLuaManager::luaErrorToString(int error) {
	switch (error) {
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error";
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	default:
		return "Lua: UNKNOWN error";
		break;
	}
}



// run this will run the passed script
void cLuaManager::ExecuteScript(std::string script) {
	int error = luaL_loadstring(this->_pLuaState, script.c_str());

	// 0 -> no error
	if (error != 0) {
		// move this to error class once lua script is working.
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << this->luaErrorToString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
	}
	// execute funtion in "protected mode", where problems are caught and placed on the stack for investigation
	// args(lua state, number of arguments, number of results, location in stack) if 0 results are on top of stack
	error = lua_pcall(this->_pLuaState, 0, 0, 0);

	// 0 -> no error
	if (error != 0) {
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << this->luaErrorToString(error) << std::endl;

		std::string luaError;

		// Get error information from top of stack (-1 is top)
		luaError.append(lua_tostring(this->_pLuaState, -1));

		// Make error message a little more clear
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		// We passed zero (0) as errfunc, so error is on stack) & pop error message from the stack
		lua_pop(this->_pLuaState, 1);
	}
	return;
};


// update will run all the scripts in a map
// this could be done using an unsorted map, as the scripts would need to get executed in a specific order.


void cLuaManager::populateSubsystemMap() {
	mapSubsystems["models"] = MODELS;
	mapSubsystems["lights"] = LIGHTS;
	mapSubsystems["audio"] = AUDIO;
	mapSubsystems["physics"] = PHYSICS;
	mapSubsystems["actors"] = ACTORS;
	mapSubsystems["complex"] = COMPLEX;
}



// lua script setter commands for the command data...
// function to set the subsystem, if not called the default is 0 (COMPLEX)
int cLuaManager::l_setSubsystem(lua_State* L) {
	bool successful = true;
	std::string subsystem = lua_tostring(L, 1);

	if (mapSubsystems.find(subsystem) != mapSubsystems.end())
		data.setSubsystem(mapSubsystems[subsystem]);
	else
		successful = false;			// oh no!! bad input naughty user!

	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setTarget(lua_State* L) {
	bool successful = true;
	data.setTarget(lua_tostring(L, 1));
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setEndPos(lua_State* L) {
	bool successful = true;
	size_t l_stackSize = lua_tointeger(L, 1);
	if (l_stackSize == 3)
		data.addVec4Data(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
	else if(l_stackSize == 4)
		data.addVec4Data(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
	else
		successful = false;			// oh no!! bad input naughty user!
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setEndOrientation(lua_State* L) {
	bool successful = true;
	data.addVec4Data(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
	//successful = false;			// oh no!! bad input naughty user!
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setDuration(lua_State* L) {
	bool successful = true;
	data.addFloatData(lua_tonumber(L, 1));
	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_setEaseInOut(lua_State* L) {
	bool successful = true;
	data.setEase(true, lua_tostring(L, 1));
	data.addFloatData(lua_tonumber(L, 2));
	data.addFloatData(lua_tonumber(L, 3));
	lua_pushboolean(L, successful);
	return 1;
}




int cLuaManager::l_MoveToCmd(lua_State* L) {
	cCommandManager* pcmdmngr = pcmdmngr->GetCommandManager();
	data.setCMD(MOVE);

	// potentially make this variable a static vector of bools which if one is false return false???
	bool successful = true;
	

	/*
	i think how this would look would be this in the lua script:
	// should probably have the major overarching command structure:
		animscript(
			gscmd(
				moveto(
					settarget(
						"DESTROY"
					),
					setendpos(
						numofarguments,
						150.0f,
						100.0f,
						50.0f,
						1.0f
					),
					setduration(
						1000.0f
					),
					setsystem(
						"models"
					)
				),
				gpcmd(
					rotateto(
						//stuff here
					)
				)
			)
		);
	*/
	pcmdmngr->CreateCommand(MOVE, data);

	lua_pushboolean(L, successful);
	return 1;
}

int cLuaManager::l_RotateToCmd(lua_State* L) {
	cCommandManager* pcmdmngr = pcmdmngr->GetCommandManager();
	data.setCMD(ROTATE);

	// potentially make this variable a static vector of bools which if one is false return false???
	bool successful = true;


	/*
	i think how this would look would be this in the lua script:
	// should probably have the major overarching command structure:
		animscript(
			gscmd(
				moveto(
					settarget(
						"DESTROY"
					),
					setendpos(
						numofarguments,
						150.0f,
						100.0f,
						50.0f,
						1.0f
					),
					setduration(
						1000.0f
					),
					setsystem(
						"models"
					)
				),
				gpcmd(
					rotateto(
						//stuff here
					)
				)
			)
		);
	*/
	pcmdmngr->CreateCommand(ROTATE, data);

	lua_pushboolean(L, successful);
	return 1;
}



// static variables annoying ship stuff!!!!!!!
sData cLuaManager::data;
std::map<std::string, size_t> cLuaManager::mapSubsystems;
