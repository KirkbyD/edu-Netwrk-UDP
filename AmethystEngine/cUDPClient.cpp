#include "cUDPClient.hpp"
#include "Globals.hpp"
#include "cProtocol.h"
#include <random>

void _PrintWSAError(const char* file, int line)
{
	int WSAErrorCode = WSAGetLastError();
	wchar_t* s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "[WSAError:%d] %S\n", WSAErrorCode, s);
	LocalFree(s);
}

cUDPClient::cUDPClient(void) 
	: mServerSocket(INVALID_SOCKET),
	UPDATE_INTERVAL(0.2),
	tickCounter(0)
{
	vec_Players.resize(4);
	//for (size_t i = 0; i < vec_Players.size(); i++)
	//{
	//	vec_Players[i].orientations.push_back(glm::vec3(mainCamera.GetYaw(), mainCamera.GetPitch(), g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetRoll()));
	//}
	vec_Bullets.resize(4);
	for (size_t i = 0; i < vec_Players.size(); i++) {
		switch (i) {
		case 0:
			vec_Bullets[i].bulletObject = pFindObjectByFriendlyName("Bullet01");
			break;
		case 1:
			vec_Bullets[i].bulletObject = pFindObjectByFriendlyName("Bullet02");
			break;
		case 2:
			vec_Bullets[i].bulletObject = pFindObjectByFriendlyName("Bullet03");
			break;
		case 3:
			vec_Bullets[i].bulletObject = pFindObjectByFriendlyName("Bullet04");
			break;
		}
	}

	WSAData		WSAData;
	int			iResult;
	int			Port = 5150;
	SOCKADDR_IN ReceiverAddr;

	// Step #0 Initialize WinSock
	iResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (iResult != 0) {
		PrintWSAError();
		return;
	}
}

cUDPClient::~cUDPClient(void)
{
	closesocket(mServerSocket);
	WSACleanup();
}

void cUDPClient::SetNonBlocking(SOCKET socket)
{
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) {
		PrintWSAError();
		return;
	}
}

void cUDPClient::CreateSocket(string ip, int port)
{
	mServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mServerSocket == SOCKET_ERROR) {
		PrintWSAError();
		return;
	}

	memset((char*)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(port);
	si_other.sin_addr.s_addr = inet_addr(ip.c_str());

	SetNonBlocking(mServerSocket);
}


void cUDPClient::Update(void)
{
	// TODO - recieve until you get right ID fomr server
	// also stop sending if 2 packets ahead
	// move some of recv out to check ID's as necessary
	Recv();
}

