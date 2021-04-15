#include "cUDPServer.hpp"

#include <winsock.h>
#include <WS2tcpip.h>
#include "cComplexObject.hpp"
#include <random>

void _PrintWSAError(const char* file, int line) {
	int WSAErrorCode = WSAGetLastError();
	wchar_t* s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "[WSAError:%d] %S\n", WSAErrorCode, s);
	LocalFree(s);
}

cUDPServer::cUDPServer(void) :
	mIsRunning(false), 
	mListenSocket(INVALID_SOCKET),
	numPlayersConnected(0),
	UPDATE_INTERVAL(0.2f),			// 5Hz / 200ms per update / 5 updates per second
	tickCounter(0),
	totalTicks(0)
{
	this->vec_Players.resize(4);

	for (size_t i = 0; i < vec_Players.size(); i++) {
		vec_Players[i].PlayerObject = g_vec_pComplexObjects[i];
		switch (i) {
		case 0:
			vec_Players[i].BulletObject = pFindObjectByFriendlyName("Bullet01");
			break;
		case 1:
			vec_Players[i].BulletObject = pFindObjectByFriendlyName("Bullet02");
			break;
		case 2:
			vec_Players[i].BulletObject = pFindObjectByFriendlyName("Bullet03");
			break;
		case 3:
			vec_Players[i].BulletObject = pFindObjectByFriendlyName("Bullet04");
			break;
		}
	}

	// WinSock vars
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

	// Step #1 Create a socket
	mListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mListenSocket == INVALID_SOCKET) {
		PrintWSAError();
		return;
	}

	// Step #2 Bind our socket
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(Port);
	ReceiverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	iResult = bind(mListenSocket, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));
	if (iResult == SOCKET_ERROR) {
		PrintWSAError();
		return;
	}

	// Set our socket to be nonblocking
	SetNonBlocking(mListenSocket);

	// Our server is ready 
	printf("[SERVER] Receiving IP: %s\n", inet_ntoa(ReceiverAddr.sin_addr));
	printf("[SERVER] Receiving Port: %d\n", htons(ReceiverAddr.sin_port));
	printf("[SERVER] Ready to receive a datagram...\n");

	mIsRunning = true;
} // end UDPServer

cUDPServer::~cUDPServer(void) {
	closesocket(this->mListenSocket);
	WSACleanup();	// <-- Not necessary if quitting application, Windows will handle this.
}

void cUDPServer::SetNonBlocking(SOCKET socket) {
	ULONG NonBlock = 1;
	int result = ioctlsocket(socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR) {
		PrintWSAError();
		return;
	}
}

void cUDPServer::ReadData(void) {
	struct sockaddr_in si_other;
	int slen = sizeof(si_other);
	char buffer[512];

	int result = recvfrom(mListenSocket, buffer, 512, 0, (struct sockaddr*) & si_other, &slen);
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

	ServerPlayer* player = GetPlayerByPort(port, si_other);
	if (player == nullptr)
		return;

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
	for (int i = (INT_SIZE*2); i < packet_length; i++)
	{
		Body.push_back(buffer[i]);
	}

	buf.ReceiveBufferContent((INT_SIZE * 2), Body);

	switch (message_id) {
	case USERINPUT: {
		player->currMessage = buf.readInt32LE(INT_SIZE * 2);

		int input_size = buf.readInt32LE(INT_SIZE * 3);
		std::vector<int> v_inputs_rec;
		for (size_t i = 0; i < input_size; i++)
		{
			int input = buf.readInt32LE(INT_SIZE * 4 + (i * INT_SIZE) + (0));
			player->v_inputs_rec.push_back(input);
		}

		int orientations_size = buf.readInt32LE(INT_SIZE * 4 + (input_size * INT_SIZE));
		for (size_t i = 0; i < orientations_size; i++)
		{
			float ori_x = buf.readFloatLE(INT_SIZE * 5 + (input_size * INT_SIZE) + (i * VEC3_SIZE) + (0));
			float ori_y = buf.readFloatLE(INT_SIZE * 5 + (input_size * INT_SIZE) + (i * VEC3_SIZE) + (FLOAT_SIZE));
			float ori_z = buf.readFloatLE(INT_SIZE * 5 + (input_size * INT_SIZE) + (i * VEC3_SIZE) + (FLOAT_SIZE * 2));
			glm::vec3 ori = glm::vec3(ori_x, ori_y, ori_z);
			player->v_orientations_rec.push_back(ori);
		}
	}
	default:
		break;
	}
}

