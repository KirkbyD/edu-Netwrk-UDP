#pragma once
#include "iMediatorInterface.hpp"
#include "iCommand.hpp"
#include "cMove.hpp"
#include "cRotate.hpp"
#include <iostream>
#include <map>

// DEVCON GLOBALS (MAIN)
extern std::string _cmd_result;




class cCommandManager : iMediatorInterface {
	cCommandManager();
	static cCommandManager stonCmdMngr;

	iMediatorInterface* pMediator;

	void ConvertToLower(std::string s);

	std::string _cmdstr;

	// mainly for devcon now???
	std::vector<std::string> vec_move_to_cmd_strs = {"moveto" , "MOVETO", "movetoxyz", "MOVETOXYZ" };
	std::vector<std::string> vec_rotate_to_cmd_strs = {"rotateto" , "ROTATETO", "rotatetoxyz", "ROTATETOXYZ" };
	std::map<std::vector<std::string>, cmdType> m_str_to_enum;

	// Make Command Factory???
	// 1)	MoveTo - does xyz translate, facade to MoveUp, MoveDown, MoveRight, MoveLeft, MoveForward, MoveBackward
	// 2)	OrientTo - does xyz rotation (using quats), facade to RotateUp, RotateDown, RotateLeft, RotateRight, RotateForward, RotateBackward, FaceDirection
	// 3)	Follow - Curvre & Object, Curve: Waypoint / Bezier | Object: distance, offset, following target, following source, max follow speed, acceleration / deceleration range.
	// 4)	Trigger - Location: trigger an event at the specific location.
	// 5)	CommandGroups - Serial: execute in order | Parallel: execute simotaneously
	

	// CommandTypes: Repeated, Finite
	// UniqueID, Type, FriendlyName,
	// EaseIn/EaseOut Required.
	// EaseIn / EaseOut will be time based and percent based of that time??? if possible
	// Camera needs to be controllable via command ( implement the subsystem.control cmd to account for user input as well )


	// If the parallel command contains a serial command the serial commands must execute in parallel, meaning that a vector or isDones, etc. is required
	// each command within the parallel group must be executed simotaneously in order for it to be a true "parallel" command, to do this with parallel serial commands
	// each serial command must run once until the end of the serial command, a check or something need to be implemented in order to check to see if all the parallel serial commands
	// have been executed, in addition if there is another seerial command qued after the parallel serial command then a check is needed to see if it affects a finished object
	// if the object has finished then the execution of that command can continue however only if the serial commands affect the specific object in question; this can continue on any object if
	// all the parallel commands on all other objects have finished and the object is infact changing.  This functionality may only be applicable to one layer deep parallel serial commands
	// commands can be serial parallel or parallel serial or any further combination of these.
	// Parallel commands to reduce retyping should have a special interface which allows for the targeting of multiple objects for the same command.  Ideally Serial commands should possess
	// the same capability, issue these commands to these objects; if this is done then the commands of the objects would be issued in parallel even if it is just a serial command.
	// to accomplish this each command must beable to handle a vector of objects, the vector will contain all the objects associated with the command, to reduce complications these commands
	// will typically only target objects, or potentially complex objects will need to discuss this more later.
	// Ask Feeney exactly how far down this murderous rabbit hole we actually need to go before it's "too much" to worry about and is unlikely to ever happen.

	// Also change the serial & parallel command vectors to queues instead or just serials to queues...

	// Commands passed in will use the name of the command as a friendly name, this will be translated into the command type enum
	// if the command does not exist an error should be returned stating that the command is unknown
	// a command group might have a friendlyname to identify it from other commands.
	// the lua brain will handle all the nessisary conversions between lua script and c++ and will call he command managers issue command method passing the data
	// trigger commands should likely be processed first as they are specific animations that happen under specific events
	// all other animations can be processed afterwords unless otherwise specified later.

	// should beable to parse the lua script input into lower case characters then search for the command
	// if at all possible make a multi-map which identifies multiple different strings to be associated with a singular command type.


public:
	iCommand* cmdMove;
	iCommand* cmdRotate;

	static cCommandManager* GetCommandManager();

	void IssueCommand(std::string cmd);
	void IssueCommand(sData& cmd_data);
	void setMediatorPointer(iMediatorInterface* pMediator);

	void CreateCommands();
	iCommand* CreateAnimScript();
	iCommand* CreateCommandGroup();
	iCommand* CreateCommand(cmdType cmd_type, sData data);


	virtual sNVData RecieveMessage(sNVData message);
	virtual sData RecieveMessage(sData& data);
};