void cUDPClient::Recv(void) {
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buffer[512];

	int result = recvfrom(mServerSocket, buffer, 512, 0, (struct sockaddr*) & si_other, &slen);
	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			// printf(".");		// Quick test
			return;
		}
		PrintWSAError();

		// For a TCP connection you would close this socket, and remove it from 
		// your list of connections. For UDP we will clear our buffer, and just
		// ignore this.
		memset(buffer, '\0', 512);
		return;
	}

	unsigned short port = si_other.sin_port;

	Buffer buf;
	std::vector<uint8_t> Header;
	for (int i = 0; i < 8; i++)
	{
		Header.push_back(buffer[i]);
	}

	buf.ReceiveBufferContent(Header);

	int packet_length = buf.readInt32LE(INT_SIZE * 0);
	int message_id = buf.readInt32LE(INT_SIZE * 1);

	std::vector<uint8_t> Body;
	for (int i = (INT_SIZE * 2); i < packet_length; i++)
	{
		Body.push_back(buffer[i]);
	}

	buf.ReceiveBufferContent((INT_SIZE * 2), Body);

	printf("Data Recieved: %d\n", packet_length);

	int id = buf.readInt32LE(INT_SIZE * 2);

	int players_size = buf.readInt32LE(INT_SIZE * 3);
	for (size_t i = 0; i < players_size; i++)
	{
		int state = buf.readInt32LE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (0));
		float pos_x = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE));
		float pos_y = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE));
		float pos_z = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 2));
		float vel_x = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 3));
		float vel_y = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 4));
		float vel_z = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 5));
		float ori_x = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 6));
		float ori_y = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 7));
		float ori_z = buf.readFloatLE(INT_SIZE * 4 + (i * PLAYER_SIZE) + (INT_SIZE + FLOAT_SIZE * 8));

		vec_Players[i].state = state;
		vec_Players[i].position = glm::vec3(pos_x, pos_y, pos_z);
		vec_Players[i].velocity = glm::vec3(vel_x, vel_y, vel_z);
		vec_Players[i].orientation = glm::vec3(ori_x, ori_y, ori_z);

		g_vec_pComplexObjects[i]->SetPosition(vec_Players[i].position);
		g_vec_pComplexObjects[i]->SetVelocity(vec_Players[i].velocity);
		g_vec_pComplexObjects[i]->SetOrientation(vec_Players[i].orientation);
		if (state == IS_CONNECTED)
			g_vec_pComplexObjects[i]->SetInverseMass(0.0f);

		cModelObject* iReckon;
		cModelObject* ServerPos;
		switch (i) {
			case 0:
				iReckon = pFindObjectByFriendlyName("DeadReckoned01");
				ServerPos = pFindObjectByFriendlyName("ServerPos01");
				break;
			case 1:
				iReckon = pFindObjectByFriendlyName("DeadReckoned02");
				ServerPos = pFindObjectByFriendlyName("ServerPos02");
				break;
			case 2:
				iReckon = pFindObjectByFriendlyName("DeadReckoned03");
				ServerPos = pFindObjectByFriendlyName("ServerPos03");
				break;
			case 3:
				iReckon = pFindObjectByFriendlyName("DeadReckoned04");
				ServerPos = pFindObjectByFriendlyName("ServerPos04");
				break;
			default:
				//should never happen
				iReckon = pFindObjectByFriendlyName("DeadReckoned01");
				ServerPos = pFindObjectByFriendlyName("ServerPos01");
				break;
		}		
		iReckon->positionXYZ = vec_Players[i].position + vec_Players[i].velocity * 0.2f;
		iReckon->setOrientation(vec_Players[i].orientation);
		ServerPos->positionXYZ = vec_Players[i].position;
		ServerPos->setOrientation(vec_Players[i].orientation);
	}

	int bullets_size = buf.readInt32LE(INT_SIZE * 4 + (players_size * PLAYER_SIZE));
	for (size_t i = 0; i < bullets_size; i++)
	{
		int state = buf.readInt32LE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (0));
		float pos_x = buf.readFloatLE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE));
		float pos_y = buf.readFloatLE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE));
		float pos_z = buf.readFloatLE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE * 2));
		float vel_x = buf.readFloatLE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE * 3));
		float vel_y = buf.readFloatLE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE * 4));
		float vel_z = buf.readFloatLE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (i * BULLET_SIZE) + (INT_SIZE + FLOAT_SIZE * 5));

		vec_Bullets[i].state = state;
		vec_Bullets[i].bulletObject->positionXYZ = glm::vec3(pos_x, pos_y, pos_z);
		vec_Bullets[i].bulletObject->velocity = glm::vec3(vel_x, vel_y, vel_z);
	}

	buf.readInt32LE(INT_SIZE * 5 + (players_size * PLAYER_SIZE) + (vec_Bullets.size() * BULLET_SIZE));
	vpComplexObjectsCurrentPos = buf.readInt32LE(INT_SIZE * 6 + (players_size * PLAYER_SIZE) + (vec_Bullets.size() * BULLET_SIZE));
	mainCamera.SetObject(g_vec_pComplexObjects[vpComplexObjectsCurrentPos]);

	cModelObject* PredictedPos = pFindObjectByFriendlyName("PredictedPos");
	PredictedPos->positionXYZ = vec_Players[vpComplexObjectsCurrentPos].position + vec_Players[vpComplexObjectsCurrentPos].velocity * 0.2f;
	PredictedPos->setOrientation(vec_Players[vpComplexObjectsCurrentPos].orientation);

	//pop off all message up to this one
	int clearIdx = -1;
	for (size_t i = 0; i < vec_PreviousPlayerInputs.size(); i++) {
		if (vec_PreviousPlayerInputs[i].MessageID <= id)
			clearIdx = i;
		else
			break;
	}
	if (clearIdx > 0)
		vec_PreviousPlayerInputs.erase(vec_PreviousPlayerInputs.begin(), vec_PreviousPlayerInputs.begin() + clearIdx);
	else if (clearIdx == 0)
		vec_PreviousPlayerInputs.erase(vec_PreviousPlayerInputs.begin());


	//reapply all controls since
	cComplexObject* playerObj = g_vec_pComplexObjects[vpComplexObjectsCurrentPos];
	float accelSpeed = 180.0f * deltaTime;
	glm::vec3 playerFront;
	glm::vec3 playerRight;

	for (size_t i = 0; i < vec_PreviousPlayerInputs.size(); i++) {
		int rotationCounter = 1;
		glm::vec3 InitialAngle = vec_PreviousPlayerInputs[i].orientations[0];
		playerFront.x = cos(glm::radians(InitialAngle.x)) * cos(glm::radians(InitialAngle.y));
		playerFront.y = sin(glm::radians(InitialAngle.y));
		playerFront.z = sin(glm::radians(InitialAngle.x)) * cos(glm::radians(InitialAngle.y));
		playerFront = glm::normalize(playerFront);
		playerRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), playerFront));

		for (size_t j = 0; j < vec_PreviousPlayerInputs[i].inputs.size(); j++) {
			switch (vec_PreviousPlayerInputs[i].inputs[j]) {
				case FORWARD: {
					//update velocity based on orientation
					//get directional vectors based on quaternion
					glm::vec3 velocity = playerObj->GetVelocity();
					playerObj->SetVelocity(glm::vec3(velocity.x + playerFront.x * accelSpeed,
						velocity.y + playerFront.y * accelSpeed,
						velocity.z + playerFront.z * accelSpeed));
					PredictedPos->velocity = playerObj->GetVelocity();
					break;
				}
				case BACKWARD: {
					glm::vec3 velocity = playerObj->GetVelocity();
					playerObj->SetVelocity(glm::vec3(velocity.x - playerFront.x * accelSpeed,
						velocity.y - playerFront.y * accelSpeed,
						velocity.z - playerFront.z * accelSpeed));
					PredictedPos->velocity = playerObj->GetVelocity();
					break;
				}
				case LEFT: {
					glm::vec3 velocity = playerObj->GetVelocity();
					playerObj->SetVelocity(glm::vec3(velocity.x + playerRight.x * accelSpeed,
						velocity.y,
						velocity.z + playerRight.z * accelSpeed));
					PredictedPos->velocity = playerObj->GetVelocity();
					break;
				}
				case RIGHT: {
					glm::vec3 velocity = playerObj->GetVelocity();
					playerObj->SetVelocity(glm::vec3(velocity.x - playerRight.x * accelSpeed,
						velocity.y,
						velocity.z - playerRight.z * accelSpeed));
					PredictedPos->velocity = playerObj->GetVelocity();
					break;
				}
				case BRAKE: {
					glm::vec3 newVel = playerObj->GetVelocity() * 0.5f;
					playerObj->SetVelocity(newVel);
					PredictedPos->velocity = playerObj->GetVelocity();
					break;
				}
				case ORIENT: {
					glm::vec3 euler = vec_PreviousPlayerInputs[i].orientations[rotationCounter++];
					playerFront.x = cos(glm::radians(euler.x)) * cos(glm::radians(euler.y));
					playerFront.y = sin(glm::radians(euler.y));
					playerFront.z = sin(glm::radians(euler.x)) * cos(glm::radians(euler.y));
					playerFront = glm::normalize(playerFront);
					playerRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), playerFront));
					playerObj->SetOrientation(glm::quatLookAt(-playerFront, glm::vec3(0.0f, 1.0f, 0.0f)));
					PredictedPos->setOrientation(playerObj->getQOrientation());
					break;
				}
				case SHOOT: {
					if (vec_PreviousPlayerInputs[i].state == HAS_AMMO) {
						vec_Bullets[vpComplexObjectsCurrentPos].bulletObject->positionXYZ = playerObj->getPosition();
						vec_Bullets[vpComplexObjectsCurrentPos].bulletObject->velocity = playerObj->GetVelocity() + 2000.0f * playerFront;
						vec_Players[vpComplexObjectsCurrentPos].state = IS_ACTIVE;
					}
					break;
				}
				case RESPAWN: {
					//not predicted on purpose because randomised
					/*if (vec_PreviousPlayerInputs[i].state == IS_CONNECTED) {
						std::random_device rd;
						std::mt19937 mt(rd());
						std::uniform_int_distribution<unsigned int>xzPos(300, 1000);
						std::uniform_int_distribution<unsigned int>yPos(200, 1000);
						playerObj->SetPosition(glm::vec3(xzPos(mt), yPos(mt), xzPos(mt)));
						vec_Players[vpComplexObjectsCurrentPos].state = HAS_AMMO;
						playerObj->SetInverseMass(1.0f);
					}*/
					break;
				}
				default: {
					break;
				}
			}
		}
	}
}