void cUDPServer::UpdatePlayers(void) {
	for (size_t i = 0; i < vec_Players.size(); i++)
	{
		if (vec_Players[i].state != -1) {
			ServerPlayer* player = &vec_Players[i];
			int rotationCounter = 1;
			accelSpeed = 180.0f * deltaTime;
			for (size_t i = 0; i < player->v_inputs_rec.size(); i++) {
				// switch case on command type
				if (player->state == IS_CONNECTED && player->v_inputs_rec[i] != RESPAWN) {
					//ignore them
				}
				else {
					switch (player->v_inputs_rec[i]) {
					case FORWARD: {
						//update velocity based on orientation
						//get directional vectors based on quaternion
						glm::vec3 velocity = player->PlayerObject->GetVelocity();
						player->PlayerObject->SetVelocity(glm::vec3(velocity.x + player->front.x * accelSpeed,
							velocity.y + player->front.y * accelSpeed,
							velocity.z + player->front.z * accelSpeed));
						break;
					}
					case BACKWARD: {
						glm::vec3 velocity = player->PlayerObject->GetVelocity();
						player->PlayerObject->SetVelocity(glm::vec3(velocity.x - player->front.x * accelSpeed,
							velocity.y - player->front.y * accelSpeed,
							velocity.z - player->front.z * accelSpeed));
						break;
					}
					case LEFT: {
						glm::vec3 velocity = player->PlayerObject->GetVelocity();
						player->PlayerObject->SetVelocity(glm::vec3(velocity.x + player->right.x * accelSpeed,
							velocity.y,
							velocity.z + player->right.z * accelSpeed));
						break;
					}
					case RIGHT: {
						glm::vec3 velocity = player->PlayerObject->GetVelocity();
						player->PlayerObject->SetVelocity(glm::vec3(velocity.x - player->right.x * accelSpeed,
							velocity.y,
							velocity.z - player->right.z * accelSpeed));
						break;
					}
					case BRAKE: {
						glm::vec3 newVel = player->PlayerObject->GetVelocity() * 0.5f;
						player->PlayerObject->SetVelocity(newVel);
						break;
					}
					case ORIENT: {
						glm::vec3 current = player->PlayerObject->getEulerAngle();
						player->eulerAngle = player->v_orientations_rec[rotationCounter++];
						player->CalcDirections();
						player->PlayerObject->SetOrientation(glm::quatLookAt(-player->front, glm::vec3(0.0f, 1.0f, 0.0f)));
						break;
					}
					case SHOOT: {
						if (player->state == HAS_AMMO) {
							player->state = IS_ACTIVE;
							player->BulletObject->positionXYZ = player->PlayerObject->getPosition();
							player->BulletObject->velocity = player->PlayerObject->GetVelocity() + 2000.0f * player->front;
							player->ShotTick = totalTicks;
						}
						break;
					}
					case RESPAWN: {
						if (player->state == IS_CONNECTED) {
							std::mt19937 mt(rd());
							std::uniform_int_distribution<unsigned int>xzPos(300, 1000);
							std::uniform_int_distribution<unsigned int>yPos(200, 1000);
							player->PlayerObject->SetPosition(glm::vec3(xzPos(mt), yPos(mt), xzPos(mt)));
							player->state = HAS_AMMO;
							player->PlayerObject->SetInverseMass(1.0f);
						}
						break;
					}
					default: {
						break;
					}
					}
				}
			}
			player->v_inputs_rec.clear();
			player->v_orientations_rec.clear();
		}

		if ((totalTicks - vec_Players[i].ShotTick) >= 10
			&& vec_Players[i].state == IS_ACTIVE)
			vec_Players[i].state = HAS_AMMO;
	}
}

