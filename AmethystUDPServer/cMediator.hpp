#pragma once
#include "Globals.hpp"
#include "MediatorGlobals.hpp"
#include "iMediatorInterface.hpp"
#include "cModelManager.hpp"
#include "cLightManager.hpp"
#include "cCommandManager.hpp"
#include "cPhysicsManager.hpp"

// NOT PART OF MEDIATOR FIGURE OUT HOW TO CALL THIS WITHOUT IT NEEDING TO BE HERE
// NOTE: WILL BE MOVED OUT IF THE SAME ISSUE HAPPENS WITH THIS THAT HAPPENED WITH THE OTHER COMMANDS FOR OBJECT CONTROL
#include "cFileManager.hpp"

// Add additional includes here for any other subsystem that uses the mediator.

class cMediator : public iMediatorInterface {
	bool ShowData();
	bool HideData();
	bool ToggleData();

	bool ShowDebug();
	bool HideDebug();
	bool ToggleDebug();
	bool ToggleDebugModel();
	bool ToggleDebugModelNormals();
	bool ToggleDebugModelTriangles();
	bool ToggleDebugAABB();
	bool ToggleDebugAABBTriangles();
	bool ToggleDebugAABBNormals();
	bool ToggleDebugCollision();
	bool ToggleDebugLights();
public:
	cCommandManager* pCmdMngr;
	cLightManager* pLgtMngr;
	cModelManager* pMdlMngr;
	cFileManager* pFilMngr;
	cPhysicsManager* pPhyMngr;

	// RENAME THIS NO IDEA WHAT IT SHOULD BE!!!!
	void ConnectEndpointClients();

	// this will be needed later when the subsystems are interacting with the world and need to send messages to the world and
	// subsequently other subsystems in the engine.
	// void GenerateWorld();

	virtual sNVData RecieveMessage(sNVData message);
	virtual sData RecieveMessage(sData& data);

	// Any Game Logic to be added below here
	// vector of game objects to comnmunicate with one and other.
};
