#include "GLCommon.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "cCallbacks.hpp"
#include "cCommandManager.hpp"
#include "Globals.hpp"

#include <stdio.h>
#include <iostream>

#include "cUDPClient.hpp"
#include "cProtocol.h"

extern GLFWwindow* _text_window;
extern GLFWwindow* _window;

bool isShiftKeyDownByAlone(int mods); 
bool isCtrlKeyDownByAlone(int mods);

// ADDITIONALLY CHANGE THE DEVELOPER CONSOLE TO USE A SECONDARY WINDOW WITH THE STUFF FROM OSCARS CLASS
// FUCNTION FOR ACTUALLY ISSUING THE COMMAND (THIS IS ESSENTIALLY THE MEDIATOR)
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (!isDevCon && !isShiftKeyDownByAlone(mods) && !isCtrlKeyDownByAlone(mods) && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		if (isObjCtrl) {
			if (isComplexSelect) {
				float accelSpeed = 180.0f * deltaTime;
				glm::vec3 velocity = g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetVelocity();
				if (key == GLFW_KEY_A) {
					g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetVelocity(glm::vec3(velocity.x + mainCamera.GetRight().x * accelSpeed,
						velocity.y,
						velocity.z + mainCamera.GetRight().z * accelSpeed));
					vec_Players[vpComplexObjectsCurrentPos].inputs.push_back(LEFT);
				}
				if (key == GLFW_KEY_D) {
					g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetVelocity(glm::vec3(velocity.x - mainCamera.GetRight().x * accelSpeed,
						velocity.y,
						velocity.z - mainCamera.GetRight().z * accelSpeed));
					vec_Players[vpComplexObjectsCurrentPos].inputs.push_back(RIGHT);
				}
				if (key == GLFW_KEY_SPACE) {
					vec_Players[vpComplexObjectsCurrentPos].inputs.push_back(BRAKE);
					g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetVelocity(velocity * 0.5f);
				}
				if (key == GLFW_KEY_W) {
					g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetVelocity(glm::vec3(velocity.x + mainCamera.GetFront().x * accelSpeed,
						velocity.y + mainCamera.GetFront().y * accelSpeed,
						velocity.z + mainCamera.GetFront().z * accelSpeed));
					vec_Players[vpComplexObjectsCurrentPos].inputs.push_back(FORWARD);
				}
				if (key == GLFW_KEY_S) {
					g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetVelocity(glm::vec3(velocity.x - mainCamera.GetFront().x * accelSpeed,
						velocity.y - mainCamera.GetFront().y * accelSpeed,
						velocity.z - mainCamera.GetFront().z * accelSpeed));
					vec_Players[vpComplexObjectsCurrentPos].inputs.push_back(BACKWARD);
				}
				if (key == GLFW_KEY_R) {
					vec_Players[vpComplexObjectsCurrentPos].inputs.push_back(RESPAWN);
				}
				/*if (key == GLFW_KEY_Q) {
					g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->UpdateRoll(1.0f);
					vec_Players[vpComplexObjectsCurrentPos].inputs.push_back(ORIENT);
					vec_Players[vpComplexObjectsCurrentPos].orientations.push_back(g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->getEulerAngle());
				}
				if (key == GLFW_KEY_E) {
					g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->UpdateRoll(-1.0f);
					vec_Players[vpComplexObjectsCurrentPos].inputs.push_back(ORIENT);
					vec_Players[vpComplexObjectsCurrentPos].orientations.push_back(g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->getEulerAngle());
				}*/
			}
		}
	}

	if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS) {
		if (g_vec_pComplexObjects.size() != 0) {
			isModelSelect = false;
			isLightSelect = false;
			isComplexSelect = true;
			useFreeCam = false;
			isObjCtrl = true;

			mainCamera.SetObject(g_vec_pComplexObjects[vpComplexObjectsCurrentPos]);
			mainCamera.SetFollowDistance(15.0f);
		}
	}

	if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS) {
		pFindObjectByFriendlyName("PredictedPos")->isVisible = !pFindObjectByFriendlyName("PredictedPos")->isVisible;
		pFindObjectByFriendlyName("DeadReckoned01")->isVisible = !pFindObjectByFriendlyName("DeadReckoned01")->isVisible;
		pFindObjectByFriendlyName("DeadReckoned02")->isVisible = !pFindObjectByFriendlyName("DeadReckoned02")->isVisible;
		pFindObjectByFriendlyName("DeadReckoned03")->isVisible = !pFindObjectByFriendlyName("DeadReckoned03")->isVisible;
		pFindObjectByFriendlyName("DeadReckoned04")->isVisible = !pFindObjectByFriendlyName("DeadReckoned04")->isVisible;
		pFindObjectByFriendlyName("ServerPos01")->isVisible = !pFindObjectByFriendlyName("ServerPos01")->isVisible;
		pFindObjectByFriendlyName("ServerPos02")->isVisible = !pFindObjectByFriendlyName("ServerPos02")->isVisible;
		pFindObjectByFriendlyName("ServerPos03")->isVisible = !pFindObjectByFriendlyName("ServerPos03")->isVisible;
		pFindObjectByFriendlyName("ServerPos04")->isVisible = !pFindObjectByFriendlyName("ServerPos04")->isVisible;
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		useFreeCam = true;
		isObjCtrl = false;

		FreeCamera.SetEye(glm::vec3(0.0f, 0.0f, 0.0f));
		FreeCamera.SetEye(glm::vec3(1250.0f, 3500.0f, 1500.0f));
		FreeCamera.SetTarget(pFindObjectByFriendlyName("Puget Sound"));
		FreeCamera.MoveFront();
	}

}

bool isShiftKeyDownByAlone(int mods) {
	if (mods == GLFW_MOD_SHIFT)
		return true;
	return false;
}

bool isCtrlKeyDownByAlone(int mods) {
	if (mods == GLFW_MOD_CONTROL)
		return true;
	return false;
}

void character_callback(GLFWwindow* window, unsigned codepoint) {
	// STUFF FOR THE DEVELOPER CONSOLE
	if ((char)codepoint != '`')
		cmdstr.push_back((char)codepoint);
}