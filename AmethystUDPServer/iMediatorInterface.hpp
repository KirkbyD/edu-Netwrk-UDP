#pragma once
#include "sNVData.hpp"
#include "sData.hpp"

enum cmdSubsystem { COMPLEX, MODELS, LIGHTS, PHYSICS, AUDIO, ACTORS };												// SUBSYSTEM TYPE
// CMD TYPE
enum cmdType {
	// Basic Commands [LUA]
	MOVE, ROTATE, FOLLOW, TRIGGER,
	// Command Line Commands [DEV_CON]
	SELECT, ENABLE, DISABLE, CREATE,
	// Internal Commands [GETS]
	GET_POSITION, GET_ORIENTATION,
	// Internal Commands [SETS]
	SET_POSITION, SET_ORIENTATION, SET_VELOCITY
};
enum cmdErrorType { OK, NOK, UNKNOWN_SUBSYSTEM, UNKNOWN_COMMAND, INVALID_COMMAND };									// ERROR TYPE

class iMediatorInterface {
//protected:
//	enum cmdSubsystem { COMPLEX, MODELS, LIGHTS, PHYSICS, AUDIO, ACTORS };				// SUBSYSTEM TYPE
//	enum cmdType { MOVE, ROTATE, FOLLOW, TRIGGER, SELECT, ENABLE, DISABLE };			// CMD TYPE
//	enum cmdErrorType { OK, NOK };														// ERROR TYPE

	// directional enumeration ( up can be used for jumping, flying & swimming ) ( down can be used for flying & swimming )
	// Forward = w, Backward = s, Left = a, Right = d, up = q, down = e
	// this will need to change as q and e are typically strifing bound
	// these can be combined to go in a specific direction i.e: FORWARD | LEFT this should go forward and to the left
	// enum cmdDirection { FORWARD = 1, BACKWARD = 2, LEFT = 4, RIGHT = 8, UP = 16, DOWN = 32 };

	// command type from source ( this is for any predefined commands, like user input, etc. that can be executed without additional input ).
	// enum cmdSourceType { PATROL_AREA=1, ARM_WEAPONS };

public:
	virtual ~iMediatorInterface() { };
	virtual sNVData RecieveMessage(sNVData message) = 0;
	virtual sData RecieveMessage(sData& data) = 0;
};
