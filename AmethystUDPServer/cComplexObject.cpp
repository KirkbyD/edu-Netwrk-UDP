#include "cComplexObject.hpp"
#include "Globals.hpp"

cComplexObject::cComplexObject(std::vector<cComplexObject*> &vObjects) {
	// ************************************
	// Interface
	this->friendlyIDNumber = 0;
	this->friendlyName = "";

	this->isVisible = true;
	this->isControl = false;

	this->m_uniqueID = next_uniqueID;

	this->parentObject = NULL;
	// ************************************

	this->scale = 1.0f;
	this->position = glm::vec3(0.0f, 0.0f, 0.0f);
	this->qRotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

	this->inverseMass = 0.0f;
	this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	this->acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

	vObjects.push_back(this);
}

cComplexObject::~cComplexObject() {
	for (cLightObject* light : lightVector)	{
		light->parentObject = NULL;
	}

	for (cModelObject* model : modelVector)	{
		model->parentObject = NULL;
	}
}

float cComplexObject::GetScale()
{
	return this->scale;
}

glm::vec3 cComplexObject::GetPosition()
{
	return this->position;
}

glm::vec3 cComplexObject::GetPreviousPosition()
{
	return this->prevPosition;
}

glm::quat cComplexObject::getQOrientation()
{
	return this->qRotation;
}

glm::vec3 cComplexObject::getEulerAngle()
{
	glm::vec3 EulerAngle = glm::eulerAngles(this->qRotation);

	return EulerAngle;
}

float cComplexObject::GetRoll(){
	return this->roll;
}

float cComplexObject::GetInverseMass()
{
	return this->inverseMass;
}

glm::vec3 cComplexObject::GetVelocity()
{
	return this->velocity;
}

glm::vec3 cComplexObject::GetAcceleration()
{
	return this->acceleration;
}

void cComplexObject::SetScale(float val) {
	this->scale = val;
	Resize();
}

void cComplexObject::SetPosition(glm::vec3 posVec) {
	glm::vec3 movementVector = posVec - this->position; 
	Move(movementVector);
}

void cComplexObject::SetPreviousPosition(glm::vec3 posVec)
{
	this->prevPosition = posVec;
}

void cComplexObject::SetInverseMass(float val) {
	this->inverseMass = val;
}

void cComplexObject::SetVelocity(glm::vec3 velocity) {
	this->velocity = velocity;

	//TODO - remove this and makesure it still works
	/*for (cModelObject* model : this->modelVector) {
		model->velocity = this->velocity;
	}*/
}

void cComplexObject::SetAcceleration(glm::vec3 accel) 
{
	this->acceleration = accel;

	//TODO - remove this and makesure it still works
	for (cModelObject* model : this->modelVector) {
		model->accel = this->acceleration;
	}
}

void cComplexObject::SetOrientation(glm::vec3 EulerAngleDegreesXYZ){
	glm::vec3 EulerAngleRadians;
	EulerAngleRadians.x = (EulerAngleDegreesXYZ.x);
	EulerAngleRadians.y = (EulerAngleDegreesXYZ.y);
	EulerAngleRadians.z = (EulerAngleDegreesXYZ.z);

	this->qRotation = glm::quat(EulerAngleRadians);
	RotateChildren();
}

void cComplexObject::SetOrientation(glm::quat qAngle){
	this->qRotation = qAngle;
	RotateChildren();
}

void cComplexObject::AddLight(std::string lightName) {
	if (mpLight.find(lightName) != mpLight.end()) {
		this->lightVector.push_back(mpLight[lightName]);
		this->lightVector.back()->parentObject = this;
		this->lightVector.back()->isControl = false;

		if (this->modelVector.size() == 0 && this->lightVector.size() == 1)
			this->position = lightVector[0]->position;

		this->lightPositions.push_back(this->lightVector.back()->getPosition());
		glm::vec3 direction = glm::rotate(this->qRotation, this->lightVector.back()->getPosition());
		this->lightVector.back()->position = glm::vec4(this->position + (direction), 1.0f);

		/*glm::vec3 Rotation = this->getEulerAngle();
		glm::vec4 lightDirection = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		lightDirection.x = cos(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
		lightDirection.y = sin(glm::radians(Rotation.y));
		lightDirection.z = sin(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
		this->lightVector.back()->direction = glm::normalize(lightDirection);*/
	}
}

void cComplexObject::AddLight(cLightObject* theLight) {
	this->lightVector.push_back(theLight);
	this->lightVector.back()->parentObject = this;
	this->lightVector.back()->isControl = false;

	if (this->modelVector.size() == 0 && this->lightVector.size() == 1)
		this->position = lightVector[0]->position;

	this->lightPositions.push_back(theLight->getPosition());
	glm::vec3 direction = glm::rotate(this->qRotation, theLight->getPosition());
	theLight->position = glm::vec4(this->position + (direction), 1.0f);


	/*glm::vec3 Rotation = this->getEulerAngle();
	glm::vec4 lightDirection = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lightDirection.x = cos(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
	lightDirection.y = sin(glm::radians(Rotation.y));
	lightDirection.z = sin(glm::radians(Rotation.x)) * cos(glm::radians(Rotation.y));
	this->lightVector.back()->direction = glm::normalize(lightDirection);*/
}

