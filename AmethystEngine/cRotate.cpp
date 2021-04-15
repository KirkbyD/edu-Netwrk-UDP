#include "cRotate.hpp"

void cRotate::Init(sData data) {
	this->_subsystem = data.getSubsystem();
	this->_target = data.getTarget();

	glm::vec3 EulerAngleEnd;
	EulerAngleEnd.x = glm::radians(data.getVec4Data()[0].x);
	EulerAngleEnd.y = glm::radians(data.getVec4Data()[0].y);
	EulerAngleEnd.z = glm::radians(data.getVec4Data()[0].z);
	this->_end_orientation = glm::quat(EulerAngleEnd);

	if (data.getVec4Data().size() == 2) { _isRelative = false; }
	if (!_isRelative) {
		glm::vec3 EulerAngle;
		EulerAngle.x = glm::radians(data.getVec4Data()[1].x);
		EulerAngle.y = glm::radians(data.getVec4Data()[1].y);
		EulerAngle.z = glm::radians(data.getVec4Data()[1].z);
		_start_orientation = glm::quat(EulerAngle);
	}

	this->_time = data.getFloatData()[0];
}

bool cRotate::Update(double deltatime) {
	cCommandManager* pcmdmngr = cCommandManager::GetCommandManager();

	if (!this->_UpdateHasBeenCalled) {
		if (this->_isRelative) {
			sData data;
			data.setSubsystem(_subsystem);
			data.setCMD(GET_ORIENTATION);
			data.setTarget(_target);
			pcmdmngr->IssueCommand(data);
			this->_start_orientation = glm::quat(data.getQuatData()[0]);
		}

		this->_UpdateHasBeenCalled = true;
	}

	float AmountOfRotationComplete = (float)this->_time_elapsed / this->_time;

	glm::quat qCurrentOrientation = glm::slerp(this->_start_orientation, this->_end_orientation, AmountOfRotationComplete);

	sData data;
	data.setSubsystem(_subsystem);
	data.setCMD(SET_ORIENTATION);
	data.setTarget(_target);
	data.setQuatData(qCurrentOrientation);
	pcmdmngr->IssueCommand(data);

	this->_time_elapsed += deltatime;

	if (this->_time_elapsed >= this->_time) {
		return true;
	}
	return false;
}

float cRotate::SmootherStep(float startdist, float enddist, float currdist) {
	return 0.0f;
}

float cRotate::Clamp(float x) {
	return 0.0f;
}
