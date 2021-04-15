#pragma once
#include <vector>
#include "iMediatorInterface.hpp"
#include "MediatorGlobals.hpp"
#include <map>
#include "cError.hpp"
#include "cPhysics.hpp"
#include "cAABB.hpp"
#include "cModelObject.hpp"
#include "DebugRenderer/cDebugRenderer.hpp"
#include "cGameObjectFactory.hpp"
#include "cUDPServer.hpp"

struct BoundingBox {
	glm::vec3 minCorner;
	glm::vec3 maxCorner;
	glm::quat orientation;
};

class cPhysicsManager : iMediatorInterface {
	cPhysicsManager();
	static cPhysicsManager stonPhysMngr;
	iMediatorInterface* pMediator;

	cError error;

	cPhysics* pPhysics;
	std::map<unsigned long long /*ID*/, cAABB*> g_mapAABBs_World;
	std::vector<glm::vec3> vec_BoundingBox_Test_Points;
	std::vector<glm::vec3> vec_BoundingBox_ClosestPoints;
	//debug aabb cubes with rendered cubes
	cGameObjectFactory ObjectFactory;

	unsigned long long prevAABB;
	std::string currAABB;
	
public:
	
	static cPhysicsManager* GetPhysicsManager();

	void IntegrationStep(std::vector<cModelObject*>& vec_pGameObjects, float deltaTime);
	void TestForCollisions(std::vector<cModelObject*>& vec_pGameObjects);
	void TestForComplexCollisions(std::vector<cComplexObject*> g_vec_pComplexObjects, cDebugRenderer* pDebugRenderer);
	void TestForNetworkBulletCollisions(std::vector<ServerPlayer>& players);

	void CalcAABBsForMeshModel(cModelObject* theModel);
	void AssignTriangle(std::vector<sPlyTriangle>::iterator itTri, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, float maxLength);
	
	void CalculateTransformedMesh(cMesh& originalMesh, glm::mat4 matWorld, cMesh& mesh_transformedInWorld);
	BoundingBox CalculateBoundingBox(cModelObject* theModel);

	void DrawAABBs();
	int TrianglesInCurrentAABB(glm::vec3 pos);
	std::string CurrentAABB(glm::vec3 pos);
	cAABB* GetCurrentAABB();
	cAABB* GetPointAABB(glm::vec3 pos);
	unsigned long long GetPreviousAABB();

	void DrawTestPoints(cComplexObject* Parent);

	// Mediator Functions
	void setMediatorPointer(iMediatorInterface* pMediator);
	virtual sNVData RecieveMessage(sNVData message);
	virtual sData RecieveMessage(sData& data);
};
