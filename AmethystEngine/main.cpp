#pragma region Includes
#include "GLCommon.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <algorithm>
#include "cModelLoader.hpp"			
#include "cVAOManager.hpp"
#include "cModelObject.hpp"
#include "cShaderManager.hpp"
#include <sstream>
#include <limits.h>
#include <float.h>
#include "DebugRenderer/cDebugRenderer.hpp"
#include "cLightHelper.hpp"
#include "cLightManager.hpp"
#include "cFileManager.hpp"
#include "cError.hpp"
#include "cCallbacks.hpp"
#include "cFreeCamera.hpp"
#include "Globals.hpp"
#include "cModelManager.hpp"
#include "cMediator.hpp"
#include "cComplexObject.hpp"
#include "cPhysicsManager.hpp"
#include "cBasicTextureManager.hpp"
#include "cTextRenderer.hpp"
#include "cLowPassFilter.hpp"
#include "cRenderer.hpp"
#include "cObjectCamera.hpp"


#include "cUDPClient.hpp"
#pragma endregion


std::vector<ClientPlayer> vec_Players;
std::vector<ClientBullet> vec_Bullets;
std::vector<ClientPlayer> vec_PreviousPlayerInputs;

#pragma region Text_Renderer_Globals
GLFWwindow* _window;
GLFWwindow* _text_window;
unsigned _program;
unsigned _text_program;
unsigned _uniform_tex;
unsigned _attribute_coord;
unsigned _uniform_color;
unsigned _dp_vbo;
int _window_width, _width;
int _window_height, _height;
const float MAX_HEIGHT = 1200.0f;
FT_Library _ft;
FT_Face _face;
GLfloat _current_y_position;
GLfloat _y_offset = 40.0f;
char _text_buffer[512];
int _xpos, _ypos;

cDebugRenderer* pDebugRenderer;
#pragma endregion


#pragma region Developer_Console_Globals
std::string _cmd_result;
bool isDataDisplay = false;
bool isDebugDisplay = false;
bool isDebugModel = false;
bool isDebugModelNormals = false;
bool isDebugModelTriangles = false;
bool isDebugAABB = true;
bool isDebugAABBTriangles = true;
bool isDebugAABBNormals = true;
bool isDebugCollision = true;
bool isDebugLights = true;
#pragma endregion


#pragma region Global_Containers/Iterators 
int vpLightObjectsCurrentPos = 0;
int vpComplexObjectsCurrentPos = 0;
std::map<std::string, cModelObject*>::iterator itModelObjects;


// Load up my "scene"  (now global)
// OBJECT VECTORS
std::vector<cModelObject*> g_vec_pGameObjects;
std::vector<cLightObject*> g_vec_pLightObjects;
std::vector<cComplexObject*> g_vec_pComplexObjects;
std::vector<cMesh*> g_vec_pMeshObjects;


// MAPS
//std::map<std::string /*FriendlyName*/, cModelObject*> g_map_GameObjectsByFriendlyName;
/**************Textures**************/
std::map<std::string, GLuint> mpTexture;
/**************Textures**************/

std::map<std::string, cMesh*> mpMesh;
std::map<std::string, cLightObject*> mpLight;
std::map<std::string, cModelObject*> mpModel;
std::map<std::string, std::string> mpScripts;
#pragma endregion


bool useFreeCam;
cFreeCamera FreeCamera;
cObjectCamera mainCamera;

double deltaTime = 0.0f;
double lastFrame = 0.0f;


#pragma region Subsystem_Boolean_Toggles
// VARIABLES FOR SELECTING AND CONTROLING VARIOUS ASPECTS OF THE ENGINE AT RUNTIME
//		isDevCons		=		Is Developer Console Enabled?
//		isLightSelect	=		Is Lights Selectable Enabled?
//		isModelSelect	=		Is Models Selectable Enabled?
//		isObjCtrl		=		Is Objects Controllable Enabled?
bool isDevCon = false;
bool isModelSelect = false;
bool isLightSelect = false;
bool isComplexSelect = false;
bool isObjCtrl = true;
#pragma endregion


