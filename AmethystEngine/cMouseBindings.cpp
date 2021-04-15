
#include "GLCommon.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "cCallbacks.hpp"
#include "Globals.hpp"

#include <stdio.h>

#include "cUDPClient.hpp"
#include "cProtocol.h"


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_1) {
		if (vec_Players[vpComplexObjectsCurrentPos].state == HAS_AMMO) {
			vec_Bullets[vpComplexObjectsCurrentPos].bulletObject->positionXYZ = g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetPosition();
			vec_Bullets[vpComplexObjectsCurrentPos].bulletObject->velocity = g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetVelocity() + (2000.0f * mainCamera.GetFront());
			vec_Players[vpComplexObjectsCurrentPos].inputs.push_back(SHOOT);
		}
	}

	return;
}


float mouseLastX = 320;
float mouseLastY = 240;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xoffset = xpos - mouseLastX;
	float yoffset = mouseLastY - ypos;
	mouseLastX = xpos;
	mouseLastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	if (!useFreeCam) {
		mainCamera.OffsetYaw(xoffset);
		mainCamera.OffsetPitch(yoffset);

		mainCamera.MoveFront();

		if (isComplexSelect) {
			float roll = g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetRoll();
			g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->SetOrientation(glm::quatLookAt(-mainCamera.GetFront(), mainCamera.GetUpVector()));
			g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->UpdateOrientation(glm::vec3(0.0f, 0.0f, roll));

			vec_Players[vpComplexObjectsCurrentPos].orientations.push_back(glm::vec3(mainCamera.GetYaw(), mainCamera.GetPitch(), roll));
			vec_Players[vpComplexObjectsCurrentPos].inputs.push_back(ORIENT);
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) { }