#include "cMediator.hpp"
#include <iostream>

void cMediator::ConnectEndpointClients() {
	pCmdMngr = cCommandManager::GetCommandManager();
	pLgtMngr = cLightManager::GetLightManager();
	pMdlMngr = cModelManager::GetModelManager();
	pFilMngr = cFileManager::GetFileManager();
	pPhyMngr = cPhysicsManager::GetPhysicsManager();

	iMediatorInterface* pMediatorInterface = this;
	pCmdMngr->setMediatorPointer(pMediatorInterface);
	pLgtMngr->setMediatorPointer(pMediatorInterface);
	pMdlMngr->setMediatorPointer(pMediatorInterface);
	pPhyMngr->setMediatorPointer(pMediatorInterface);
}

sNVData cMediator::RecieveMessage(sNVData message) {
	sNVData response;
	response.status = "OK";

	// wireframe on/off/toggle
	if (message.cmd == "wireframe") {
		sNVData data;
		data.cmd = "wireframe";
		if (message.state == "enable") { data.state = "enable"; }
		else if (message.state == "disable") { data.state = "disable"; }
		else if (message.state == "toggle") { data.state = "toggle"; }
		else { response.status == "NOK"; }
		if (message.target == "")
			data.pmo = (*itModelObjects).second;
		else
			data.pmo = mpModel[message.target];
		if (pMdlMngr->RecieveMessage(data).status == "NOK")
			response.status = "NOK";
	}

	else if (message.cmd == "wireframeall") {
		sNVData data;
		data.cmd = "wireframeall";
		if (message.state == "enable") { data.state = "enable"; }
		else if (message.state == "disable") { data.state = "disable"; }
		else if (message.state == "toggle") { data.state = "toggle"; }
		else { response.status == "NOK"; }
		data.vcmo = g_vec_pGameObjects;
		if (pMdlMngr->RecieveMessage(data).status == "NOK")
			response.status = "NOK";
	}

	// visibility on/off/toggle
	else if (message.cmd == "visible") {
		if (message.subsystem == "") {
			if (isModelSelect) { message.subsystem = "model"; }
			if (isLightSelect) { message.subsystem = "light"; }
			if (isComplexSelect) { message.subsystem = "complex"; }
		}

		if (message.subsystem == "model") {
			sNVData data;
			data.cmd = "visible";
			if (message.state == "enable") { data.state = "enable"; }
			else if (message.state == "disable") { data.state = "disable"; }
			else if (message.state == "toggle") { data.state = "toggle"; }
			else { response.status = "NOK"; }
			if (message.target == "") { data.pmo = (*itModelObjects).second; }
			else { data.pmo = mpModel[message.target]; }
			if (response.status == "OK") {
				if (pMdlMngr->RecieveMessage(data).status == "NOK") { response.status == "NOK"; }
			}
		}
		else if (message.subsystem == "light") {
			sNVData data;
			data.cmd = "visible";
			if (message.state == "enable") { data.state = "enable"; }
			else if (message.state == "disable") { data.state = "disable"; }
			else if (message.state == "toggle") { data.state = "toggle"; }
			else { response.status = "NOK"; }
			if (message.target == "") { data.plo = g_vec_pLightObjects[vpLightObjectsCurrentPos]; }
			else { data.plo = mpLight[message.target]; }
			if (response.status == "OK") {
				if (pLgtMngr->RecieveMessage(data).status == "NOK") { response.status == "NOK"; }
			}
		}
		else { response.status = "NOK"; }
		return response;
	}

	// visibility all on/off/toggle
	else if (message.cmd == "visibleall") {
		if (message.subsystem == "") {
			if (isModelSelect) { message.subsystem = "model"; }
			if (isLightSelect) { message.subsystem = "light"; }
			if (isComplexSelect) { message.subsystem = "complex"; }
		}

		if (message.subsystem == "model") {
			sNVData data;
			data.cmd = "visibleall";
			if (message.state == "enable") { data.state = "enable"; }
			else if (message.state == "disable") { data.state = "disable"; }
			else if (message.state == "toggle") { data.state = "toggle"; }
			else { response.status == "NOK"; }
			data.vcmo = g_vec_pGameObjects;
			if (response.status == "OK") {
				if (pMdlMngr->RecieveMessage(data).status == "NOK") { response.status == "NOK"; }
			}
		}
		else if (message.subsystem == "light") {
			sNVData data;
			data.cmd = "visibleall";
			if (message.state == "enable") { data.state = "enable"; }
			else if (message.state == "disable") { data.state = "disable"; }
			else if (message.state == "toggle") { data.state = "toggle"; }
			else { response.status == "NOK"; }
			data.vclo = g_vec_pLightObjects;
			if (response.status == "OK") {
				if (pLgtMngr->RecieveMessage(data).status == "NOK") { response.status == "NOK"; }
			}
		}

		else { response.status = "NOK"; }
	}

	else if (message.cmd == "savefiles") {		
		if (message.target != "") {
			if (!pFilMngr->SaveData(message._models_filename, message._lights_filename, message._complex_filename))
				response.status = "NOK";
		}
		
		else {
			if (!pFilMngr->SaveData())
				response.status = "NOK";
		}
	}

	else if (message.cmd == "data") {
		if (message.state == "show") {
			if (!ShowData())
				response.status = "NOK";
		}
		else if (message.state == "hide") {
			if (!HideData())
				response.status = "NOK";
		}
		else if (message.state == "toggle") {
			if (!ToggleData())
				response.status = "NOK";
		}
		else
			response.status = "NOK";
	}
	else if (message.cmd == "debug") {
		if (message.state == "show") {
			if (!ShowDebug())
				response.status = "NOK";
		}
		else if (message.state == "hide") {
			if (!HideDebug())
				response.status = "NOK";
		}
		else if (message.state == "toggle") {
			if (!ToggleDebug())
				response.status = "NOK";
		}
		else if (message.state == "model") {
			if (!ToggleDebugModel())
				response.status = "NOK";
		}
		else if (message.state == "modelnormal") {
			if (!ToggleDebugModelNormals())
				response.status = "NOK";
		}
		else if (message.state == "modeltriangle") {
			if (!ToggleDebugModelTriangles())
				response.status = "NOK";
		}
		else if (message.state == "aabb") {
			if (!ToggleDebugAABB())
				response.status = "NOK";
		}
		else if (message.state == "aabbtriangle") {
			if (!ToggleDebugAABBTriangles())
				response.status = "NOK";
		}
		else if (message.state == "aabbnormal") {
			if (!ToggleDebugAABBNormals())
				response.status = "NOK";
		}
		else if (message.state == "collision") {
			if (!ToggleDebugCollision())
				response.status = "NOK";
		}
		else if (message.state == "lights") {
			if (!ToggleDebugLights())
				response.status = "NOK";
		}
		else
			response.status = "NOK";
	}

	else if (message.cmd == "control") {
		if (message.subsystem == "models") {
			isModelSelect = true;
			isLightSelect = false;
			isComplexSelect = false;
		}

		else if (message.subsystem == "lights") {
			isModelSelect = false;
			isLightSelect = true;
			isComplexSelect = false;
		}

		else if (message.subsystem == "complex") {
			isModelSelect = false;
			isLightSelect = false;
			isComplexSelect = true;
		}

		else
			response.status = "NOK";
	}
	
	else if (message.cmd == "create") {
		sNVData data;
		data.cmd = "create";
		if (message.subsystem == "") {
			if (isModelSelect) { message.subsystem = "model"; }
			if (isLightSelect) { message.subsystem = "light"; }
			if (isComplexSelect) { message.subsystem = "complex"; }
		}
		if (message.subsystem == "model") {
			data.subsystem = "model";
			data.mesh = message.mesh;
			data.name = message.name;
			if(response.status == "OK")
				if (pMdlMngr->RecieveMessage(data).status == "NOK") { response.status = "NOK"; }
		}
		else if (message.subsystem == "light") {
			data.subsystem = "light";
			data.name = message.name;
			if (response.status == "OK")
				if (pLgtMngr->RecieveMessage(data).status == "NOK") { response.status = "NOK"; }
		}
		else if (message.subsystem == "complex") { }
		else { response.status = "NOK"; }
	}


	else
		response.status = "NOK";

	return response;
}


