#pragma once
#include "cModelObject.hpp"
#include "cLightObject.hpp"
#include "iGameObject.hpp"
#include <vector>
#include <string>
#include <glm/gtx/quaternion.hpp>

struct collisionPoint {
	glm::vec3 RelativePos;
	glm::vec3 Position;
	float Collided;
};

class cComplexObject : public iGameObject {
private:
	// Graphics
	float scale;
	glm::vec3 position;
	glm::vec3 prevPosition;
	glm::quat qRotation;		// Orientation as a quaternion
	float roll;

	// Physics
	float inverseMass;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	std::vector<collisionPoint> collisionPoints;

	// Members
	std::vector<glm::vec3> modelPositions;
	std::vector<cModelObject*> modelVector;
	std::vector<glm::vec3> lightPositions;
	std::vector<cLightObject*> lightVector;

	void RotateChildren();

public:
	cComplexObject(std::vector<cComplexObject*> &vObjects);
	~cComplexObject();

	float GetScale();
	glm::vec3 GetPosition();
	glm::vec3 GetPreviousPosition();
	glm::quat getQOrientation();
	glm::vec3 getEulerAngle();
	float GetRoll();
	float GetInverseMass();
	glm::vec3 GetVelocity();
	glm::vec3 GetAcceleration();
	std::vector<cModelObject*> GetModels();
	std::vector<cLightObject*> GetLights();
	std::vector<collisionPoint*> GetCollisionPoints();

	void SetScale(float val);
	void SetPosition(glm::vec3 posVec);
	void SetPreviousPosition(glm::vec3 posVec);
	void SetInverseMass(float val);
	void SetVelocity(glm::vec3 velocity);
	void SetAcceleration(glm::vec3 accel);
	void SetOrientation(glm::vec3 EulerAngleDegreesXYZ);
	void SetOrientation(glm::quat qAngle);

	void AddLight(std::string);
	void AddLight(cLightObject* theLight);
	void AddModel(std::string);		// has physics
	void AddModel(cModelObject* theModel);
	void AddCollisionPoint(glm::vec3 thePoint);
	//void AddPhysics(std::string);
	//void AddAudio(std::string);
	bool RemoveLight(cLightObject* theLight);

	void Move(glm::vec3 movement);
	void Resize();
	void UpdateOrientation(glm::vec3 EulerAngleDegreesXYZ);
	void UpdateOrientation(glm::quat qAngle);
	void UpdateRoll(float offset);

	/*Interface*/
	virtual unsigned int getUniqueID(void);
	virtual glm::vec3 getPosition(void);
};