void cUDPClient::Send() {
	this->tickCounter += deltaTime;
	if (this->tickCounter < (UPDATE_INTERVAL)) return;
	this->tickCounter = 0;

	ClientPlayer* me = &vec_Players[vpComplexObjectsCurrentPos];	//[playerNum];

	me->orientations.insert(me->orientations.begin(), glm::vec3(mainCamera.GetYaw(), mainCamera.GetPitch(), g_vec_pComplexObjects[vpComplexObjectsCurrentPos]->GetRoll()));

	vec_PreviousPlayerInputs.push_back(*me);

	Protocol proto;
	proto.UserInput(me->MessageID++, me->inputs, me->orientations);

	me->inputs.clear();
	me->orientations.clear();
	
	Buffer buf;
	buf.ReceiveBufferContent(proto.GetBuffer());

	std::vector<uint8_t> vect = proto.GetBuffer();
	int packet_length = buf.readInt32LE(INT_SIZE * 0);

	int result = sendto(this->mServerSocket, (char*)vect.data(), packet_length, 0,
		(struct sockaddr*) & si_other, sizeof(si_other));

	if (result == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) return;
		PrintWSAError();
		return;
	}
	if (result == 0) {
		printf("Disconnected...\n");
		return;
	}
	printf("Number of bytes sent: %d\n", result);
}