void cComplexObject::AddModel(std::string friendlyName) {
	cModelObject* theModel = pFindObjectByFriendlyName(friendlyName);
	this->modelVector.push_back(theModel);
	this->modelVector.back()->parentObject = this;
	this->modelVector.back()->isControl = false;
	this->modelVector.back()->accel = this->acceleration;
	this->modelVector.back()->velocity = this->velocity;

	if (this->modelVector.size() == 1)
		this->position = modelVector[0]->positionXYZ;
	
	this->modelPositions.push_back(theModel->getPosition() - this->position);
}

void cComplexObject::AddModel(cModelObject* theModel) {
	this->modelVector.push_back(theModel);
	this->modelVector.back()->parentObject = this;
	this->modelVector.back()->isControl = false;
	this->modelVector.back()->accel = this->acceleration;
	this->modelVector.back()->velocity = this->velocity;

	if (this->modelVector.size() == 1)
		this->position = modelVector[0]->positionXYZ;

	this->modelPositions.push_back(theModel->getPosition() - this->position);
}

void cComplexObject::AddCollisionPoint(glm::vec3 location) {
	collisionPoint thePoint;
	thePoint.RelativePos = location;
	thePoint.Position = location + this->position;
	thePoint.Collided = -1.0f;
	this->collisionPoints.push_back(thePoint);
}

bool cComplexObject::RemoveLight(cLightObject* theLight){
	for (size_t i = 0; i < lightVector.size(); i++)	{
		if (lightVector[i] = theLight) {
			lightPositions.erase(lightPositions.begin() + (i));
			lightVector.erase(lightVector.begin() + (i));
			return true;
		}
	}
	return false;
}

void cComplexObject::Move(glm::vec3 movement) {
	position += movement;

	for (cModelObject* model : modelVector) {
		model->positionXYZ += movement;
	}
	for (cLightObject* light : lightVector) {
		light->position += glm::vec4(movement, 1.0f);
	}
	for (size_t i = 0; i < collisionPoints.size(); i++) {
		collisionPoints[i].Position += movement;
	}
}

void cComplexObject::Resize() {
	for (cModelObject* model : modelVector) {
		model->positionXYZ -= position;
		model->positionXYZ *= this->scale;
		model->scale *= this->scale;
		model->positionXYZ += position;
	}

	for (cLightObject* light : lightVector) {
		light->position.x -= position.x;
		light->position.y -= position.y;
		light->position.z -= position.z;
		light->position *= this->scale;
		light->position.x += position.x;
		light->position.y += position.y;
		light->position.z += position.z;
	}
}

void cComplexObject::RotateChildren() {
	for (size_t i = 0; i < modelVector.size(); i++)	{
		glm::vec3 direction = glm::rotate(this->qRotation, modelPositions[i]);
		modelVector[i]->positionXYZ = this->position + (direction);
		modelVector[i]->setOrientation(this->qRotation);
	}
	for (size_t i = 0; i < lightVector.size(); i++)	{
		glm::vec3 direction = glm::rotate(this->qRotation, lightPositions[i]);
		lightVector[i]->position = glm::vec4(this->position + (direction), 1.0f);

		/*glm::vec3 Rotation = this->getEulerAngle();
		glm::vec4 lightDirection = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		lightDirection.x = cos(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x));
		lightDirection.y = sin(glm::radians(Rotation.x));
		lightDirection.z = sin(glm::radians(Rotation.y)) * cos(glm::radians(Rotation.x));
		lightVector[i]->direction = glm::normalize(lightDirection);*/
	}
	for (size_t i = 0; i < collisionPoints.size(); i++)	{
		glm::vec3 direction = glm::rotate(this->qRotation, collisionPoints[i].RelativePos);
		collisionPoints[i].Position = this->position + (direction);
	}
}

void cComplexObject::UpdateOrientation(glm::vec3 EulerAngleDegreesXYZ) {
	// Create a quaternion of this angle...
	glm::vec3 EulerAngleRadians;
	EulerAngleRadians.x = glm::radians(EulerAngleDegreesXYZ.x);
	EulerAngleRadians.y = glm::radians(EulerAngleDegreesXYZ.y);
	EulerAngleRadians.z = glm::radians(EulerAngleDegreesXYZ.z);

	glm::quat angleChange = glm::quat(EulerAngleRadians);

	// ...apply it to the exiting rotation
	this->qRotation *= angleChange;
	RotateChildren();
}

void cComplexObject::UpdateOrientation(glm::quat qAngle) {
	this->qRotation *= qAngle;
	RotateChildren();
}

void cComplexObject::UpdateRoll(float offset) {
	this->roll = glm::mod(this->roll + offset, 360.0f);
	UpdateOrientation(glm::vec3(0.0f, 0.0f, offset));
}

unsigned int cComplexObject::getUniqueID(void) { return this->m_uniqueID; }

glm::vec3 cComplexObject::getPosition(void)
{
	return this->position;
}

std::vector<cModelObject*> cComplexObject::GetModels() { return this->modelVector; }
std::vector<cLightObject*> cComplexObject::GetLights() { return this->lightVector; }

std::vector<collisionPoint*> cComplexObject::GetCollisionPoints() {
	std::vector<collisionPoint*> rtn;
	for (size_t i = 0; i < this->collisionPoints.size(); i++) {
		rtn.push_back(&this->collisionPoints[i]);
	}
	return rtn;
}
