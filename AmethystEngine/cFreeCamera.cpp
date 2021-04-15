#include "cFreeCamera.hpp"

cFreeCamera::cFreeCamera() {
	this->yaw = 0.0f;
	this->pitch = 0.0f;

	this->eye = glm::vec3(0.0f, 0.0f, 0.0f);
	this->previousPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	this->front = glm::vec3(0.0f, 0.0f, 1.0f);
	this->upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);
	this->viewMatrix = glm::mat4(1.0f);

	this->lookAtObject = NULL;
}

glm::vec3 cFreeCamera::GetEye() {
	return this->eye;
}

glm::vec3 cFreeCamera::GetUpVector() {
	return this->upVector;
}

glm::vec3 cFreeCamera::GetFront() {
	return this->front;
}

void cFreeCamera::SetTarget(iGameObject* newLookAtTarget) {
	this->lookAtObject = newLookAtTarget;
}

iGameObject* cFreeCamera::GetLookAtTarget() {
	return this->lookAtObject;
}

float cFreeCamera::GetYaw()
{
	return this->yaw;
}

float cFreeCamera::GetPitch()
{
	return this->pitch;
}

glm::mat4 cFreeCamera::GetViewMatrix() {
	this->viewMatrix = glm::lookAt(this->eye,
		this->eye + this->front,
		this->upVector);

	return this->viewMatrix;
}

void cFreeCamera::SetEye(glm::vec3 location) {
	this->eye = location;
}

void cFreeCamera::SetUpVector(glm::vec3 upVector) {
	this->upVector = upVector;
}

void cFreeCamera::MoveForBack(float speed) {
	this->eye += this->front * speed;
}

void cFreeCamera::MoveUpDown(float speed) {
	this->eye += this->up * speed;
}

void cFreeCamera::MoveStrafe(float speed) {
	this->eye += this->right * speed;
}

void cFreeCamera::MoveFront() {
	if (this->lookAtObject != NULL) {
		this->front = glm::normalize(this->lookAtObject->getPosition() - this->eye);
	}
	else {
		this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front.y = sin(glm::radians(this->pitch));
		this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
		this->front = glm::normalize(this->front);
	}
	this->front = glm::normalize(this->front);

	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);
}

void cFreeCamera::OffsetYaw(float offset) {
	//Constrains the yaw to within 360
	this->yaw = glm::mod(this->yaw + offset, 360.0f);
}

void cFreeCamera::OffsetPitch(float offset) {
	this->pitch += offset;
	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;
}