void cUDPServer::Update(void) {
	if (!this->mIsRunning) return;

	ReadData();

	this->tickCounter += deltaTime;
	if (this->tickCounter < (UPDATE_INTERVAL)) return;
	this->tickCounter = 0;
	this->totalTicks++;

	UpdatePlayers();
	BroadcastUpdate();	
}

void cUDPServer::BroadcastUpdate(void) {
	// create our data to send, then send the same data to all players

	std::vector<Player> v_players;
	std::vector<Bullet> v_bullets;

	for (int i = 0; i < vec_Players.size(); i++) {
		Player protocolPlayer;
		protocolPlayer.state = vec_Players[i].state;
		protocolPlayer.position = vec_Players[i].PlayerObject->GetPosition();
		protocolPlayer.velocity = vec_Players[i].PlayerObject->GetVelocity();
		protocolPlayer.orientation = vec_Players[i].PlayerObject->getEulerAngle();
		v_players.push_back(protocolPlayer);

		Bullet b;
		if (protocolPlayer.state == HAS_AMMO) {
			b.state = HAS_AMMO;
			b.position = vec_Players[i].PlayerObject->GetPosition();
			b.velocity = vec_Players[i].PlayerObject->GetVelocity();
			v_bullets.push_back(b);
		}
		else if (protocolPlayer.state == IS_ACTIVE) {
			b.state = IS_ACTIVE;
			b.position = vec_Players[i].BulletObject->getPosition();
			b.velocity = vec_Players[i].BulletObject->velocity;
			v_bullets.push_back(b);
		}
		else {
			b.state = -1;
			b.position = glm::vec3(1000000.0f, 1000000.0f, 1000000.0f);
			b.velocity = glm::vec3(1000000.0f, 1000000.0f, 1000000.0f);
			v_bullets.push_back(b);
		}
	}

	for (size_t i = 0; i < vec_Players.size(); i++)	{
		if (vec_Players[i].state != -1) {
			proto.GameScene(vec_Players[i].currMessage, v_players, v_bullets, i);
			Buffer buf;
			buf.ReceiveBufferContent(proto.GetBuffer());

			std::vector<uint8_t> vect = proto.GetBuffer();
			int packet_length = buf.readInt32LE(INT_SIZE * 0);

			int result = sendto(mListenSocket, (char*)vect.data(), packet_length, 0,
				(struct sockaddr*) & vec_Players[i].si_other, sizeof(vec_Players[i].si_other));
		
			if (result == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAEWOULDBLOCK) {
					// printf(".");		// Quick test
					return;
				}
				PrintWSAError();

				// For a TCP connection you would close this socket, and remove it from 
				// your list of connections. For UDP we will clear our buffer, and just
				// ignore this.
				return;
			}
		}
	}	

	printf("Updates Broadcasted - Server Tick: %i\n", this->totalTicks);
}

ServerPlayer* cUDPServer::GetPlayerByPort(unsigned short port, struct sockaddr_in si_other)
{
	// If a player with this port is already connected, return it
	for (int i = 0; i < vec_Players.size(); i++) {
		if (vec_Players[i].port == port) return &(vec_Players[i]);
	}

	if (numPlayersConnected >= 4) { //only allow up to 4 connections
		return nullptr;
	}

	// Bind the socket with the server address 
	bind(mListenSocket, (const struct sockaddr*) & si_other,
		sizeof(si_other));

	// Otherwise create a new player, and return that one!
	vec_Players[numPlayersConnected].port = port;
	vec_Players[numPlayersConnected].PlayerObject = g_vec_pComplexObjects[numPlayersConnected];
	vec_Players[numPlayersConnected].si_other = si_other;

	//move player to the game!
	std::mt19937 mt(rd());
	std::uniform_int_distribution<unsigned int>xzPos(300, 1000);
	std::uniform_int_distribution<unsigned int>yPos(200, 1000);
	vec_Players[numPlayersConnected].PlayerObject->SetPosition(glm::vec3(xzPos(mt), yPos(mt), xzPos(mt)));
	vec_Players[numPlayersConnected].state = HAS_AMMO;

	printf("Player %i has connected\n", numPlayersConnected + 1);

	return &(vec_Players[numPlayersConnected++]);
}