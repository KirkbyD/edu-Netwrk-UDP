#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <vector>
#include "Globals.hpp"
#include "cProtocol.h"
#include <random>

#pragma comment(lib, "Ws2_32.lib")

void _PrintWSAError(const char* file, int line);
#define PrintWSAError() _PrintWSAError(__FILE__, __LINE__)

struct ServerPlayer {
	unsigned short port; // their id;
	struct sockaddr_in si_other;
	SOCKET socket;

	int state = -1;
	int currMessage = 0;
	glm::vec3 eulerAngle;
	glm::vec3 front;
	glm::vec3 right;
	std::vector<int> v_inputs_rec;
	std::vector<glm::vec3> v_orientations_rec;

	cComplexObject* PlayerObject; //contains worldspace info
	cModelObject* BulletObject;
	int ShotTick;

	void CalcDirections() {
		this->front.x = cos(glm::radians(this->eulerAngle.x)) * cos(glm::radians(this->eulerAngle.y));
		this->front.y = sin(glm::radians(this->eulerAngle.y));
		this->front.z = sin(glm::radians(this->eulerAngle.x)) * cos(glm::radians(this->eulerAngle.y));
		this->front = glm::normalize(this->front);
		this->right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), this->front));
	}
};

class cUDPServer {
public:
	cUDPServer(void);
	~cUDPServer(void);

	void Update(void);

	std::vector<ServerPlayer> vec_Players;

private:
	void SetNonBlocking(SOCKET socket);
	void ReadData(void);

	void UpdatePlayers(void);
	void BroadcastUpdate(void);

	ServerPlayer* GetPlayerByPort(unsigned short port, sockaddr_in si_other);

	bool mIsRunning;

	FD_SET mReadSet;
	timeval mTv;
	SOCKET mListenSocket;

	Protocol proto;

	double tickCounter;
	unsigned long long totalTicks;

	const float UPDATE_INTERVAL;
	unsigned int numPlayersConnected;
	float accelSpeed;

	std::random_device rd;
};
