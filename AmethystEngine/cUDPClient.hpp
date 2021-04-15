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
#include <glm/vec3.hpp>

#pragma comment(lib, "Ws2_32.lib")

#include <vector>
#include <string>
#include "cModelObject.hpp"

using std::string;

void _PrintWSAError(const char* file, int line);
#define PrintWSAError() _PrintWSAError(__FILE__, __LINE__)

struct ClientPlayer {
	int state;
	int MessageID = 0;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 orientation;

	std::vector<int> inputs;
	std::vector<glm::vec3> orientations;
};

struct ClientBullet {
	int state; // [ACTIVE]
	cModelObject* bulletObject;
};

extern std::vector<ClientPlayer> vec_Players;
extern std::vector<ClientPlayer> vec_PreviousPlayerInputs;
extern std::vector<ClientBullet> vec_Bullets;

class cUDPClient {
public:
	cUDPClient(void);
	~cUDPClient(void);

	void CreateSocket(string ip, int port);
	void Update(void);

	void Send();

private:
	void SetNonBlocking(SOCKET socket);
	void Recv(void);

	//Protocol proto;
	double tickCounter;
	double UPDATE_INTERVAL;

	SOCKET mServerSocket;
	struct sockaddr_in si_other;
};