std::string cmdstr;

// returns NULL (0) if we didn't find it.
cModelObject* pFindObjectByFriendlyName(std::string name);
cModelObject* pFindObjectByFriendlyNameMap(std::string name);


#pragma region ShapeTypes
std::map<std::string, eShapeTypes> g_mp_String_to_ShapeType;

void PopulateShapeTypeMap() {
	g_mp_String_to_ShapeType["CUBE"] = eShapeTypes::CUBE;
	g_mp_String_to_ShapeType["MESH"] = eShapeTypes::MESH;
	g_mp_String_to_ShapeType["PLANE"] = eShapeTypes::PLANE;
	g_mp_String_to_ShapeType["SPHERE"] = eShapeTypes::SPHERE;
	g_mp_String_to_ShapeType["UNKNOWN"] = eShapeTypes::UNKNOWN;
}
#pragma endregion


int main() {

	// UserInput
	{
		//std::vector<int> v_inputs;
		//std::vector<glm::vec3> v_orientations;

		//int input1 = 3;
		//int input2 = 0;
		//glm::vec3 ori1 = glm::vec3(0.0f, 5.5f, 0.0f);
		//glm::vec3 ori2 = glm::vec3(2.111f, 0.0f, 0.0f);

		//v_inputs.push_back(input1);
		//v_inputs.push_back(input2);
		//v_orientations.push_back(ori1);
		//v_orientations.push_back(ori2);

		//Protocol proto;
		//proto.UserInput(150, v_inputs, v_orientations);

		//Buffer buf;
		//buf.ReceiveBufferContent(proto.GetBuffer());

		//int packet_length = buf.readInt32LE(INT_SIZE * 0);
		//int message_id = buf.readInt32LE(INT_SIZE * 1);
		//int id = buf.readInt32LE(INT_SIZE * 2);

		//int input_size = buf.readInt32LE(INT_SIZE * 3);
		//std::vector<int> v_inputs_rec;
		//for (size_t i = 0; i < input_size; i++)
		//{
		//	int input = buf.readInt32LE(INT_SIZE * 4 + (i * INT_SIZE) + (0));
		//	v_inputs_rec.push_back(input);
		//}

		//int orientations_size = buf.readInt32LE(INT_SIZE * 4 + (input_size * INT_SIZE));
		//std::vector<glm::vec3> v_orientations_rec;
		//for (size_t i = 0; i < orientations_size; i++)
		//{
		//	float ori_x = buf.readFloatLE(INT_SIZE * 5 + (input_size * INT_SIZE) + (i * VEC3_SIZE) + (0));
		//	float ori_y = buf.readFloatLE(INT_SIZE * 5 + (input_size * INT_SIZE) + (i * VEC3_SIZE) + (FLOAT_SIZE));
		//	float ori_z = buf.readFloatLE(INT_SIZE * 5 + (input_size * INT_SIZE) + (i * VEC3_SIZE) + (FLOAT_SIZE * 2));
		//	glm::vec3 ori = glm::vec3(ori_x, ori_y, ori_z);
		//	v_orientations_rec.push_back(ori);
		//}
	}

	// GameScene
	{
		//std::vector<Player> v_players;
		//std::vector<Bullet> v_bullets;

		//Player p1, p2;
		//p1.state = 1;
		//p1.position = glm::vec3(-1.5f, 0.0f, 500.11111f);
		//p1.velocity = glm::vec3(0.0f, 1.0f, 0.0f);
		//p1.orientation = glm::vec3(50.0f, 0.0f, 0.0f);
		//p2.state = 3;
		//p2.position = glm::vec3(0.0f, 5.0f, 0.0f);
		//p2.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		//p2.orientation = glm::vec3(0.0f, 0.0f, 20.0f);

		//Bullet b1, b2, b3;
		//b1.state = 1;
		//b1.position = glm::vec3(-1.5f, 0.0f, 500.11111f);
		//b1.velocity = glm::vec3(0.0f, 1.0f, 0.0f);
		//b2.state = 3;
		//b2.position = glm::vec3(0.0f, 5.0f, 0.0f);
		//b2.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		//b3.state = 0;
		//b3.position = glm::vec3(0.0f, 10.0f, 0.0f);
		//b3.velocity = glm::vec3(0.0f, 0.0f, -1.0f);

		//v_players.push_back(p1);
		//v_players.push_back(p2);
		//v_bullets.push_back(b1);
		//v_bullets.push_back(b2);
		//v_bullets.push_back(b3);

		//Protocol proto;
		//proto.GameScene(150, v_players, v_bullets);

		//Buffer buf;
		//buf.ReceiveBufferContent(proto.GetBuffer());

		//int packet_length = buf.readInt32LE(INT_SIZE * 0);
		//int message_id = buf.readInt32LE(INT_SIZE * 1);
		//int id = buf.readInt32LE(INT_SIZE * 2);

		//int players_size = buf.readInt32LE(INT_SIZE * 3);
		//std::vector<Player> v_players_rec;
		//for (size_t i = 0; i < players_size; i++)
		//{
		//	int state = buf.readInt32LE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (0));
		//	float pos_x = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE));
		//	float pos_y = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE));
		//	float pos_z = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 2));
		//	float vel_x = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 3));
		//	float vel_y = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 4));
		//	float vel_z = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 5));
		//	float ori_x = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 6));
		//	float ori_y = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 7));
		//	float ori_z = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 8));
		//	Player p;
		//	p.state = state;
		//	p.position = glm::vec3(pos_x, pos_y, pos_z);
		//	p.velocity = glm::vec3(vel_x, vel_y, vel_z);
		//	p.orientation = glm::vec3(ori_x, ori_y, ori_z);
		//	v_players_rec.push_back(p);
		//}

		//int bullets_size = buf.readInt32LE(INT_SIZE * 4 + (players_size * PLAYER_SIZE));
		//std::vector<Bullet> v_bullets_rec;
		//for (size_t i = 0; i < bullets_size; i++)
		//{
		//	int state = buf.readInt32LE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (0));
		//	float pos_x = buf.readFloatLE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE));
		//	float pos_y = buf.readFloatLE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE));
		//	float pos_z = buf.readFloatLE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE * 2));
		//	float vel_x = buf.readFloatLE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE * 3));
		//	float vel_y = buf.readFloatLE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE * 4));
		//	float vel_z = buf.readFloatLE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE * 5));
		//	Bullet b;
		//	b.state = state;
		//	b.position = glm::vec3(pos_x, pos_y, pos_z);
		//	b.velocity = glm::vec3(vel_x, vel_y, vel_z);
		//	v_bullets_rec.push_back(b);
		//}
	}


	PopulateShapeTypeMap();

	cError errmngr;
	cFileManager* pFileManager = cFileManager::GetFileManager();
	cLightManager* pLightManager = cLightManager::GetLightManager();
	cModelManager* pModelManager = cModelManager::GetModelManager();
	cPhysicsManager* pPhysicsManager = cPhysicsManager::GetPhysicsManager();
	cTextRenderer* pTextRenderer = new cTextRenderer();

	cMediator* pMediator = new cMediator();
	pMediator->ConnectEndpointClients();

	cRenderer* pRenderer = new cRenderer();
	cBasicTextureManager* pTextureManager = new cBasicTextureManager();

	std::map<std::string, cShaderManager::cShader> mpVertexShader;
	std::map<std::string, cShaderManager::cShader> mpFragmentShader;

	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 4);

	_window = glfwCreateWindow(1280, 960, "Simple example", NULL, NULL);

	if (!_window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(_window, key_callback);
	glfwSetMouseButtonCallback(_window, mouse_button_callback);
	glfwSetCursorPosCallback(_window, cursor_position_callback);
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// for window transparency & stops user manipulation of devcon window
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	_text_window = glfwCreateWindow(1280, 960, "Dev Console", NULL, _window);
	// change the windows height and width based off of the main windows


	if (!_text_window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwGetWindowPos(_window, &_xpos, &_ypos);
	glfwSetWindowPos(_text_window, _xpos, _ypos);
	glfwGetWindowSize(_window, &_width, &_height);
	glfwSetWindowSize(_text_window, _width, _height);
	glfwGetWindowSize(_text_window, &_window_width, &_window_height);

	glfwMakeContextCurrent(_window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);
	glfwHideWindow(_text_window);

	//glfwSetWindowOpacity(_text_window, 0.7f);

	cDebugRenderer* pDebugRenderer = new cDebugRenderer();
	pDebugRenderer->initialize();

	cModelLoader* pTheModelLoader = new cModelLoader();	// Heap
	cShaderManager* pTheShaderManager = new cShaderManager();

	pTextRenderer->InitFreetype();

	pFileManager->LoadShaders(pFileManager->GetShadersFile(), mpVertexShader, mpFragmentShader);

	// change LoadShaders to bool and return true or false based off of success, then change below to return -1 if false.
	// error will display from the LoadShaders function.
	if (!pTheShaderManager->createProgramFromFile("SimpleShader", mpVertexShader["exampleVertexShader01"], mpFragmentShader["exampleFragmentShader01"])) {
		errmngr.DisplayError("Main", __func__, "shader_compile_failure", "Could not compile shader", pTheShaderManager->getLastError());
		return -1;
	}
	if (!pTheShaderManager->createProgramFromFile("SimpleTextShader", mpVertexShader["exampleTextVertexShader01"], mpFragmentShader["exampleTextFragmentShader01"])) {
		errmngr.DisplayError("Main", __func__, "shader_compile_failure", "Could not compile shader", pTheShaderManager->getLastError());
		return -1;
	}

	GLuint shaderProgID = pTheShaderManager->getIDFromFriendlyName("SimpleShader");
	GLuint textShaderProgID = pTheShaderManager->getIDFromFriendlyName("SimpleTextShader");

	// Create a VAO Manager...
	cVAOManager* pTheVAOManager = new cVAOManager();

	// move this configuration into a seperate file that loads from a configuration file.
	// Note, the "filename" here is really the "model name" that we can look up later (i.e. it doesn't have to be the file name)

	pFileManager->LoadModelsOntoGPU(pTheModelLoader, pTheVAOManager, shaderProgID, pFileManager->GetMeshFile(), mpMesh);

	//move the physics stuff into a physics.ini so that it can be loaded in the physics engine instead of the graphics engine portion of the game engine file loading.
	// At this point, the model is loaded into the GPU

	// make a call to the filemanager to create the gameobjects from the loaded file.
	pFileManager->BuildObjects(pFileManager->GetModelFile());


	/**************Textures**************/
	pFileManager->LoadTexturesOntoGPU(pTextureManager, pFileManager->GetTextureFile(), mpTexture);
	pFileManager->LoadSkyboxTexturesOntoGPU(pTextureManager, pFileManager->GetSkyboxTextureFile(), mpTexture);
	/**************Textures**************/

	// Enable depth test
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwMakeContextCurrent(_text_window);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
	glfwMakeContextCurrent(_window);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	cLowPassFilter avgDeltaTimeThingy;

	cLightHelper* pLightHelper = new cLightHelper();

	// Get the initial time
	double lastTime = glfwGetTime();

	pFileManager->BuildObjects(pFileManager->GetLightsFile());

	itModelObjects = mpModel.begin();

	//BUILD COMPLEX OBJECTS
	pFileManager->BuildObjects(pFileManager->GetComplexObjFile());

	if (g_vec_pComplexObjects.size() != 0) {
		mainCamera.SetObject(g_vec_pComplexObjects[0]);
		mainCamera.SetFollowDistance(15.0f);
		isComplexSelect = true;
	}
	else {
		useFreeCam = true;
	}
	FreeCamera.SetEye(glm::vec3(0.0f, 0.0f, 0.0f));
	FreeCamera.SetEye(glm::vec3(1250.0f, 3500.0f, 1500.0f));
	FreeCamera.SetTarget(pFindObjectByFriendlyName("Puget Sound"));
	FreeCamera.MoveFront();


	glfwSetWindowTitle(_window, "Amethyst Client");

	_uniform_tex = glGetUniformLocation(textShaderProgID, "tex");
	_attribute_coord = glGetAttribLocation(textShaderProgID, "coord");
	_uniform_color = glGetUniformLocation(textShaderProgID, "color");
	glGenBuffers(1, &_dp_vbo);

	for (cModelObject* Model : g_vec_pGameObjects) {
		if (Model->physicsShapeType == AABB)
			pPhysicsManager->CalcAABBsForMeshModel(Model);
	}

	/*************** UDP CLIENT ***************/
	cUDPClient pUDPClient;
	pUDPClient.CreateSocket("127.0.0.1", 5150);
	/*************** UDP CLIENT ***************/

	lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(_window)) {
		// Clear both the colour buffer (what we see) and the depth (or z) buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/***************************************/

		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);
		glViewport(0, 0, width, height);

		/***************************************/

		// Get the initial time
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		const double SOME_HUGE_TIME = 0.1;	// 100 ms;
		if (deltaTime > SOME_HUGE_TIME)
		{
			deltaTime = SOME_HUGE_TIME;
		}

		avgDeltaTimeThingy.addValue(deltaTime);


		/***************************************/
		glm::mat4 ProjectionMatrix, ViewMatrix;

		float ratio;
		ratio = (height > 0.0f) ? (width / (float)height) : 0.0f;

		// Projection matrix
		ProjectionMatrix = glm::perspective(0.6f,		// FOV
			ratio,			// Aspect ratio
			0.1f,			// Near clipping plane
			10000.0f);		// Far clipping plane

		// View matrix
		if (useFreeCam) {
			//FreeCamera.MoveFront();
			ViewMatrix = FreeCamera.GetViewMatrix();
		}
		else {
			mainCamera.Move();
			ViewMatrix = mainCamera.GetViewMatrix();
		}

		GLint matView_UL = glGetUniformLocation(shaderProgID, "matView");
		GLint matProj_UL = glGetUniformLocation(shaderProgID, "matProj");

		glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
		glUniformMatrix4fv(matProj_UL, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

		/***************************************/

		// Set the position of my "eye" (the camera)
		GLint eyeLocation_UL = glGetUniformLocation(shaderProgID, "eyeLocation");

		glm::vec3 CameraPosition;
		if (useFreeCam) {
			CameraPosition = FreeCamera.GetEye();
		}
		else {
			CameraPosition = mainCamera.GetEye();
		}

		glUniform4f(eyeLocation_UL,
			CameraPosition.x,
			CameraPosition.y,
			CameraPosition.z,
			1.0f);

		/***************************************/

		pLightManager->LoadLightsToGPU(g_vec_pLightObjects, shaderProgID);

		/***************************************/

		// Loop to draw everything in the scene
		// TODO: move into a render function / class
		for (int index = 0; index != ::g_vec_pGameObjects.size(); index++) {
			cModelObject* pCurrentObject = ::g_vec_pGameObjects[index];
			if (pCurrentObject->isVisible) {
				if (pCurrentObject->friendlyName == "skybox")
				{
					pCurrentObject->positionXYZ = CameraPosition;
					pCurrentObject->recalculateWorldMatrix();
					pRenderer->RenderSkyBoxObject(pCurrentObject, shaderProgID, pTheVAOManager);
				}
				else
				{
					pCurrentObject->recalculateWorldMatrix();
					pRenderer->Render3DObject(pCurrentObject, shaderProgID, pTheVAOManager);
				}
			}
		}

		/***************************************/

		//DEBUG Drawing - hook up to a mediator boolean
		if (isDebugDisplay) {
			// Bounds:		Cyan	glm::vec3(0.0f, 1.0f, 1.0f)
			// Collision:	Red		glm::vec3(1.0f, 0.0f, 0.0f)
			// Triangles:	Purple	glm::vec3(1.0f, 0.0f, 1.0f)
			// Normals:		Yellow	glm::vec3(1.0f, 1.0f, 0.0f)
			// ModelCenter:	Green	glm::vec3(0.0f, 1.0f, 0.0f)
			// Lights:		White	glm::vec3(1.0f, 1.0f, 1.0f)
			std::vector<collisionPoint*> thePoints = g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetCollisionPoints();

			if (isDebugAABB) {
				std::set<cAABB*> setAABBs;
				for (size_t i = 0; i < thePoints.size(); i++) {
					cAABB* theAABB = pPhysicsManager->GetPointAABB(thePoints[i]->Position);
					if (theAABB != nullptr)
						setAABBs.emplace(theAABB);
				}
				//AABB
				for (cAABB* theAABB : setAABBs) {
					//Box Min / Max - Cyan
					float sideLength = theAABB->GetLength();
					//min Corner
					{
						glm::vec3 minCorner = theAABB->GetMin();
						glm::vec3 minX = minCorner;
						minX.x += sideLength;
						pDebugRenderer->addLine(minCorner, minX, glm::vec3(0.0f, 1.0f, 1.0f), 0.1f);
						glm::vec3 minY = minCorner;
						minY.y += sideLength;
						pDebugRenderer->addLine(minCorner, minY, glm::vec3(0.0f, 1.0f, 1.0f), 0.1f);
						glm::vec3 minZ = minCorner;
						minZ.z += sideLength;
						pDebugRenderer->addLine(minCorner, minZ, glm::vec3(0.0f, 1.0f, 1.0f), 0.1f);
					}
					//max Corner
					{
						glm::vec3 maxCorner = theAABB->GetMax();
						glm::vec3 maxX = maxCorner;
						maxX.x -= sideLength;
						pDebugRenderer->addLine(maxCorner, maxX, glm::vec3(0.0f, 1.0f, 1.0f), 0.1f);
						glm::vec3 maxY = maxCorner;
						maxY.y -= sideLength;
						pDebugRenderer->addLine(maxCorner, maxY, glm::vec3(0.0f, 1.0f, 1.0f), 0.1f);
						glm::vec3 maxZ = maxCorner;
						maxZ.z -= sideLength;
						pDebugRenderer->addLine(maxCorner, maxZ, glm::vec3(0.0f, 1.0f, 1.0f), 0.1f);
					}

					//AABB Contents
					if (isDebugAABBTriangles || isDebugAABBNormals) {
						for (size_t i = 0; i < theAABB->vec_pAABBTriangles.size(); i++) {
							//Triangle - Purple
							if (isDebugAABBTriangles) {
								pDebugRenderer->addTriangle(theAABB->vec_pAABBTriangles[i].vertsPos[0],
									theAABB->vec_pAABBTriangles[i].vertsPos[1],
									theAABB->vec_pAABBTriangles[i].vertsPos[2],
									glm::vec3(1.0f, 0.0f, 1.0f));
							}

							if (isDebugAABBNormals) {
								//Normals - Yellow
								glm::vec3 triNorm = glm::normalize(theAABB->vec_pAABBTriangles[i].vertsNorm[0] + theAABB->vec_pAABBTriangles[i].vertsNorm[1] + theAABB->vec_pAABBTriangles[i].vertsNorm[2]);
								glm::vec3 triCenter = (theAABB->vec_pAABBTriangles[i].vertsPos[0] +
									theAABB->vec_pAABBTriangles[i].vertsPos[1] +
									theAABB->vec_pAABBTriangles[i].vertsPos[2]) / 3.0f;
								pDebugRenderer->addLine(triCenter,
									(triCenter + triNorm * 5.0f),
									glm::vec3(1.0f, 1.0f, 0.0f),
									0.1f);
							}
						}
					}
				}
			}

			//Draw collision Points - red
			if (isDebugCollision) {
				for (int i = 0; i < g_vec_pComplexObjects.size(); i++) {
					std::vector<collisionPoint*> Points = g_vec_pComplexObjects[i]->GetCollisionPoints();
					for (int j = 0; j < Points.size(); j++) {
						if (Points[j]->Collided >= 5.0f) {
							pDebugRenderer->addPointPointSize(Points[j]->Position, glm::vec3(1.0f, 0.0f, 0.0f), 5.0f);
							Points[j]->Collided = -1.0f;
						}
						else if (Points[j]->Collided >= 0.0f) {
							pDebugRenderer->addPointPointSize(Points[j]->Position, glm::vec3(1.0f, 0.0f, 1.0f), 10.0f);
							Points[j]->Collided += deltaTime;
						}
						else
							pDebugRenderer->addPointPointSize(Points[j]->Position, glm::vec3(1.0f, 0.0f, 0.0f), 5.0f);
					}
				}
			}

			if (isDebugModel) {
				//Model center point
				//(*itModelObjects).second
				pDebugRenderer->addPointPointSize((*itModelObjects).second->positionXYZ, glm::vec3(0.0f, 1.0f, 0.0f), 5.0f);

				//render bounding box
				//recalcs model world matrix:
				//BoundingBox modelBB = pPhysicsManager->CalculateBoundingBox((*itModelObjects).second);

				if (isDebugModelNormals || isDebugModelTriangles) {
					cMesh theMesh;
					pPhysicsManager->CalculateTransformedMesh(*mpMesh[(*itModelObjects).second->meshName], (*itModelObjects).second->matWorld, theMesh);
					for (size_t i = 0; i < theMesh.vecTriangles.size(); i++) {
						sPlyTriangle theTriangle = theMesh.vecTriangles[i];

						glm::vec3 v1 = glm::vec3(theMesh.vecVertices[theTriangle.vert_index_1].x, theMesh.vecVertices[theTriangle.vert_index_1].y, theMesh.vecVertices[theTriangle.vert_index_1].z);
						glm::vec3 v2 = glm::vec3(theMesh.vecVertices[theTriangle.vert_index_2].x, theMesh.vecVertices[theTriangle.vert_index_2].y, theMesh.vecVertices[theTriangle.vert_index_2].z);
						glm::vec3 v3 = glm::vec3(theMesh.vecVertices[theTriangle.vert_index_3].x, theMesh.vecVertices[theTriangle.vert_index_3].y, theMesh.vecVertices[theTriangle.vert_index_3].z);

						if (isDebugModelNormals) {
							glm::vec3 n1 = glm::vec3(theMesh.vecVertices[theTriangle.vert_index_1].nx, theMesh.vecVertices[theTriangle.vert_index_1].ny, theMesh.vecVertices[theTriangle.vert_index_1].nz);
							glm::vec3 n2 = glm::vec3(theMesh.vecVertices[theTriangle.vert_index_2].nx, theMesh.vecVertices[theTriangle.vert_index_2].ny, theMesh.vecVertices[theTriangle.vert_index_2].nz);
							glm::vec3 n3 = glm::vec3(theMesh.vecVertices[theTriangle.vert_index_2].nx, theMesh.vecVertices[theTriangle.vert_index_2].ny, theMesh.vecVertices[theTriangle.vert_index_2].nz);
							glm::vec3 triNorm = glm::normalize(n1 + n2 + n3);
							glm::vec3 triCenter = (v1 + v2 + v3) / 3.0f;
							pDebugRenderer->addLine(triCenter,
								(triCenter + triNorm * 5.0f),
								glm::vec3(1.0f, 1.0f, 0.0f),
								0.1f);
						}

						if (isDebugModelTriangles) {
							pDebugRenderer->addTriangle(v1, v2, v3, glm::vec3(1.0f, 0.0f, 1.0f));
						}
					}
				}
			}

			if (isDebugLights) {
				for (size_t i = 0; i < g_vec_pLightObjects.size(); i++) {
					pDebugRenderer->addPointPointSize(g_vec_pLightObjects[i]->position, glm::vec3(1.0f, 1.0f, 1.0f), 10.0f);
				}

				float sphereSize = pLightHelper->calcApproxDistFromAtten(
					0.95f,		// 95% brightness
					0.001f,		// Within 0.1%
					100000.0f,	// Will quit when it's at this distance
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.x,
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.y,
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.z);
				pDebugRenderer->addDebugSphere(g_vec_pLightObjects[vpLightObjectsCurrentPos]->position, glm::vec3(0.95f, 0.95f, 0.95f), sphereSize);

				sphereSize = pLightHelper->calcApproxDistFromAtten(
					0.75f,		// 75% brightness
					0.001f,		// Within 0.1%
					100000.0f,	// Will quit when it's at this distance
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.x,
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.y,
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.z);
				pDebugRenderer->addDebugSphere(g_vec_pLightObjects[vpLightObjectsCurrentPos]->position, glm::vec3(0.75f, 0.75f, 0.75f), sphereSize);

				sphereSize = pLightHelper->calcApproxDistFromAtten(
					0.50f,		// 50% brightness
					0.001f,		// Within 0.1%
					100000.0f,	// Will quit when it's at this distance
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.x,
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.y,
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.z);
				pDebugRenderer->addDebugSphere(g_vec_pLightObjects[vpLightObjectsCurrentPos]->position, glm::vec3(0.5f, 0.5f, 0.5f), sphereSize);

				sphereSize = pLightHelper->calcApproxDistFromAtten(
					0.25f,		// 25% brightness
					0.001f,		// Within 0.1%
					100000.0f,	// Will quit when it's at this distance
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.x,
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.y,
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.z);
				pDebugRenderer->addDebugSphere(g_vec_pLightObjects[vpLightObjectsCurrentPos]->position, glm::vec3(0.25f, 0.25f, 0.25f), sphereSize);

				sphereSize = pLightHelper->calcApproxDistFromAtten(
					0.01f,		// 1% brightness (essentially black)
					0.001f,		// Within 0.1%
					100000.0f,	// Will quit when it's at this distance
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.x,
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.y,
					g_vec_pLightObjects[vpLightObjectsCurrentPos]->atten.z);
				pDebugRenderer->addDebugSphere(g_vec_pLightObjects[vpLightObjectsCurrentPos]->position, glm::vec3(0.1f, 0.1f, 0.1f), sphereSize);
			}

			pDebugRenderer->RenderDebugObjects(ViewMatrix, ProjectionMatrix, 0.01f);
		}

		/***************************************/
		glm::vec3 currentNosePos = g_vec_pComplexObjects[0]->GetCollisionPoints()[0]->Position;
		std::string currentAABB = pPhysicsManager->CurrentAABB(currentNosePos);
		int currentTriangles = pPhysicsManager->TrianglesInCurrentAABB(currentNosePos);


		// Update the objects through physics
		pPhysicsManager->IntegrationStep(::g_vec_pGameObjects, deltaTime);
		pPhysicsManager->TestForCollisions(::g_vec_pGameObjects);
		pPhysicsManager->TestForComplexCollisions(::g_vec_pComplexObjects, pDebugRenderer);

		/*************** UDP CLIENT ***************/
		pUDPClient.Send();
		pUDPClient.Update();
		/*************** UDP CLIENT ***************/

		glfwSwapBuffers(_text_window);
		glfwSwapBuffers(_window);
		glfwPollEvents();
	}
	glfwDestroyWindow(_text_window);
	glfwDestroyWindow(_window);
	glfwTerminate();

	// Delete everything
	delete pRenderer;
	delete pTextureManager;
	delete pTextRenderer;
	delete pTheModelLoader;
	delete pDebugRenderer;
	delete pLightHelper;
	exit(EXIT_SUCCESS);
}


// returns NULL (0) if we didn't find it.
cModelObject* pFindObjectByFriendlyName(std::string name) {
	// Do a linear search 
	for (unsigned int index = 0; index != g_vec_pGameObjects.size(); index++) {
		if (::g_vec_pGameObjects[index]->friendlyName == name) {
			// Found it!!
			return ::g_vec_pGameObjects[index];
		}
	}
	// Didn't find it
	return NULL;
}

// returns NULL (0) if we didn't find it.
cModelObject* pFindObjectByFriendlyNameMap(std::string name) {
	return ::mpModel[name];
}