sData cMediator::RecieveMessage(sData& data) {
	switch (data.getSubsystem()) {
		case COMPLEX:
			break;

		case MODELS:
			pMdlMngr->RecieveMessage(data);
			break;

		case LIGHTS:
			if (pLgtMngr->RecieveMessage(data).getResult() != OK) { data.setResult(data.getResult()); }
			break;

		case PHYSICS:
			break;

		case AUDIO:
			break;

		case ACTORS:
			break;

		default:	
			data.setResult(UNKNOWN_SUBSYSTEM);
			// should also log the error...
			break;
	}
	return data;
}


bool cMediator::ShowData() {
	if (!isDataDisplay)
		isDataDisplay = true;
	else
		return false;
	return true;
}
bool cMediator::HideData() {
	if (isDataDisplay)
		isDataDisplay = false;
	else
		return false;
	return true;
}
bool cMediator::ToggleData() { return ((isDataDisplay) ? HideData() : ShowData()); }

bool cMediator::ShowDebug() {
	if (!isDebugDisplay)
		isDebugDisplay = true;
	else
		return false;
	return true;
}
bool cMediator::HideDebug() {
	if (isDebugDisplay)
		isDebugDisplay = false;
	else
		return false;
	return true;
}
bool cMediator::ToggleDebug() { return ((isDebugDisplay) ? HideDebug() : ShowDebug()); }

bool cMediator::ToggleDebugModel() { isDebugModel = !isDebugModel; return true; }
bool cMediator::ToggleDebugModelNormals() { isDebugModelNormals = !isDebugModelNormals; return true; }
bool cMediator::ToggleDebugModelTriangles() { isDebugModelTriangles = !isDebugModelTriangles; return true; }
bool cMediator::ToggleDebugAABB() { isDebugAABB = !isDebugAABB; return true; }
bool cMediator::ToggleDebugAABBTriangles() { isDebugAABBTriangles = !isDebugAABBTriangles; return true; }
bool cMediator::ToggleDebugAABBNormals() { isDebugAABBNormals = !isDebugAABBNormals; return true; }
bool cMediator::ToggleDebugCollision() { isDebugAABBNormals = !isDebugAABBNormals; return true; }
bool cMediator::ToggleDebugLights() { isDebugLights = !isDebugLights; return true; }