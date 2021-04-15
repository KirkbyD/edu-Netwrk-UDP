#pragma once
#include <vector>
#include "GLCommon.hpp"
#include "cLightObject.hpp"
#include "cGameObjectFactory.hpp"
#include "iMediatorInterface.hpp"
#include "MediatorGlobals.hpp"



class cLightManager : iMediatorInterface {
	cLightManager();
	static cLightManager stonLightMngr;

	iMediatorInterface* pMediator;

	// FACTORY OBJECT TO HANDLE THE CREATION OF NEW LIGHT OBJECTS
	cGameObjectFactory _fact_game_obj;


public:
	static cLightManager* GetLightManager();



	void LoadLightsToGPU(std::vector<cLightObject*> vLightObjects, const GLuint &shaderProgID);



	// Mediator Functions
	void setMediatorPointer(iMediatorInterface* pMediator);
	virtual sNVData RecieveMessage(sNVData message);
	virtual sData RecieveMessage(sData& data);


	// SINGULAR VISIBILITY FUNCTIONS
	bool EnableVisible(cLightObject*& pLight);
	bool DisableVisible(cLightObject*& pLight);
	bool ToggleVisible(cLightObject*& pLight);

	// ALL VISIBILITY FUNCTIONS
	bool EnableVisible(std::vector<cLightObject*>& pLights);
	bool DisableVisible(std::vector<cLightObject*>& pLights);
	bool ToggleVisible(std::vector<cLightObject*>& pLights);

};
