#include "cObjectCamera.hpp"

cObjectCamera::cObjectCamera() {
	this->yaw = 0.0f;
	this->pitch = 0.0f;
	this->followDistance = 1.0f;

	this->eye = glm::vec3(0.0f, 0.0f, 0.0f);
	this->previousPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	this->front = glm::vec3(0.0f, 0.0f, 1.0f);
	this->upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);
	this->viewMatrix = glm::mat4(1.0f);

	this->followObject = NULL;
}

glm::vec3 cObjectCamera::GetEye() {
	return this->eye;
}

glm::vec3 cObjectCamera::GetUpVector() {
	return this->upVector;
}

glm::vec3 cObjectCamera::GetFront() {
	return this->front;
}

glm::vec3 cObjectCamera::GetRight()
{
	return this->right;
}

glm::vec3 cObjectCamera::GetUp()
{
	return up;
}

glm::mat4 cObjectCamera::GetViewMatrix()
{
	this->viewMatrix = glm::lookAt(this->eye,
		this->eye + this->front,
		this->upVector);

	return this->viewMatrix;
}

float cObjectCamera::GetYaw()
{
	return this->yaw;
}

float cObjectCamera::GetPitch()
{
	return this->pitch;
}

void cObjectCamera::SetUpVector(glm::vec3 upVector) {
	this->upVector = upVector;
}

void cObjectCamera::SetObject(iGameObject* newFollowTarget)
{
	this->followObject = newFollowTarget;
}

void cObjectCamera::SetFollowDistance(float distanceMultiplier)
{
	this->followDistance = distanceMultiplier;
}

void cObjectCamera::MoveForBack(float speed)
{
	this->eye += this->front * speed;
}

void cObjectCamera::MoveUpDown(float speed)
{
	this->eye += this->up * speed;
}

void cObjectCamera::MoveStrafe(float speed)
{
	this->eye += this->right * speed;
}

void cObjectCamera::Move() {
	this->previousPosition = this->eye;
	this->eye = this->followObject->getPosition();

	this->eye += this->front * -5.0f * followDistance;
	this->eye += this->up * 0.5f * followDistance;
	//this->eye += this->right * -2.0f;
}

void cObjectCamera::MoveFront() {
	this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	this->front.y = sin(glm::radians(this->pitch));
	this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	this->front = glm::normalize(this->front);

	this->right = glm::normalize(glm::cross(this->upVector, this->front));
	this->up = glm::cross(this->front, this->right);
}

void cObjectCamera::OffsetYaw(float offset) {
	this->yaw = glm::mod(this->yaw + offset, 360.0f);
}

void cObjectCamera::OffsetPitch(float offset) {
	this->pitch += offset;
	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;
}